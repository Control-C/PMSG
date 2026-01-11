#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_box.h>
#include <notification/notification_messages.h>

#include <furi_hal_serial.h>
#include <furi_hal_power.h>

#include "pmsg_protocol.h"
#include "uart_line_reader.h"

typedef struct {
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    TextBox* textbox;
    NotificationApp* notifications;

    FuriString* log;
    FuriString* rx_line;

    // Serial handle
    FuriHalSerialHandle* serial;
    FuriHalSerialId serial_id;

    bool serial_ok;
} App;

static void log_append(App* app, const char* prefix, const char* msg) {
    if(furi_string_size(app->log) > 7000) {
        furi_string_reset(app->log);
        furi_string_cat(app->log, "Log cleared (size limit)\n");
    }
    furi_string_cat_printf(app->log, "%s%s\n", prefix, msg);
    text_box_set_text(app->textbox, furi_string_get_cstr(app->log));
    text_box_set_focus(app->textbox, TextBoxFocusEnd);
}

static void uart_send_line(App* app, const char* line) {
    if(!app->serial_ok) return;

    furi_hal_serial_tx(app->serial, (const uint8_t*)line, strlen(line));
    const char nl = '\n';
    furi_hal_serial_tx(app->serial, (const uint8_t*)&nl, 1);
}

// RX callback (ISR-ish context)
static void serial_rx_cb(FuriHalSerialHandle* handle, FuriHalSerialRxEvent event, void* context) {
    UNUSED(event);
    App* app = context;

    uint8_t b = 0;
    while(furi_hal_serial_rx(handle, &b, 1) == 1) {
        if(uart_line_reader_push_byte(app->rx_line, b, PMSG_RX_LINE_MAX)) {
            // We can’t safely update GUI from here; push to main thread via custom event.
            // Quick/simple: stash line in rx_line and notify main loop by sending event.
            view_dispatcher_send_custom_event(app->view_dispatcher, 1);
            // Prepare for next line: main thread will copy/reset.
            break;
        }
    }
}

static uint32_t custom_event_cb(void* context, uint32_t event) {
    App* app = context;
    if(event == 1) {
        // Consume rx_line
        if(furi_string_size(app->rx_line) > 0) {
            log_append(app, "< ", furi_string_get_cstr(app->rx_line));
            furi_string_reset(app->rx_line);
        }
    }
    return 0;
}

static void submenu_cb(void* context, uint32_t index) {
    App* app = context;
    if(index >= PMSG_UART_COMMAND_COUNT) return;

    const char* cmd = PMSG_UART_COMMANDS[index];
    log_append(app, "> ", cmd);
    uart_send_line(app, cmd);

    notification_message(app->notifications, &sequence_blink_green_10);
    view_dispatcher_switch_to_view(app->view_dispatcher, 1);
}

static bool serial_start(App* app) {
    // USART on GPIO header (the common choice)
    app->serial_id = FuriHalSerialIdUsart;

    app->serial = furi_hal_serial_control_acquire(app->serial_id);
    if(!app->serial) return false;

    furi_hal_serial_init(app->serial, PMSG_UART_BAUDRATE);

    furi_hal_serial_async_rx_start(app->serial, serial_rx_cb, app, false);

    return true;
}

static void serial_stop(App* app) {
    if(!app->serial) return;

    furi_hal_serial_async_rx_stop(app->serial);
    furi_hal_serial_deinit(app->serial);

    furi_hal_serial_control_release(app->serial);
    app->serial = NULL;
}

int32_t pmsg_uart_remote_app(void* p) {
    UNUSED(p);

    App* app = malloc(sizeof(App));
    app->view_dispatcher = view_dispatcher_alloc();
    app->submenu = submenu_alloc();
    app->textbox = text_box_alloc();
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    app->log = furi_string_alloc();
    app->rx_line = furi_string_alloc();

    // UI
    submenu_set_header(app->submenu, "PMSG UART Remote");
    for(uint32_t i = 0; i < PMSG_UART_COMMAND_COUNT; i++) {
        submenu_add_item(app->submenu, PMSG_UART_COMMANDS[i], i, submenu_cb, app);
    }

    text_box_set_font(app->textbox, TextBoxFontText);
    text_box_set_focus(app->textbox, TextBoxFocusEnd);

    furi_string_cat(app->log, "PMSG UART Remote\n");
    furi_string_cat(app->log, "UART: 115200 8N1\n");
    furi_string_cat(app->log, "GPIO pins: TX/RX/GND\n\n");
    text_box_set_text(app->textbox, furi_string_get_cstr(app->log));

    view_dispatcher_add_view(app->view_dispatcher, 0, submenu_get_view(app->submenu));
    view_dispatcher_add_view(app->view_dispatcher, 1, text_box_get_view(app->textbox));
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, custom_event_cb);
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);

    // Attach GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    // Start serial
    app->serial_ok = serial_start(app);
    if(app->serial_ok) {
        log_append(app, "", "Serial OK. Select a command.");
        notification_message(app->notifications, &sequence_success);
    } else {
        log_append(app, "", "Serial ERROR: cannot acquire USART.");
        notification_message(app->notifications, &sequence_error);
    }

    // Run
    view_dispatcher_run(app->view_dispatcher);

    // Cleanup
    if(app->serial_ok) serial_stop(app);

    view_dispatcher_remove_view(app->view_dispatcher, 0);
    view_dispatcher_remove_view(app->view_dispatcher, 1);
    submenu_free(app->submenu);
    text_box_free(app->textbox);
    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);

    furi_string_free(app->log);
    furi_string_free(app->rx_line);
    free(app);

    return 0;
}
