#include <MIDIUSB.h>
#include <OneShotTimer.h>

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

OneShotTimer blink_timer;
midiEventPacket_t rx;
uint8_t clock_pulses = 0;
int slider_val[2] = {0, 1};

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
    if (rx.byte1 == SYS_START || rx.byte1 == SYS_STOP){
      clock_pulses = 0;
    }

    if (rx.byte1 == SYS_CLOCK){
      clock_pulses++;

      if (clock_pulses == PULSES){
        digitalWrite(LED_BUILTIN, HIGH);
        blink_timer.OneShot(BLINK_TIME, unblink);
        clock_pulses = 0;
      }
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

void setup(){
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  blink_timer.Update();
  read_midi();
  read_slider();
  MidiUSB.flush();
}

