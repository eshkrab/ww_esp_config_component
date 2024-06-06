#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#include "cJSON.h"
#include "fs_utils.h"
#include "ww_netman.h"
#include "ww_leds.h"

class Config {
public:
  int id;
  double version;

  char server_ip[16];
  char bin_fname[30];
  int ota_port;
  bool dev_branch;

  bool serial_ctrl;
  bool osc_ctrl;
  int osc_port;
  int cmd_port;
  int uart_baud;
  int uart_tx_pin;
  int uart_rx_pin;

  net_config_t net_config;
  pixel_config_t leds_config;

  Config();
  bool loadConfigFile(const char* dir, const char* fn);
  bool loadConfig(char* buf);
  bool saveConfigFile(const char* dir, const char* fn);
  void printConfig();

private:
  void initDefaults();
};

#endif // CONFIG_H

