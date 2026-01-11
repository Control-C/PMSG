#include "pmsg_protocol.h"
#include <string.h>

size_t pmsg_build_adv_payload(uint8_t* out, size_t out_max, const char* cmd) {
    if(!out || !cmd) return 0;
    if(out_max < 3) return 0;

    // Build ASCII: "PMSG:<cmd>"
    char msg[32];
    // Keep it bounded (ADV is tiny anyway) // todo PMSGv4 + UID 
    const char* prefix = "PMSG:"; 
    size_t prefix_len = strlen(prefix);
    size_t cmd_len = strlen(cmd);
    if(prefix_len + cmd_len >= sizeof(msg)) return 0;

    memcpy(msg, prefix, prefix_len);
    memcpy(msg + prefix_len, cmd, cmd_len);
    msg[prefix_len + cmd_len] = '\0';
    size_t msg_len = prefix_len + cmd_len;

    // ADV flags AD structure: len=2, type=0x01, data=0x06
    // Total bytes: 3
    uint8_t payload[31];
    size_t i = 0;

    payload[i++] = 0x02;
    payload[i++] = 0x01;
    payload[i++] = 0x06;

    // Manufacturer specific AD structure:
    // len = 1(type) + 2(company) + msg_len
    // Stored as: [len][0xFF][company_lsb][company_msb][...msg...]
    size_t mfg_len = 1 + 2 + msg_len;
    if(mfg_len > 0xFF) return 0;

    // Total structure bytes is (1 + len) because of the len byte itself
    size_t mfg_total = 1 + mfg_len;

    if(i + mfg_total > sizeof(payload)) return 0;

    payload[i++] = (uint8_t)mfg_len;
    payload[i++] = 0xFF;
    payload[i++] = PMSG_COMPANY_ID_LSB;
    payload[i++] = PMSG_COMPANY_ID_MSB;

    memcpy(&payload[i], msg, msg_len);
    i += msg_len;

    if(i > out_max) return 0;
    memcpy(out, payload, i);
    return i;
}
