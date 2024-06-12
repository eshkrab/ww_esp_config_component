#include "ww_config.h"
#include "ww_netman.h"
#include "fs_utils.h"

#define TAG "CONFIG_PARSER"

Config::Config() {
    initDefaults();
}

void Config::initDefaults() {
    id = 0;

    strcpy(server_filename, "ww-esp");
    version = 0.1;
    strcpy(ota_server_ip, "0.0.0.0");
    ota_port = 8070;
    cmd_server_port = 8080;
    dev_branch = false;

    serial_ctrl = false;
    osc_ctrl = false;
    osc_port = 8000;
    cmd_port = 0;
    uart_tx_pin = 21;
    uart_rx_pin = 25;
    uart_baud = 115200;

    net_config.dhcp = 1;
    strcpy(net_config.ip, "10.25.0.25");
    strcpy(net_config.subnet, "255.255.255.0");
    strcpy(net_config.gw, "10.25.0.1");
    strcpy(net_config.SSID, "ww.dev");
    strcpy(net_config.pswd, "glitterpixels");
    strcpy(net_config.AP_SSID, "WW_ESP");
    strcpy(net_config.AP_pswd, "waitingis");
    net_config.mode = MODE_WIFI;

    leds_config.num_pixels = 300;
    leds_config.num_strips = 4;
    leds_config.brightness = 70;
    strcpy(leds_config.led_type, "WS2815");
    leds_config.pins = new uint8_t[leds_config.num_strips] { 9, 10, 5, 18 };

    player_config.framerate = 60;
    player_config.autoplay = 1;
    player_config.autoplay_speed = 1000;
    player_config.autostart = 1;
    strcpy(player_config.play_mode, "sdcard");
    player_config.shuffle = 1;

    strcpy(streaming_config.protocol, "sacn");
    streaming_config.multicast = 0;
    streaming_config.start_chan = 2;
    streaming_config.start_uni = 1;
}

bool Config::loadConfigFile(const char* dir, const char* fn) {
    char fname[64];
    sprintf(fname, "/%s/%s", dir, fn);

    char* buf;
    buf = (char*)calloc(2048, 1);

    bool loaded = false;

    if (buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate buffer");
        return loaded;
    }

    int bytes = readFile(buf, fname, 2048);
    if (bytes > 0) {
        loaded = loadConfig(buf);
    }

    free(buf);
    return loaded;
}

bool Config::loadConfig(char* buf) {
    cJSON* root = cJSON_Parse(buf);
    if (root == NULL) {
        ESP_LOGE(TAG, "ERROR opening config JSON");
        return false;
    }

    cJSON* val = cJSON_GetObjectItem(root, "id");
    if (cJSON_IsNumber(val)) id = val->valueint;

    ////////////////////////////////////////////////
    // NETWORK SETTINGS
    ////////////////////////////////////////////////
    cJSON* network = cJSON_GetObjectItem(root, "network");
    if (cJSON_IsObject(network)) {
        val = cJSON_GetObjectItem(network, "connection");
        if (cJSON_IsString(val))
            net_config.mode = (strcmp(val->valuestring, "wifi") == 0) ? MODE_WIFI : (strcmp(val->valuestring, "eth") == 0) ? MODE_ETH : MODE_NONE;

        val = cJSON_GetObjectItem(network, "DHCP");
        if (cJSON_IsNumber(val)) net_config.dhcp = val->valueint;

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
    // LED SETTINGS
    ////////////////////////////////////////////////
    cJSON* leds = cJSON_GetObjectItem(root, "leds");
    if (cJSON_IsObject(leds)) {
        val = cJSON_GetObjectItem(leds, "num_pixels");
        if (cJSON_IsNumber(val)) leds_config.num_pixels = val->valueint;

        val = cJSON_GetObjectItem(leds, "num_strips");
        if (cJSON_IsNumber(val)) leds_config.num_strips = val->valueint;

        val = cJSON_GetObjectItem(leds, "brightness");
        if (cJSON_IsNumber(val)) leds_config.brightness = val->valueint;

        val = cJSON_GetObjectItem(leds, "led_type");
        if (cJSON_IsString(val)) strcpy(leds_config.led_type, val->valuestring);

        cJSON* pins = cJSON_GetObjectItem(leds, "data_pins");
        if (cJSON_IsArray(pins)) {
            delete[] leds_config.pins;
            leds_config.pins = new uint8_t[leds_config.num_strips];
            int i = 0;
            cJSON* pin;
            cJSON_ArrayForEach(pin, pins) {
                leds_config.pins[i++] = pin->valueint;
            }
        }
    }

    ////////////////////////////////////////////////
    // PLAYER SETTINGS
    ////////////////////////////////////////////////
    cJSON* player = cJSON_GetObjectItem(root, "player");
    if (cJSON_IsObject(player)) {
        val = cJSON_GetObjectItem(player, "framerate");
        if (cJSON_IsNumber(val)) player_config.framerate = val->valueint;

        val = cJSON_GetObjectItem(player, "autoplay");
        if (cJSON_IsNumber(val)) player_config.autoplay = val->valueint;

        val = cJSON_GetObjectItem(player, "autoplay_speed");
        if (cJSON_IsNumber(val)) player_config.autoplay_speed = val->valueint;

        val = cJSON_GetObjectItem(player, "autostart");
        if (cJSON_IsNumber(val)) player_config.autostart = val->valueint;

        val = cJSON_GetObjectItem(player, "play_mode");
        if (cJSON_IsString(val)) strcpy(player_config.play_mode, val->valuestring);

        val = cJSON_GetObjectItem(player, "shuffle");
        if (cJSON_IsNumber(val)) player_config.shuffle = val->valueint;
    }

    ////////////////////////////////////////////////
    // STREAMING SETTINGS
    ////////////////////////////////////////////////
    cJSON* streaming = cJSON_GetObjectItem(root, "streaming");
    if (cJSON_IsObject(streaming)) {
        val = cJSON_GetObjectItem(streaming, "protocol");
        if (cJSON_IsString(val)) strcpy(streaming_config.protocol, val->valuestring);

        val = cJSON_GetObjectItem(streaming, "multicast");
        if (cJSON_IsNumber(val)) streaming_config.multicast = val->valueint;

        val = cJSON_GetObjectItem(streaming, "start_chan");
        if (cJSON_IsNumber(val)) streaming_config.start_chan = val->valueint;

        val = cJSON_GetObjectItem(streaming, "start_uni");
        if (cJSON_IsNumber(val)) streaming_config.start_uni = val->valueint;
    }

    ////////////////////////////////////////////////
    // CONTROL SETTINGS
    ////////////////////////////////////////////////
    cJSON* control = cJSON_GetObjectItem(root, "control");
    if (cJSON_IsObject(control)) {
        val = cJSON_GetObjectItem(control, "serial");
        if (cJSON_IsNumber(val)) serial_ctrl = val->valueint;

        val = cJSON_GetObjectItem(control, "serial_tx");
        if (cJSON_IsNumber(val)) uart_tx_pin = val->valueint;

        val = cJSON_GetObjectItem(control, "serial_rx");
        if (cJSON_IsNumber(val)) uart_rx_pin = val->valueint;

        val = cJSON_GetObjectItem(control, "serial_baud");
        if (cJSON_IsNumber(val)) uart_baud = val->valueint;

        val = cJSON_GetObjectItem(control, "osc");
        if (cJSON_IsNumber(val)) osc_ctrl = val->valueint;

        val = cJSON_GetObjectItem(control, "osc_port");
        if (cJSON_IsNumber(val)) osc_port = val->valueint;
    }

    ////////////////////////////////////////////////
    // SERVER SETTINGS
    ////////////////////////////////////////////////
    cJSON* server = cJSON_GetObjectItem(root, "server");
    if (cJSON_IsObject(server)) {
        val = cJSON_GetObjectItem(server, "filename");
        if (cJSON_IsString(val)) strcpy(server_filename, val->valuestring);

        val = cJSON_GetObjectItem(server, "version");
        if (cJSON_IsNumber(val)) version = val->valuedouble;

        val = cJSON_GetObjectItem(server, "branch");
        if (cJSON_IsString(val)) {
            dev_branch = strcmp(val->valuestring, "dev") == 0;
        }

        val = cJSON_GetObjectItem(server, "ota_server_ip");
        if (cJSON_IsString(val)) strcpy(ota_server_ip, val->valuestring);

        val = cJSON_GetObjectItem(server, "ota_port");
        if (cJSON_IsNumber(val)) ota_port = val->valueint;

        val = cJSON_GetObjectItem(server, "cmd_server_port");
        if (cJSON_IsNumber(val)) cmd_server_port = val->valueint;
    }

    cJSON_Delete(root);
    return true;
}

bool Config::saveConfigFile(const char* dir, const char* fn) {
    char fname[64];
    sprintf(fname, "/%s/%s", dir, fn);

    cJSON* root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "id", id);

    ////////////////////////////////////////////////
    // NETWORK SETTINGS
    ////////////////////////////////////////////////
    cJSON* network = cJSON_CreateObject();
    cJSON_AddStringToObject(network, "connection", net_config.mode == MODE_WIFI ? "wifi" : net_config.mode == MODE_ETH ? "eth" : "none");
    cJSON_AddNumberToObject(network, "DHCP", net_config.dhcp);
    cJSON_AddStringToObject(network, "IP", net_config.ip);
    cJSON_AddStringToObject(network, "subnet", net_config.subnet);
    cJSON_AddStringToObject(network, "gw", net_config.gw);
    cJSON_AddStringToObject(network, "SSID", net_config.SSID);
    cJSON_AddStringToObject(network, "pswd", net_config.pswd);
    cJSON_AddStringToObject(network, "AP_SSID", net_config.AP_SSID);
    cJSON_AddStringToObject(network, "AP_pswd", net_config.AP_pswd);
    cJSON_AddItemToObject(root, "network", network);

    ////////////////////////////////////////////////
    // LED SETTINGS
    ////////////////////////////////////////////////
    cJSON* leds = cJSON_CreateObject();
    cJSON_AddNumberToObject(leds, "num_pixels", leds_config.num_pixels);
    cJSON_AddNumberToObject(leds, "num_strips", leds_config.num_strips);
    cJSON_AddNumberToObject(leds, "brightness", leds_config.brightness);
    cJSON_AddStringToObject(leds, "led_type", leds_config.led_type);

    cJSON* pins = cJSON_CreateArray();
    for (int i = 0; i < leds_config.num_strips; i++) {
        cJSON_AddItemToArray(pins, cJSON_CreateNumber(leds_config.pins[i]));
    }
    cJSON_AddItemToObject(leds, "data_pins", pins);
    cJSON_AddItemToObject(root, "leds", leds);

    ////////////////////////////////////////////////
    // PLAYER SETTINGS
    ////////////////////////////////////////////////
    cJSON* player = cJSON_CreateObject();
    cJSON_AddNumberToObject(player, "framerate", player_config.framerate);
    cJSON_AddNumberToObject(player, "autoplay", player_config.autoplay);
    cJSON_AddNumberToObject(player, "autoplay_speed", player_config.autoplay_speed);
    cJSON_AddNumberToObject(player, "autostart", player_config.autostart);
    cJSON_AddStringToObject(player, "play_mode", player_config.play_mode);
    cJSON_AddNumberToObject(player, "shuffle", player_config.shuffle);
    cJSON_AddItemToObject(root, "player", player);

    ////////////////////////////////////////////////
    // STREAMING SETTINGS
    ////////////////////////////////////////////////
    cJSON* streaming = cJSON_CreateObject();
    cJSON_AddStringToObject(streaming, "protocol", streaming_config.protocol);
    cJSON_AddNumberToObject(streaming, "multicast", streaming_config.multicast);
    cJSON_AddNumberToObject(streaming, "start_chan", streaming_config.start_chan);
    cJSON_AddNumberToObject(streaming, "start_uni", streaming_config.start_uni);
    cJSON_AddItemToObject(root, "streaming", streaming);

    ////////////////////////////////////////////////
    // CONTROL SETTINGS
    ////////////////////////////////////////////////
    cJSON* ctrl = cJSON_CreateObject();
    cJSON_AddNumberToObject(ctrl, "serial", serial_ctrl);
    cJSON_AddNumberToObject(ctrl, "serial_tx", uart_tx_pin);
    cJSON_AddNumberToObject(ctrl, "serial_rx", uart_rx_pin);
    cJSON_AddNumberToObject(ctrl, "serial_baud", uart_baud);
    cJSON_AddNumberToObject(ctrl, "osc", osc_ctrl);
    cJSON_AddNumberToObject(ctrl, "osc_port", osc_port);
    cJSON_AddItemToObject(root, "control", ctrl);

    ////////////////////////////////////////////////
    // SERVER SETTINGS
    ////////////////////////////////////////////////
    cJSON* server = cJSON_CreateObject();
    cJSON_AddStringToObject(server, "filename", server_filename);
    cJSON_AddNumberToObject(server, "version", version);
    cJSON_AddStringToObject(server, "branch", dev_branch ? "dev" : "prod");
    cJSON_AddStringToObject(server, "ota_server_ip", ota_server_ip);
    cJSON_AddNumberToObject(server, "ota_port", ota_port);
    cJSON_AddNumberToObject(server, "cmd_server_port", cmd_server_port);
    cJSON_AddItemToObject(root, "server", server);

    char* buf = cJSON_Print(root);
    ESP_LOGI(TAG, "Config JSON before saving: %s", buf);

    bool saved = writeFile(buf, fname, strlen(buf), "w");

    free(buf);
    cJSON_Delete(root);

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

    ESP_LOGI(TAG, "Server Filename: %s", server_filename);
    ESP_LOGI(TAG, "Version: %f", version);
    ESP_LOGI(TAG, "OTA Server IP: %s", ota_server_ip);
    ESP_LOGI(TAG, "OTA Port: %d", ota_port);
    ESP_LOGI(TAG, "CMD Server Port: %d", cmd_server_port);
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

    ESP_LOGI(TAG, "Number of Pixels: %d", leds_config.num_pixels);
    ESP_LOGI(TAG, "Number of Strips: %d", leds_config.num_strips);
    ESP_LOGI(TAG, "Brightness: %d", leds_config.brightness);
    ESP_LOGI(TAG, "LED Type: %s", leds_config.led_type);

    ESP_LOGI(TAG, "Player Framerate: %d", player_config.framerate);
    ESP_LOGI(TAG, "Player Autoplay: %d", player_config.autoplay);
    ESP_LOGI(TAG, "Player Autoplay Speed: %d", player_config.autoplay_speed);
    ESP_LOGI(TAG, "Player Autostart: %d", player_config.autostart);
    ESP_LOGI(TAG, "Player Play Mode: %s", player_config.play_mode);
    ESP_LOGI(TAG, "Player Shuffle: %d", player_config.shuffle);

    ESP_LOGI(TAG, "Streaming Protocol: %s", streaming_config.protocol);
    ESP_LOGI(TAG, "Streaming Multicast: %d", streaming_config.multicast);
    ESP_LOGI(TAG, "Streaming Start Channel: %d", streaming_config.start_chan);
    ESP_LOGI(TAG, "Streaming Start Universe: %d", streaming_config.start_uni);
}
