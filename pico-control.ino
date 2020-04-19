#include <EEPROM.h>

#include <MIDIUSB.h>
#include <OneShotTimer.h>

struct ControlConf {
  uint8_t cc, inc, dec;
};

#define CC_CHANNEL    0
#define CC_MSG        0xB0
#define CC_MSG_HEADER 0x0B
#define SYS_START     0xFA
#define SYS_STOP      0xFC
#define SYS_CLOCK     0xF8
#define PULSES        24
#define BLINK_TIME    50
#define PIN_SLIDER    A0
#define SLIDER_FACTOR 8
#define SERIAL_BAUD   115200

OneShotTimer blink_timer;
midiEventPacket_t rx;
uint8_t clock_pulses = 0, serial_byte;
int slider_val[2] = {0, 1};
uint8_t conf_msg[5];
//char buf[64];

void control_change(uint8_t channel, uint8_t control, uint8_t value){
  midiEventPacket_t event = {CC_MSG_HEADER, CC_MSG | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void unblink(){
  digitalWrite(LED_BUILTIN, LOW);
}

void read_midi(){
  rx = MidiUSB.read();

  if (rx.header != 0){
    switch (rx.byte1){
      case SYS_START:
      case SYS_STOP:
        clock_pulses = 0;
        break;

      case SYS_CLOCK:
        if (++clock_pulses == PULSES){
          digitalWrite(LED_BUILTIN, HIGH);
          blink_timer.OneShot(BLINK_TIME, unblink);
          clock_pulses = 0;
        }
        break;
    }
  }
}

void read_slider(){
  slider_val[0] = analogRead(PIN_SLIDER) / SLIDER_FACTOR;

  if (slider_val[0] != slider_val[1]){
    control_change(CC_CHANNEL, 0, slider_val[0]);
    slider_val[1] = slider_val[0];
  }
}

uint8_t wait_byte(){
  while (!Serial.available()){}
  return Serial.read();
}

void read_conf_msg(){
  for (uint8_t i = 0; i < 5; i++){
    conf_msg[i] = wait_byte();
  }
}

void update_eeprom(){
  struct ControlConf conf = {conf_msg[2], conf_msg[3], conf_msg[4]};
  EEPROM.put(conf_msg[1] * sizeof(ControlConf), conf);
}

//void print_conf(uint8_t n){
//  struct ControlConf conf;
//  EEPROM.get(n * sizeof(ControlConf), conf);
//  sprintf(buf, "phys %u\ncc %u\ninc %u\ndec %u\n", n, conf.cc, conf.inc, conf.dec);
//  Serial.print(buf);
//}

void read_serial(){
  if (Serial.available()){
    serial_byte = Serial.read();

    switch (serial_byte){
      case 0xFE:
        read_conf_msg();
        break;

      case 0xFF:
        update_eeprom();
        //print_conf(conf_msg[1]);
        break;
    }  
  }
}

void setup(){
  Serial.begin(SERIAL_BAUD);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  blink_timer.Update();
  read_serial();
  read_midi();
  read_slider();
  MidiUSB.flush();
}

