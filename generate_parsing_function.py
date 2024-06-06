def generate_parsing_function(config, struct_name="config_t"):
    function_body = []
    for key, value in config.items():
        if isinstance(value, dict):
            sub_struct_name = f"{key}_t"
            function_body.append(f"parse_{key}(json_object, &config->{key});")
        elif isinstance(value, list):
            function_body.append(f"parse_array(json_object, \"{key}\", config->{key}, {len(value)});")
        else:
            if isinstance(value, int):
                function_body.append(f"config->{key} = cJSON_GetObjectItem(json_object, \"{key}\")->valueint;")
            elif isinstance(value, float):
                function_body.append(f"config->{key} = cJSON_GetObjectItem(json_object, \"{key}\")->valuedouble;")
            else:
                function_body.append(f"config->{key} = cJSON_GetObjectItem(json_object, \"{key}\")->valuestring;")

    function_code = f"void parse_config(cJSON *json_object, {struct_name} *config) {{\n" + "\n".join(function_body) + "\n}}"
    return function_code

parsing_code = generate_parsing_function(config)
with open('ww_config.cpp', 'w') as f:
    f.write(parsing_code)

