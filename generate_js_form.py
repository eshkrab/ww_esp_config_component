import json

# Load the JSON config
with open('config.json') as f:
    config = json.load(f)

# Function to generate JavaScript field labels from the JSON keys
def generate_field_labels(config, parent_key=''):
    field_labels = {}
    for key, value in config.items():
        full_key = f"{parent_key}.{key}" if parent_key else key
        label = key.replace('_', ' ').capitalize()
        field_labels[full_key] = label
        if isinstance(value, dict):
            field_labels.update(generate_field_labels(value, full_key))
    return field_labels

field_labels = generate_field_labels(config)

# Function to generate JavaScript form fields
def generate_form_fields(config, parent_key=''):
    fields = []
    for key, value in config.items():
        full_key = f"{parent_key}.{key}" if parent_key else key
        if isinstance(value, dict):
            fields.append(f"createSection(configForm, '{key.capitalize()}', {generate_form_fields(value, full_key)});")
        elif isinstance(value, list):
            fields.append(f"createArrayField(configForm, '{key}', {value});")
        else:
            input_type = "text"
            if isinstance(value, int):
                input_type = "number"
            elif isinstance(value, bool):
                input_type = "checkbox"
            fields.append(f"createConfigField(configForm, '{key}', {value}, '{input_type}', fieldLabels['{full_key}']);")
    return "[" + ", ".join(fields) + "]"

# Generate the JavaScript code
js_code = f"""
document.addEventListener('DOMContentLoaded', function() {{
    const fieldLabels = {json.dumps(field_labels)};

    // Load and populate config data
    fetch('/config.json')
        .then(response => response.json())
        .then(data => {{
            const configForm = document.getElementById('configForm');
            {generate_form_fields(config)}

            toggleConfigEditable(false);
        }})
        .catch(error => console.error('Error fetching config:', error));

    // Edit button functionality
    document.getElementById('editConfigBtn').addEventListener('click', function() {{
        toggleConfigEditable(true);
    }});

    // Save and reboot button
    document.getElementById('saveConfigBtn').addEventListener('click', function() {{
        let configData = {{}};
        const configForm = document.getElementById('configForm');
        const inputs = configForm.querySelectorAll('input, select');

        inputs.forEach(input => {{
            let [section, key] = input.id.split('_');
            if (!key) {{
                key = section;
                section = null;
            }}
            if (input.checkValidity()) {{
                let value;
                if (input.type === 'checkbox') {{
                    value = input.checked ? 1 : 0;
                }} else if (input.type === 'number') {{
                    value = parseFloat(input.value);
                }} else {{
                    value = input.value;
                }}
                if (section) {{
                    if (!configData[section]) {{
                        configData[section] = {{}};
                    }}
                    configData[section][key] = value;
                }} else {{
                    configData[key] = value;
                }}
            }} else {{
                alert(`Invalid input for ${input.id}`);
            }}
        }});

        fetch('/save_config', {{
            method: 'POST',
            headers: {{
                'Content-Type': 'application/json'
            }},
            body: JSON.stringify(configData)
        }})
        .then(response => response.text())
        .then(data => {{
            alert(data);
            // Reboot the device
            fetch('/restart')
                .then(response => response.text())
                .then(data => {{
                    alert(data);
                }})
                .catch(error => console.error('Error restarting:', error));
        }})
        .catch(error => console.error('Error saving config:', error));
    }});
}});
"""

with open('config.js', 'w') as f:
    f.write(js_code)

