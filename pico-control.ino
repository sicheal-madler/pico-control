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

OneShotTimer blink_timer;
midiEventPacket_t rx;
uint8_t clock_pulses;

void control_change(byte channel, byte control, byte value){
  midiEventPacket_t event = {CC_MSG_HEADER, CC_MSG | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void unblink(){
  digitalWrite(LED_BUILTIN, LOW);
}

void setup(){
  clock_pulses = 0;
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  blink_timer.Update();
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

  if (Serial.available()){
    switch (Serial.read()){
      case '1':
        control_change(CC_CHANNEL, 1, 127);
        break;
      case '2':
        control_change(CC_CHANNEL, 1, 1);
        break;
      case '3':
        control_change(CC_CHANNEL, 2, 127);
        break;
      case '4':
        control_change(CC_CHANNEL, 2, 1);
        break;
    }

    MidiUSB.flush();
  }
}

