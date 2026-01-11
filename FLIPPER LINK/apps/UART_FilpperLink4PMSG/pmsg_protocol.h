#pragma once

// UART config
#define PMSG_UART_BAUDRATE 115200
#define PMSG_RX_LINE_MAX 220

// Same commands as BLE app, but over UART we send full line + '\n' 
// TODO Display will be added later to send and get Sting test 126 signs 
static const char* PMSG_UART_COMMANDS[] = {
    "PMSG:BUZZ",
    "PMSG:vibration-tune1",
    "PMSG:vibration-tune2",
    "PMSG:vibration-tune3",
    "PMSG:vibration-sonar",
    "PMSG:LED:ON",
    "PMSG:RAINBOW",
    "PMSG:leftanimation",
    "PMSG:rightanimation",
    "PMSG:allblue",
    "PMSG:allblue-flash",
    "PMSG:allgreen-flash",
    "PMSG:allgreen",

    "PMSG:LUX",
    "PMSG:UV",
    "PMSG:movement",
    "PMSG:temp",
    "PMSG:humidity",
    "PMSG:tap",
    "PMSG:touch",
};

#define PMSG_UART_COMMAND_COUNT (sizeof(PMSG_UART_COMMANDS) / sizeof(PMSG_UART_COMMANDS[0]))
