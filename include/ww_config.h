#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include "cJSON.h"
#include "fs_utils.h"
#include "ww_netman.h"
#include "ww_leds.h"
#include "ww_player.h"  // Include player.h for player settings
#include "ww_sacn.h"  

// #include "streaming.h"  // Include streaming.h for streaming settings

class Config {
public:
    int id;
    double version;

    char server_filename[30];
    char ota_server_ip[16];
    int ota_port;
    int cmd_server_port;
    bool dev_branch;

    bool serial_ctrl;
    bool dmx_ctrl;
    bool osc_ctrl;
    int osc_port;
    int cmd_port;

    int uart_baud;
    int uart_tx_pin;
    int uart_rx_pin;
    int dmx_tx_pin;
    int dmx_rx_pin;
    int dmx_rst_pin;
    int dmx_addr; // max 512
    int num_dmx_channels;


    net_config_t net_config;
    pixel_config_t leds_config;
    player_config_t player_config;  // Player settings
    sacn_config_t streaming_config;  // Streaming settings

    Config();
    bool loadConfigFile(const char* dir, const char* fn);
    bool loadConfig(char* buf);
    bool saveConfigFile(const char* dir, const char* fn);
    void printConfig();

private:
    void initDefaults();
};

#endif // CONFIG_H

