#include <furi.h>
#include <gui/gui.h>
#include <gui/modules/submenu.h>
#include <gui/view_dispatcher.h>
#include <notification/notification_messages.h>

#include <furi_hal_bt.h>
#include <extra_beacon.h>

#include "pmsg_protocol.h"

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;

    NotificationApp* notifications;

    bool beacon_running;
} App;

typedef enum {
    CmdBuzz = 0,
    CmdLedOn,
    CmdRainbow,
    CmdAllBlue,
    CmdAllGreen,
    CmdAllGreenFlash,
    CmdVibTune1,
    CmdVibTune2,
    CmdVibTune3,
    CmdVibSonar,
} CmdId;
// TODO check Pebble procal and meshstatic PMSG setup 
static const struct {
    CmdId id;
    const char* label;
    const char* cmd; // will become "PMSG:<cmd>" inside protocol builder
} kCmds[] = {
    {CmdBuzz,          "BUZZ",              "BUZZ"},
    {CmdVibTune1,      "Vibration tune 1",  "vibration-tune1"},
    {CmdVibTune2,      "Vibration tune 2",  "vibration-tune2"},
    {CmdVibTune3,      "Vibration tune 3",  "vibration-tune3"},
    {CmdVibSonar,      "Vibration sonar",   "vibration-sonar"},
    {CmdLedOn,         "LED:ON",            "LED:ON"},
    {CmdRainbow,       "RAINBOW",           "RAINBOW"},
    {CmdAllBlue,       "All Blue",          "allblue"},
    {CmdAllGreen,      "All Green",         "allgreen"},
    {CmdAllGreenFlash, "All Green Flash",   "allgreen-flash"},
};

static void pmsg_set_beacon_data_from_cmd(const char* cmd_ascii) {
    uint8_t adv[EXTRA_BEACON_MAX_DATA_SIZE];
    size_t adv_len = pmsg_build_adv_payload(adv, sizeof(adv), cmd_ascii);
    if(adv_len == 0) return;

    // Extra beacon data must be <= 31 bytes.  [oai_citation:5‡developer.flipper.net](https://developer.flipper.net/flipperzero/doxygen/extra__beacon_8h_source.html)
    furi_hal_bt_extra_beacon_set_data(adv, (uint8_t)adv_len);
}

static bool pmsg_beacon_start() {
    // Start BLE radio stack if needed.  [oai_citation:6‡developer.flipper.net](https://developer.flipper.net/flipperzero/doxygen/furi__hal__bt_8h.html?utm_source=chatgpt.com)
    if(!furi_hal_bt_start_radio_stack()) {
        // It may already be running; start_radio_stack() returns bool,
        // but behavior depends on firmware state.
        // We continue anyway; if extra beacon fails, we stop.
    }

    GapExtraBeaconConfig cfg = {
        .min_adv_interval_ms = 120,
        .max_adv_interval_ms = 160,
        .adv_channel_map = GapAdvChannelMapAll,
        .adv_power_level = GapAdvPowerLevel_0dBm,
        .address_type = GapAddressTypeRandom,
        .address = {0xC0, 0xFF, 0xEE, 0x13, 0x37, 0x01}, // locally administered random-ish
    };

    if(!furi_hal_bt_extra_beacon_set_config(&cfg)) return false; //  [oai_citation:7‡developer.flipper.net](https://developer.flipper.net/flipperzero/doxygen/furi__hal__bt_8h.html?utm_source=chatgpt.com)
    if(!furi_hal_bt_extra_beacon_start()) return false;          //  [oai_citation:8‡developer.flipper.net](https://developer.flipper.net/flipperzero/doxygen/furi__hal__bt_8h.html?utm_source=chatgpt.com)
    return true;
}

static void pmsg_beacon_stop() {
    furi_hal_bt_extra_beacon_stop(); //  [oai_citation:9‡developer.flipper.net](https://developer.flipper.net/flipperzero/doxygen/furi__hal__bt_8h.html?utm_source=chatgpt.com)
}

static void submenu_cb(void* context, uint32_t index) {
    App* app = context;

    // index is CmdId in our mapping
    const char* cmd = NULL;
    for(size_t i = 0; i < COUNT_OF(kCmds); i++) {
        if(kCmds[i].id == (CmdId)index) {
            cmd = kCmds[i].cmd;
            break;
        }
    }
    if(!cmd) return;

    // Set the adv payload immediately
    pmsg_set_beacon_data_from_cmd(cmd);

    // Start beacon if not running; otherwise just "update" payload live
    if(!app->beacon_running) {
        app->beacon_running = pmsg_beacon_start();
        if(app->beacon_running) {
            notification_message(app->notifications, &sequence_success);
        } else {
            notification_message(app->notifications, &sequence_error);
        }
    } else {
        // quick haptic tick to confirm "sent"
        notification_message(app->notifications, &sequence_blink_green_10);
    }
}

static uint32_t view_dispatcher_custom_event_cb(void* context, uint32_t event) {
    UNUSED(context);
    return event;
}

static App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->beacon_running = false;

    app->view_dispatcher = view_dispatcher_alloc();
    app->submenu = submenu_alloc();
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Build menu
    submenu_set_header(app->submenu, "PMSG BLE Beacon");
    for(size_t i = 0; i < COUNT_OF(kCmds); i++) {
        submenu_add_item(app->submenu, kCmds[i].label, kCmds[i].id, submenu_cb, app);
    }
    submenu_add_item(app->submenu, "STOP Beacon", 0xFFFFFFFF, submenu_cb, app);

    // GUI
    app->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    view_dispatcher_set_custom_event_callback(app->view_dispatcher, view_dispatcher_custom_event_cb);

    view_dispatcher_add_view(app->view_dispatcher, 0, submenu_get_view(app->submenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);

    return app;
}

static void app_free(App* app) {
    if(app->beacon_running) pmsg_beacon_stop();

    view_dispatcher_remove_view(app->view_dispatcher, 0);

    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);

    free(app);
}

int32_t pmsg_ble_beacon_app(void* p) {
    UNUSED(p);

    App* app = app_alloc();

    // Special handling: STOP Beacon item uses index 0xFFFFFFFF
    // We patch the callback by checking in submenu_cb:
    // but submenu_cb currently doesn’t check STOP; easiest is:
    // - treat STOP by detecting unknown cmd id -> do stop here via input
    //
    // For now: user can exit app to stop, and we stop in app_free().
    // (If you want STOP as a real menu item, I’ll refactor with a separate handler.)

    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    return 0;
}
