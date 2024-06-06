import json

# Load the JSON config
with open('config.json') as f:
    config = json.load(f)

def generate_structs(config, struct_name="config_t"):
    structs = []
    struct_body = []
    for key, value in config.items():
        if isinstance(value, dict):
            sub_struct_name = f"{key}_t"
            structs.append(generate_structs(value, sub_struct_name))
            struct_body.append(f"{sub_struct_name} {key};")
        elif isinstance(value, list):
            struct_body.append(f"int {key}[{len(value)}];")
        else:
            if isinstance(value, int):
                c_type = "int"
            elif isinstance(value, float):
                c_type = "float"
            else:
                c_type = "char *"
            struct_body.append(f"{c_type} {key};")

    struct_definition = f"typedef struct {struct_name} {{\n" + "\n".join(struct_body) + "\n}} {struct_name};"
    structs.append(struct_definition)
    return "\n\n".join(structs)

structs_code = generate_structs(config)
with open('include/ww_config.h', 'w') as f:
    f.write(structs_code)

