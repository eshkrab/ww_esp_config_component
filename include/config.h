#pragma once

#ifndef CONFIG_H
#define CONFIG_H

// #include "main.h"
// enum net_mode_t {
//     MODE_NONE,
//     MODE_ETH,
//     MODE_WIFI,
//     MODE_AP
// };

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

  int dhcp_mode;
  char ip[16];
  char subnet[16];
  char gw[16];
  char SSID[32];
  char pswd[32];
  char AP_SSID[32];
  char AP_pswd[32];
  int net_mode;

  Config();
  bool loadConfigFile(const char* dir, const char* fn);
  void loadConfig(char* buf);
  bool saveConfigFile(const char* dir, const char* fn);
  void saveConfig(char* buf);
  void printConfig();

private:
  void initDefaults();
};

#endif // CONFIG_H

