# Configuration Automation Scripts

This repository contains scripts to automate the generation of configuration structures, parsing functions, and HTML form code based on a JSON configuration file.

## Prerequisites

- Python 3.x

## Files

- `generate_structs.py`: Generates C structure definitions from `config.json`.
- `generate_parsing_function.py`: Generates C parsing functions from `config.json`.
- `generate_js_form.py`: Generates JavaScript code for HTML form based on `config.json`.
- `config.json`: Example JSON configuration file.

## Usage

1. **Generate C Structs and Parsing Functions**

    Run the following commands to generate the C header and source files:

    ```sh
    python generate_structs.py
    python generate_parsing_function.py
    ```

    This will create/update the `ww_config.h` and `ww_config.cpp` files with the necessary C structures and parsing functions.

2. **Generate JavaScript Form Code**

    Run the following command to generate the JavaScript form code:

    ```sh
    python generate_js_form.py
    ```

    This will create/update the `config.js` file with the necessary JavaScript code to generate the HTML form dynamically.

3. **Integrate Generated Files**

    - **C Files**: Include the generated `ww_config.h` and `ww_config.cpp` in your ESP-IDF project.
    - **JavaScript File**: Include the generated `config.js` in your web server to serve the HTML form.

4. **Run Your Project**

    - Build and flash your ESP-IDF project.
    - Open the web interface to view and edit the configuration using the dynamically generated form.

## Example

Here's an example `config.json` file used for generating the structures, parsing functions, and form code:

```json
{
   "connection": "wifi",
   "framerate": 60,
   "leds": {
      "data_pins": [9, 10, 5, 18],
      "led_type": "WS2815",
      "num_pixels": 300,
      "num_strips": 4
   },
   "network": {
     "DHCP": 0,
     "SSID": "space_goats",
     "pswd": "spacespace",
     "IP": "192.168.10.226",
     "subnet": "255.255.255.0",
     "gw": "192.168.10.1"
   },
   "streaming": {
      "multicast": 0,
      "start_chan": 2,
      "start_uni": 1
   },
   "settings": {
      "autoplay": 1,
      "autoplay_speed": 1000,
      "autostart": 1,
      "brightness": 70,
      "play_mode": "sdcard",
      "shuffle": 1
   }
}

