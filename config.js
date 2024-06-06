document.addEventListener('DOMContentLoaded', function() {
    const fieldLabels = {
        id: "ID",
        version: "Version",
        "MAC Address": "MAC Address",
        connection: "Connection Type",
        DHCP: "Use DHCP",
        SSID: "Wi-Fi Network Name (SSID)",
        pswd: "Wi-Fi Password",
        AP_SSID: "Access Point Mode Network Name",
        AP_pswd: "Access Point Mode Password",
        IP: "Static IP Address",
        subnet: "Subnet Mask",
        gw: "Gateway Address",
        server_ip: "Server IP Address",
        cmd_port: "Command Port",
        ota_port: "OTA Port",
        branch: "Branch",
        filename: "Firmware Filename"
    };

    // Load and populate config data
    fetch('/config.json')
        .then(response => response.json())
        .then(data => {
            const configForm = document.getElementById('configForm');

            // Create Info section
            createSection(configForm, 'Info', [
                { key: 'id', value: data.id, type: 'number' },
                { key: 'version', value: data.version, type: 'text' },
                { key: 'MAC Address', value: '', type: 'text', fetchUrl: '/mac_address' }
            ]);

            // Create Network section
            createSection(configForm, 'Network', [
                { key: 'connection', value: data.network.connection, type: 'select', options: ['none', 'wifi', 'eth', 'sta_ap'] },
                { key: 'DHCP', value: data.network.DHCP, type: 'checkbox' },
                { key: 'SSID', value: data.network.SSID, type: 'text' },
                { key: 'pswd', value: data.network.pswd, type: 'password' },
                { key: 'AP_SSID', value: data.network.AP_SSID, type: 'text' },
                { key: 'AP_pswd', value: data.network.AP_pswd, type: 'password' },
                { key: 'IP', value: data.network.IP, type: 'text', pattern: '^([0-9]{1,3}\\.){3}[0-9]{1,3}$', title: 'Please enter a valid IP address' },
                { key: 'subnet', value: data.network.subnet, type: 'text', pattern: '^([0-9]{1,3}\\.){3}[0-9]{1,3}$', title: 'Please enter a valid subnet mask' },
                { key: 'gw', value: data.network.gw, type: 'text', pattern: '^([0-9]{1,3}\\.){3}[0-9]{1,3}$', title: 'Please enter a valid gateway address' }
            ]);

            // Create Server section
            createSection(configForm, 'Server', [
                { key: 'server_ip', value: data.server.server_ip, type: 'text', pattern: '^([0-9]{1,3}\\.){3}[0-9]{1,3}$', title: 'Please enter a valid IP address' },
                { key: 'cmd_port', value: data.server.cmd_port, type: 'number', min: '1', max: '65535', title: 'Please enter a valid port number' },
                { key: 'ota_port', value: data.server.ota_port, type: 'number', min: '1', max: '65535', title: 'Please enter a valid port number' },
                { key: 'branch', value: data.server.branch, type: 'text' },
                { key: 'filename', value: data.server.filename, type: 'text' }
            ]);

            toggleConfigEditable(false);
        })
        .catch(error => console.error('Error fetching config:', error));

    // Edit button functionality
    document.getElementById('editConfigBtn').addEventListener('click', function() {
        toggleConfigEditable(true);
    });

    // Save and reboot button
    document.getElementById('saveConfigBtn').addEventListener('click', function() {
        let configData = {};
        const configForm = document.getElementById('configForm');
        const inputs = configForm.querySelectorAll('input, select');

        inputs.forEach(input => {
            let [section, key] = input.id.split('_');
            if (!key) {
                key = section;
                section = null;
            }
            if (input.checkValidity()) {
                let value;
                if (input.type === 'checkbox') {
                    value = input.checked ? 1 : 0;
                } else if (input.type === 'number') {
                    value = parseFloat(input.value);
                } else {
                    value = input.value;
                }
                if (section) {
                    if (!configData[section]) {
                        configData[section] = {};
                    }
                    configData[section][key] = value;
                } else {
                    configData[key] = value;
                }
            } else {
                alert(`Invalid input for ${input.name}`);
            }
        });

        fetch('/save_config', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(configData)
        })
        .then(response => response.text())
        .then(data => {
            alert(data);
            // Reboot the device
            fetch('/restart')
                .then(response => response.text())
                .then(data => {
                    alert(data);
                })
                .catch(error => console.error('Error restarting:', error));
        })
        .catch(error => console.error('Error saving config:', error));
    });

    function createSection(form, sectionTitle, fields) {
        const sectionDiv = document.createElement('div');
        sectionDiv.className = 'section';
        const sectionHeader = document.createElement('h3');
        sectionHeader.textContent = sectionTitle;
        sectionDiv.appendChild(sectionHeader);

        const gridDiv = document.createElement('div');
        gridDiv.className = 'grid';

        fields.forEach(field => {
            if (field.fetchUrl) {
                fetch(field.fetchUrl)
                    .then(response => response.text())
                    .then(value => {
                        createConfigField(gridDiv, null, field.key, value, field.type, fieldLabels[field.key]);
                    })
                    .catch(error => console.error(`Error fetching ${field.key}:`, error));
            } else {
                createConfigField(gridDiv, null, field.key, field.value, field.type, fieldLabels[field.key], field.options, field.pattern, field.title, field.min, field.max);
            }
        });

        sectionDiv.appendChild(gridDiv);
        form.appendChild(sectionDiv);
    }

    function createConfigField(form, section, key, value, type, label, options, pattern, title, min, max) {
        const div = document.createElement('div');
        let labelElement = document.createElement('label');
        labelElement.textContent = label || key;
        div.appendChild(labelElement);

        if (type === 'select') {
            let select = document.createElement('select');
            select.id = section ? `${section}_${key}` : key;
            select.name = section ? `${section}_${key}` : key;

            options.forEach(option => {
                let opt = document.createElement('option');
                opt.value = option;
                opt.textContent = option;
                if (option === value) {
                    opt.selected = true;
                }
                select.appendChild(opt);
            });
            div.appendChild(select);
        } else if (type === 'checkbox') {
            let input = document.createElement('input');
            input.type = 'checkbox';
            input.id = section ? `${section}_${key}` : key;
            input.name = section ? `${section}_${key}` : key;
            input.checked = value === 1;
            div.appendChild(input);
        } else {
            let input = document.createElement('input');
            input.type = type;
            input.id = section ? `${section}_${key}` : key;
            input.name = section ? `${section}_${key}` : key;
            input.value = value;
            if (pattern) input.pattern = pattern;
            if (title) input.title = title;
            if (min) input.min = min;
            if (max) input.max = max;

            div.appendChild(input);
        }

        form.appendChild(div);
    }

    function toggleConfigEditable(editable) {
        const configForm = document.getElementById('configForm');
        const inputs = configForm.querySelectorAll('input, select');
        inputs.forEach(input => {
            input.disabled = !editable;
        });

        document.getElementById('editConfigBtn').style.display = editable ? 'none' : 'block';
        document.getElementById('saveConfigBtn').style.display = editable ? 'block' : 'none';
    }
});

