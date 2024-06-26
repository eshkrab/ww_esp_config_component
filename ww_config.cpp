#include "ww_config.h"
#include "ww_netman.h"

#include "fs_utils.h"

#define TAG "CONFIG_PARSER"

Config::Config() {
    initDefaults();
}

void Config::initDefaults() {
    strcpy(server_ip, "0.0.0.0");
    strcpy(bin_fname, "default.bin");
    ota_port = 0;
    dev_branch = false;
    serial_ctrl = false;
    osc_ctrl = false;
    osc_port = 0;
    cmd_port = 0;
    uart_baud = 115200;
    uart_tx_pin = 0;
    uart_rx_pin = 0;
    net_config.dhcp = 0;
    strcpy(net_config.ip, "0.0.0.0");
    strcpy(net_config.subnet, "255.255.255.0");
    strcpy(net_config.gw, "0.0.0.0");
    strcpy(net_config.SSID, "");
    strcpy(net_config.pswd, "");
    strcpy(net_config.AP_SSID, "espAP");
    strcpy(net_config.AP_pswd, "0000");
    net_config.mode = MODE_NONE;
}

bool Config::loadConfigFile(const char* dir, const char* fn) {
    char fname[64];
    sprintf(fname, "/%s/%s", dir, fn);

    char* buf;
    buf = (char*)calloc(2048, 1);

    bool loaded = false;

    if (buf == NULL) {
        ESP_LOGE("loadConfigFile", "failed to allocate buffer");
        return loaded;
    }

    int bytes = readFile(buf, fname, 2048);
    if (bytes > 0) {
        loadConfig(buf);
        loaded = true;
    }

    free(buf);
    return loaded;
}

bool Config::loadConfig(char* buf) {
    cJSON* val = cJSON_CreateNull();
    cJSON* root = cJSON_Parse(buf);

    if (root == NULL) {
        ESP_LOGE("WW_CONFIG","ERROR opening config json");
        return -1;
    }
    
    // ROOT VARS
    val = cJSON_GetObjectItem(root, "id");
    if (cJSON_IsNumber(val)) id = val->valueint;

    val = cJSON_GetObjectItem(root, "version");
    if (cJSON_IsNumber(val)) version = val->valuedouble;

    ////////////////////////////////////////////////
    // NETWORK SETTINGS
    ////////////////////////////////////////////////
    cJSON* network = cJSON_GetObjectItem(root, "network");
    if (cJSON_IsObject(network)) {
        val = cJSON_GetObjectItem(network, "connection");
        if (cJSON_IsString(val)) {
            char connection[16];
            strcpy(connection, val->valuestring);
            ESP_LOGI(TAG, "Connection: %s", connection);
            if ((strstr(connection, "none") != NULL))
                net_config.mode = MODE_NONE;
            else {
                net_config.mode = (strstr(connection, "eth") != NULL) ? MODE_ETH : MODE_WIFI;
                net_config.mode = (strstr(connection, "ap") != NULL) ? MODE_STA_AP : MODE_WIFI;
            }
            ESP_LOGI(TAG, "Net Mode: %d", net_config.mode);
            ESP_LOGI(TAG, " is it mode_wifi? %d", net_config.mode == MODE_WIFI);
        }
        val = cJSON_GetObjectItem(network, "DHCP");
        if (cJSON_IsNumber(val)) net_config.dhcp= val->valueint;

        val = cJSON_GetObjectItem(network, "IP");
        if (cJSON_IsString(val)) strcpy(net_config.ip, val->valuestring);

        val = cJSON_GetObjectItem(network, "subnet");
        if (cJSON_IsString(val)) strcpy(net_config.subnet, val->valuestring);

        val = cJSON_GetObjectItem(network, "gw");
        if (cJSON_IsString(val)) strcpy(net_config.gw, val->valuestring);

        val = cJSON_GetObjectItem(network, "SSID");
        if (cJSON_IsString(val)) strcpy(net_config.SSID, val->valuestring);

        val = cJSON_GetObjectItem(network, "pswd");
        if (cJSON_IsString(val)) strcpy(net_config.pswd, val->valuestring);

        val = cJSON_GetObjectItem(network, "AP_SSID");
        if (cJSON_IsString(val)) strcpy(net_config.AP_SSID, val->valuestring);

        val = cJSON_GetObjectItem(network, "AP_pswd");
        if (cJSON_IsString(val)) strcpy(net_config.AP_pswd, val->valuestring);
    }

    ////////////////////////////////////////////////
    // CONTROL INFORMATION
    ////////////////////////////////////////////////
    cJSON* ctrl = cJSON_GetObjectItem(root, "control");
    if (cJSON_IsObject(ctrl)) {
        val = cJSON_GetObjectItem(ctrl, "serial");
        if (cJSON_IsNumber(val)) serial_ctrl = val->valueint;

        val = cJSON_GetObjectItem(ctrl, "serial_tx");
        if (cJSON_IsNumber(val)) uart_tx_pin = val->valueint;

        val = cJSON_GetObjectItem(ctrl, "serial_rx");
        if (cJSON_IsNumber(val)) uart_rx_pin = val->valueint;

        val = cJSON_GetObjectItem(ctrl, "serial_baud");
        if (cJSON_IsNumber(val)) uart_baud = val->valueint;

        val = cJSON_GetObjectItem(ctrl, "osc");
        if (cJSON_IsNumber(val)) osc_ctrl = val->valueint;

        val = cJSON_GetObjectItem(ctrl, "osc_port");
        if (cJSON_IsNumber(val)) osc_port = val->valueint;

        val = cJSON_GetObjectItem(ctrl, "cmd_port");
        if (cJSON_IsNumber(val)) cmd_port = val->valueint;
    }


    ////////////////////////////////////////////////
    //LED INFORMATION
    ////////////////////////////////////////////////
    cJSON *leds = cJSON_GetObjectItem(root,"leds");
    if (cJSON_IsObject(leds)) {
      val = cJSON_GetObjectItem(leds,"num_pixels");
      if (cJSON_IsNumber(val)) leds_config.num_pixels = val->valueint;

      val = cJSON_GetObjectItem(leds,"num_strips");
      if (cJSON_IsNumber(val)) leds_config.num_strips = val->valueint;

      val = cJSON_GetObjectItem(leds,"led_type");
      if (cJSON_IsString(val)){
        char _led_type[10];
        strcpy(_led_type, val->valuestring);
        leds_config.led_type = toLedType(_led_type);
      }

      cJSON * pin;
      cJSON * pins = cJSON_GetObjectItem(leds, "data_pins");
      if (cJSON_IsArray(pins)) {
        if (leds_config.pins == NULL) {
          leds_config.pins = new uint8_t[leds_config.num_strips];
        }else{
          delete[] leds_config.pins;
          leds_config.pins = new uint8_t[leds_config.num_strips];
        }
        int c = 0;
        cJSON_ArrayForEach(pin, pins) {
          leds_config.pins[c++] = pin->valueint;
          ESP_LOGD("JSON UTILS","PIN %d", pin->valueint);
        }

      }
    }

    ////////////////////////////////////////////////
    // OTA INFORMATION
    ////////////////////////////////////////////////
    cJSON* ota = cJSON_GetObjectItem(root, "server");
    if (cJSON_IsObject(ota)) {
        val = cJSON_GetObjectItem(ota, "server_ip");
        if (cJSON_IsString(val)) strcpy(server_ip, val->valuestring);

        val = cJSON_GetObjectItem(ota, "filename");
        if (cJSON_IsString(val)) strcpy(bin_fname, val->valuestring);

        val = cJSON_GetObjectItem(ota, "branch");
        if (cJSON_IsString(val)) {
            if (strcmp(val->valuestring, "dev") == 0) dev_branch = true;
        }

        val = cJSON_GetObjectItem(ota, "ota_port");
        if (cJSON_IsNumber(val)) ota_port = val->valueint;
    }

    cJSON_Delete(root);
    return 1;
}


bool Config::saveConfigFile(const char* dir, const char* fn) {
    char fname[64];
    sprintf(fname, "/%s/%s", dir, fn);

    cJSON* root = cJSON_CreateObject();
    
    cJSON_AddNumberToObject(root, "id", id);
    cJSON_AddNumberToObject(root, "version", version);

    cJSON* network = cJSON_CreateObject();
    cJSON_AddStringToObject(network, "connection", net_config.mode == MODE_WIFI ? "wifi" : (net_config.mode == MODE_ETH ? "eth" : "none"));
    cJSON_AddNumberToObject(network, "DHCP", net_config.dhcp);
    cJSON_AddStringToObject(network, "IP", net_config.ip);
    cJSON_AddStringToObject(network, "subnet", net_config.subnet);
    cJSON_AddStringToObject(network, "gw", net_config.gw);
    cJSON_AddStringToObject(network, "SSID", net_config.SSID);
    cJSON_AddStringToObject(network, "pswd", net_config.pswd);
    cJSON_AddStringToObject(network, "AP_SSID", net_config.AP_SSID);
    cJSON_AddStringToObject(network, "AP_pswd", net_config.AP_pswd);
    cJSON_AddItemToObject(root, "network", network);

    cJSON* server = cJSON_CreateObject();
    cJSON_AddStringToObject(server, "server_ip", server_ip);
    cJSON_AddStringToObject(server, "filename", bin_fname);
    cJSON_AddNumberToObject(server, "ota_port", ota_port);
    cJSON_AddNumberToObject(server, "cmd_port", cmd_port);
    cJSON_AddStringToObject(server, "branch", dev_branch ? "dev" : "prod");
    cJSON_AddItemToObject(root, "server", server);

    char* buf = cJSON_Print(root);

    // Print the JSON structure before saving
    ESP_LOGI(TAG, "Config JSON before saving: %s", buf);

    bool saved = writeFile(buf, fname, strlen(buf), "w");

    free(buf);
    cJSON_Delete(root);

    // Print the saved file content
    char* savedBuf = (char*)calloc(2048, 1);
    int bytes = readFile(savedBuf, fname, 2048);
    if (bytes > 0) {
        ESP_LOGI(TAG, "Config JSON after saving: %s", savedBuf);
    }
    free(savedBuf);

    return saved;
}

void Config::printConfig() {
    ESP_LOGI(TAG, "ID: %d", id);
    ESP_LOGI(TAG, "Version: %f", version);

    ESP_LOGI(TAG, "Server IP: %s", server_ip);
    ESP_LOGI(TAG, "Bin Filename: %s", bin_fname);
    ESP_LOGI(TAG, "OTA Port: %d", ota_port);
    ESP_LOGI(TAG, "Dev Branch: %d", dev_branch);

    ESP_LOGI(TAG, "Serial Control: %d", serial_ctrl);
    ESP_LOGI(TAG, "OSC Control: %d", osc_ctrl);
    ESP_LOGI(TAG, "OSC Port: %d", osc_port);
    ESP_LOGI(TAG, "CMD Port: %d", cmd_port);
    ESP_LOGI(TAG, "UART Baud: %d", uart_baud);
    ESP_LOGI(TAG, "UART TX Pin: %d", uart_tx_pin);
    ESP_LOGI(TAG, "UART RX Pin: %d", uart_rx_pin);

    ESP_LOGI(TAG, "DHCP Mode: %d", net_config.dhcp);
    ESP_LOGI(TAG, "IP: %s", net_config.ip);
    ESP_LOGI(TAG, "Subnet: %s", net_config.subnet);
    ESP_LOGI(TAG, "GW: %s", net_config.gw);
    ESP_LOGI(TAG, "SSID: %s", net_config.SSID);
    ESP_LOGI(TAG, "PSWD: %s", net_config.pswd);
    ESP_LOGI(TAG, "AP_SSID: %s", net_config.AP_SSID);
    ESP_LOGI(TAG, "AP_PSWD: %s", net_config.AP_pswd);
    ESP_LOGI(TAG, "Net Mode: %d", net_config.mode);
}

