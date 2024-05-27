#include "config.h"
#include "main.h"
// #include "cJSON.h"
#include "tcpip.h"

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
    dhcp_mode = 0;
    strcpy(ip, "0.0.0.0");
    strcpy(subnet, "255.255.255.0");
    strcpy(gw, "0.0.0.0");
    strcpy(SSID, "");
    strcpy(pswd, "");
    strcpy(AP_SSID, "espAP");
    strcpy(AP_pswd, "0000");
    net_mode = MODE_NONE;
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

void Config::loadConfig(char* buf) {
    cJSON* val = cJSON_CreateNull();
    cJSON* root = cJSON_Parse(buf);

    if (root == NULL) {
        printf("ERROR opening settings json\n");
        return;
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
                net_mode = MODE_NONE;
            else {
                net_mode = (strstr(connection, "eth") != NULL) ? MODE_ETH : MODE_WIFI;
                net_mode = (strstr(connection, "ap") != NULL) ? MODE_AP : MODE_WIFI;
            }
            ESP_LOGI(TAG, "Net Mode: %d", net_mode);
            ESP_LOGI(TAG, "mode_wifi: %d", MODE_WIFI);
        }
        val = cJSON_GetObjectItem(network, "DHCP");
        if (cJSON_IsNumber(val)) dhcp_mode = val->valueint;

        val = cJSON_GetObjectItem(network, "IP");
        if (cJSON_IsString(val)) strcpy(ip, val->valuestring);

        val = cJSON_GetObjectItem(network, "subnet");
        if (cJSON_IsString(val)) strcpy(subnet, val->valuestring);

        val = cJSON_GetObjectItem(network, "gw");
        if (cJSON_IsString(val)) strcpy(gw, val->valuestring);

        val = cJSON_GetObjectItem(network, "SSID");
        if (cJSON_IsString(val)) strcpy(SSID, val->valuestring);

        val = cJSON_GetObjectItem(network, "pswd");
        if (cJSON_IsString(val)) strcpy(pswd, val->valuestring);

        val = cJSON_GetObjectItem(network, "AP_SSID");
        if (cJSON_IsString(val)) strcpy(AP_SSID, val->valuestring);

        val = cJSON_GetObjectItem(network, "AP_pswd");
        if (cJSON_IsString(val)) strcpy(AP_pswd, val->valuestring);
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
}

bool Config::saveConfigFile(const char* dir, const char* fn) {
    char fname[64];
    sprintf(fname, "/%s/%s", dir, fn);

    cJSON* root = cJSON_CreateObject();

    ////////////////////////////////////////////////
    // NETWORK SETTINGS
    ////////////////////////////////////////////////
    cJSON* network = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "network", network);
    cJSON_AddNumberToObject(network, "DHCP", dhcp_mode);
    cJSON_AddStringToObject(network, "IP", ip);
    cJSON_AddStringToObject(network, "subnet", subnet);
    cJSON_AddStringToObject(network, "gw", gw);
    cJSON_AddStringToObject(network, "SSID", SSID);
    cJSON_AddStringToObject(network, "pswd", pswd);
    cJSON_AddStringToObject(network, "AP_SSID", AP_SSID);
    cJSON_AddStringToObject(network, "AP_pswd", AP_pswd);

    ////////////////////////////////////////////////
    // CONTROL INFORMATION
    ////////////////////////////////////////////////
    cJSON* ctrl = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "control", ctrl);
    cJSON_AddNumberToObject(ctrl, "serial", serial_ctrl);
    cJSON_AddNumberToObject(ctrl, "serial_tx", uart_tx_pin);
    cJSON_AddNumberToObject(ctrl, "serial_rx", uart_rx_pin);
    cJSON_AddNumberToObject(ctrl, "serial_baud", uart_baud);
    cJSON_AddNumberToObject(ctrl, "osc", osc_ctrl);
    cJSON_AddNumberToObject(ctrl, "osc_port", osc_port);
    cJSON_AddNumberToObject(ctrl, "cmd_port", cmd_port);

    ////////////////////////////////////////////////
    // OTA INFORMATION
    ////////////////////////////////////////////////
    cJSON* ota = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "server", ota);
    cJSON_AddStringToObject(ota, "server_ip", server_ip);
    cJSON_AddStringToObject(ota, "filename", bin_fname);
    cJSON_AddStringToObject(ota, "branch", dev_branch ? "dev" : "main");
    cJSON_AddNumberToObject(ota, "ota_port", ota_port);

    char* buf = cJSON_Print(root);
    bool saved = writeFile(fname, buf, strlen(buf), "w");

    free(buf);
    cJSON_Delete(root);

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

    ESP_LOGI(TAG, "DHCP Mode: %d", dhcp_mode);
    ESP_LOGI(TAG, "IP: %s", ip);
    ESP_LOGI(TAG, "Subnet: %s", subnet);
    ESP_LOGI(TAG, "GW: %s", gw);
    ESP_LOGI(TAG, "SSID: %s", SSID);
    ESP_LOGI(TAG, "PSWD: %s", pswd);
    ESP_LOGI(TAG, "AP_SSID: %s", AP_SSID);
    ESP_LOGI(TAG, "AP_PSWD: %s", AP_pswd);
    ESP_LOGI(TAG, "Net Mode: %d", net_mode);
}
