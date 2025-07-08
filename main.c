#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hardware/gpio.h"
#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_descriptors.h"

#define ROWS 6   
#define COLS 17      
#define ENCODER_PIN_A 23
#define ENCODER_PIN_B 24
#define ENCODER2_PIN_A 26
#define ENCODER2_PIN_B 27

const uint8_t row_pins[ROWS] = {17, 18, 19, 20, 21, 22}; 
const uint8_t col_pins[COLS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};  

const uint8_t keymap[6][17] = {
{ 0, HID_KEY_ESCAPE, HID_KEY_F1, HID_KEY_F2, HID_KEY_F3, HID_KEY_F4, HID_KEY_F5, HID_KEY_F6, HID_KEY_F7,
  HID_KEY_F8, HID_KEY_F9, HID_KEY_F10, HID_KEY_F11, HID_KEY_F12, HID_KEY_PRINT_SCREEN, HID_KEY_SCROLL_LOCK, HID_KEY_PAUSE},

{ HID_KEY_GRAVE, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, HID_KEY_6, HID_KEY_7,
  HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_MINUS, HID_KEY_EQUAL, HID_KEY_BACKSPACE, HID_KEY_INSERT, HID_KEY_HOME, HID_KEY_PAGE_UP },

{ HID_KEY_TAB, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T, HID_KEY_Y, HID_KEY_U,
  HID_KEY_I, HID_KEY_O, HID_KEY_P, HID_KEY_BRACKET_LEFT, HID_KEY_BRACKET_RIGHT, HID_KEY_BACKSLASH, HID_KEY_DELETE, HID_KEY_END, HID_KEY_PAGE_DOWN },

{ HID_KEY_CAPS_LOCK, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_H, HID_KEY_J,
  HID_KEY_K, HID_KEY_L, HID_KEY_SEMICOLON, HID_KEY_APOSTROPHE, 0, HID_KEY_ENTER, 0, 0, 0 },

{ HID_KEY_SHIFT_LEFT, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B, HID_KEY_N,
  HID_KEY_M, HID_KEY_COMMA, HID_KEY_PERIOD, HID_KEY_SLASH, 0, 0, HID_KEY_SHIFT_RIGHT, 0, HID_KEY_ARROW_UP, 0 },

{ HID_KEY_CONTROL_LEFT, HID_KEY_GUI_LEFT, HID_KEY_ALT_LEFT, 0, 0, 0, HID_KEY_SPACE, 0, 0, 0, HID_KEY_ALT_RIGHT, HID_KEY_GUI_RIGHT, HID_KEY_APPLICATION,
  HID_KEY_CONTROL_RIGHT, HID_KEY_ARROW_LEFT, HID_KEY_ARROW_DOWN, HID_KEY_ARROW_RIGHT }
};

uint8_t key_state[ROWS][COLS] = {0};

void hid_task(void);
void setup_gpio(void);
void scan_matrix(void);
void handle_encoder(void);
void send_hid_report(void);

int main(void)
{
  board_init();

  // init device stack on configured roothub port
  tud_init(BOARD_TUD_RHPORT);
  setup_gpio();

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  while (true)
  {
    tud_task(); // tinyusb device task
    handle_encoder();
    scan_matrix();
    send_hid_report();
  }
}

void setup_gpio() {
  
  for (int i = 0; i < ROWS; i++) {
  gpio_init(row_pins[i]);          
  gpio_set_dir(row_pins[i], GPIO_IN);  // set as input
  gpio_pull_down(row_pins[i]);     // pull-up resistor    
  }

  
  for (int i = 0; i < COLS; i++) {
    gpio_init(col_pins[i]);          
    gpio_set_dir(col_pins[i], GPIO_OUT); // set as output
    gpio_put(col_pins[i], 0);        
  }

  
  gpio_init(ENCODER_PIN_A);
  gpio_set_dir(ENCODER_PIN_A, GPIO_IN);
  gpio_pull_up(ENCODER_PIN_A);

  gpio_init(ENCODER_PIN_B);
  gpio_set_dir(ENCODER_PIN_B, GPIO_IN);
  gpio_pull_up(ENCODER_PIN_B);

  gpio_init(25);
  gpio_set_dir(25, GPIO_IN);
  gpio_pull_up(25);

  gpio_init(ENCODER2_PIN_A);
  gpio_set_dir(ENCODER2_PIN_A, GPIO_IN);
  gpio_pull_up(ENCODER2_PIN_A);

  gpio_init(ENCODER2_PIN_B);
  gpio_set_dir(ENCODER2_PIN_B, GPIO_IN);
  gpio_pull_up(ENCODER2_PIN_B);

  gpio_init(28);
  gpio_set_dir(28, GPIO_IN);
  gpio_pull_up(28);
}

void scan_matrix() {
  for (int col = 0; col < COLS; col++) {
    gpio_put(col_pins[col], 1);  // set current column high
    sleep_us(50);                

    for (int row = 0; row < ROWS; row++) {
      // check if the key is pressed
      if (gpio_get(row_pins[row])) {  // if row is high, key is pressed
        key_state[row][col] = 1;    // set key as pressed
      }
      else {
        key_state[row][col] = 0;    // set key as not pressed
      }
    }

    gpio_put(col_pins[col], 0);  // set current column low again
  }
}

int last_a = 0;
int last_b = 0;
int last_a2 = 0;
int last_b2 = 0;

uint16_t vol_up = 0xE9;
uint16_t vol_down = 0xEA;
uint16_t mute = 0xE2;

bool button_pressed = false;
bool button2_pressed = false;

void handle_encoder()
{
  int a = gpio_get(ENCODER_PIN_A);
  int b = gpio_get(ENCODER_PIN_B);
  int d = gpio_get(25);
  
  int a2 = gpio_get(ENCODER2_PIN_A);
  int b2 = gpio_get(ENCODER2_PIN_B);
  int d2 = gpio_get(28);

  if (d == 0) {
    if (!button_pressed) {
      tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &mute, sizeof(mute));
      button_pressed = true;  
    }
  }
  else {
    button_pressed = false;
  }

  if (d2 == 0) {
    if (!button2_pressed) {
      tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &mute, sizeof(mute));
      button2_pressed = true;  
    }
  }
  else {
    button2_pressed = false;
  }

  if (a != last_a) {
    if (b != a) {
      tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &vol_up, sizeof(vol_up));
    }
    else {
      tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &vol_down, sizeof(vol_down));
    }
  }

  if (a2 != last_a2) {
    if (b2 != a2) {
      tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &vol_up, sizeof(vol_up));
    }
    else {
      tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &vol_down, sizeof(vol_down));
    }
  }

  last_a = a;
  last_b = b;
  last_a2 = a2;
  last_b2 = b2;
}

void send_hid_report()
{
  // skip if hid is not ready yet
  if ( !tud_hid_ready() ) return;

  uint8_t keycode[6] = {0};  
  int idx = 0;

  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      if (key_state[row][col] && idx < 6) {
        keycode[idx++] = (keymap[row][col]); 
      }
    }
  }

  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);

}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  scan_matrix();

  // Remote wakeup
  if (tud_suspended()) {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  }
  else {
    // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
    send_hid_report();

  }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) len;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;
}