void parse_config(cJSON *json_object, config_t *config) {
config->connection = cJSON_GetObjectItem(json_object, "connection")->valuestring;
config->framerate = cJSON_GetObjectItem(json_object, "framerate")->valueint;
parse_leds(json_object, &config->leds);
parse_network(json_object, &config->network);
parse_streaming(json_object, &config->streaming);
parse_settings(json_object, &config->settings);
}}