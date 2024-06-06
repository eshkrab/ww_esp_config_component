// #pragma once
//
// enum led_type_t { LED_WS2811, LED_WS2812, LED_WS2812B, LED_SK6812, LED_WS2813,
//                   LED_WS2815, LED_GS8208, LED_RT1809, LED_SK6812W, NUM_LED_TYPES};
//
//
// typedef union {
//   struct __attribute__ ((packed)) {
//     uint8_t r, g, b, w;
//   };
//   uint32_t num;
// } color_t;
//
// typedef struct{
//   led_type_t led_type;
//   uint8_t num_strips  = 0;
//   uint16_t num_pixels = 0;
//   uint8_t * pins      = NULL;
//   color_t ** data_ptr = NULL;
//
// } pixel_config_t;
//
//
// inline led_type_t toLedType(char str[10])
// {
//
//   led_type_t r = (led_type_t)0;
//
//   if ( strcmp( str, "WS2811") == 0){
//       r = LED_WS2811;
//
//   }else if ( strcmp( str,"WS2812") == 0){
//       r = LED_WS2812;
//
//   }else if ( strcmp( str,"WS2812B") == 0){
//       r = LED_WS2812B;
//
//   }else if ( strcmp( str,"WS2813") == 0){
//       r = LED_WS2813;
//
//   } else if ( strcmp( str, "WS2815") == 0){
//       r = LED_WS2815;
//
//   } else if ( strcmp (str, "SK6812") == 0){
//       r = LED_SK6812;
//
//   } else if ( strcmp (str, "SK6812W") == 0){
//       r = LED_SK6812W;
//
//   } else if ( strcmp (str, "GS8208") == 0){
//       r = LED_GS8208;
//
//   } else if ( strcmp (str, "RT1809") == 0){
//       r = LED_RT1809;
//   }
//   return r;
// }
//
