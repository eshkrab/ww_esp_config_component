typedef struct leds_t {
int data_pins[4];
char * led_type;
int num_pixels;
int num_strips;
}} {struct_name};

typedef struct network_t {
int DHCP;
char * SSID;
char * pswd;
char * IP;
char * subnet;
char * gw;
}} {struct_name};

typedef struct streaming_t {
int multicast;
int start_chan;
int start_uni;
}} {struct_name};

typedef struct settings_t {
int autoplay;
int autoplay_speed;
int autostart;
int brightness;
char * play_mode;
int shuffle;
}} {struct_name};

typedef struct config_t {
char * connection;
int framerate;
leds_t leds;
network_t network;
streaming_t streaming;
settings_t settings;
}} {struct_name};