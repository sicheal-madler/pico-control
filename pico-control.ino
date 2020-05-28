//#include <EEPROM.h>

#include <MIDIUSB.h>
//#include <OneShotTimer.h>
#include <SimpleRotary.h>
#include <JC_Button.h>

//struct ControlConf {
//  uint8_t cc, inc, dec;
//};

#define DEBUG

#define CC_CHANNEL        0
#define CC_MSG            0xB0
#define CC_MSG_HEADER     0x0B
#define SYS_START         0xFA
#define SYS_STOP          0xFC
#define SYS_CLOCK         0xF8
#define PULSES            24
#define BLINK_TIME        50
#define SLIDER_FACTOR     8
#define SLIDER_INVERT
#define SERIAL_BAUD       115200
#define RELATIVE_DEC      127
#define RELATIVE_INC      1
#define CC_DELAY          250
#define ENC_OFFSET        1
#define BUTTON_OFFSET     6
#define BUTTON_OFFSET2    11
#define BUTTON_LONGPRESS  300

#define SLIDER_PIN    A0
#define BEAT_LED_PIN  14
#define ENC0_PIN1     2
#define ENC0_PIN2     3
#define ENC1_PIN1     4
#define ENC1_PIN2     5
#define ENC2_PIN1     6
#define ENC2_PIN2     7
#define ENC3_PIN1     8
#define ENC3_PIN2     9
#define ENC4_PIN1     10
#define ENC4_PIN2     16
#define SW0_PIN       14
#define SW1_PIN       15
#define SW2_PIN       A1
#define SW3_PIN       A2
#define SW4_PIN       A3

SimpleRotary enc0(ENC0_PIN1, ENC0_PIN2, -1);
SimpleRotary enc1(ENC1_PIN1, ENC1_PIN2, -1);
SimpleRotary enc2(ENC2_PIN1, ENC2_PIN2, -1);
SimpleRotary enc3(ENC3_PIN1, ENC3_PIN2, -1);
SimpleRotary enc4(ENC4_PIN1, ENC4_PIN2, -1);
SimpleRotary* encoders[5] = {&enc0, &enc1, &enc2, &enc3, &enc4};

Button b0(SW0_PIN);
Button b1(SW1_PIN);
Button b2(SW2_PIN);
Button b3(SW3_PIN);
Button b4(SW4_PIN);
Button* buttons[5] = {&b0, &b1, &b2, &b3, &b4};

//OneShotTimer blink_timer;
//midiEventPacket_t rx;
midiEventPacket_t midi_tx;

//uint8_t clock_pulses = 0, serial_byte;
int slider_val[2] = {0, 1};
uint32_t last_button_cc[5] = {0, 0, 0, 0, 0};
//uint8_t conf_msg[5];

#ifdef DEBUG
char buf[64];
#endif

void control_change(uint8_t channel, uint8_t control, uint8_t value){
#ifdef DEBUG
  sprintf(buf, "[OUT] chan=%u cc=%u val=%u", channel, control, value);
  Serial.println(buf);
#endif

  midi_tx = {CC_MSG_HEADER, CC_MSG | channel, control, value};
  MidiUSB.sendMIDI(midi_tx);
}

//void unblink(){
//  digitalWrite(BEAT_LED_PIN, LOW);
//}
//
//void read_midi(){
//  rx = MidiUSB.read();
//
//  if (rx.header != 0){
//    switch (rx.byte1){
//      case SYS_START:
//      case SYS_STOP:
//        clock_pulses = 0;
//        break;
//
//      case SYS_CLOCK:
//        if (++clock_pulses == PULSES){
//          digitalWrite(BEAT_LED_PIN, HIGH);
//          blink_timer.OneShot(BLINK_TIME, unblink);
//          clock_pulses = 0;
//        }
//        break;
//    }
//  }
//}

void read_slider(){
  slider_val[0] = analogRead(SLIDER_PIN) / SLIDER_FACTOR;

#ifdef SLIDER_INVERT
  slider_val[0] = 127 - slider_val[0];
#endif

  if (slider_val[0] != slider_val[1]){
    control_change(CC_CHANNEL, 0, slider_val[0]);
    slider_val[1] = slider_val[0];
  }
}

void read_encoder(uint8_t n){
  uint8_t val = encoders[n]->rotate();

  switch (val){
    case 1:
      control_change(CC_CHANNEL, n + ENC_OFFSET, RELATIVE_DEC);
      break;
    case 2:
      control_change(CC_CHANNEL, n + ENC_OFFSET, RELATIVE_INC);
      break;
  }
}

void read_button(uint8_t n){
  uint32_t now = millis();

  if (buttons[n]->read() && now - last_button_cc[n] >= CC_DELAY){
    control_change(CC_CHANNEL, n + BUTTON_OFFSET, 127);
    last_button_cc[n] = now;
  }

  if (buttons[n]->pressedFor(BUTTON_LONGPRESS)){
    control_change(CC_CHANNEL, n + BUTTON_OFFSET2, 127);
  }
}

//uint8_t wait_byte(){
//  while (!Serial.available()){}
//  return Serial.read();
//}
//
//void read_conf_msg(){
//  for (uint8_t i = 0; i < 5; i++){
//    conf_msg[i] = wait_byte();
//  }
//}
//
//void update_eeprom(){
//  struct ControlConf conf = {conf_msg[2], conf_msg[3], conf_msg[4]};
//  EEPROM.put(conf_msg[1] * sizeof(ControlConf), conf);
//}

//void print_conf(uint8_t n){
//  struct ControlConf conf;
//  EEPROM.get(n * sizeof(ControlConf), conf);
//  sprintf(buf, "phys %u\ncc %u\ninc %u\ndec %u\n", n, conf.cc, conf.inc, conf.dec);
//  Serial.print(buf);
//}

//void read_serial(){
//  if (Serial.available()){
//    serial_byte = Serial.read();
//
//    switch (serial_byte){
//      case 0xFE:
//        read_conf_msg();
//        break;
//
//      case 0xFF:
//        update_eeprom();
//        //print_conf(conf_msg[1]);
//        break;
//    }  
//  }
//}

void setup(){
#ifdef DEBUG
  Serial.begin(SERIAL_BAUD);
#endif

  //pinMode(BEAT_LED_PIN, OUTPUT);

  for (uint8_t i = 0; i < 5; i++){
    buttons[i]->begin();
  }
}

void loop(){
  //blink_timer.Update();

  //read_serial();
  //read_midi();
  read_slider();

  for (uint8_t i = 0; i < 5; i++){
    read_encoder(i);
  }

  for (uint8_t i = 0; i < 5; i++){
    read_button(i);
  }

  MidiUSB.flush();
}

