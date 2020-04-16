#include <MIDIUSB.h>
#include <OneShotTimer.h>

#define CC_CHANNEL 0

OneShotTimer blink_timer;
midiEventPacket_t rx;
uint8_t clock_pulses;

void control_change(byte channel, byte control, byte value){
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
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
    if (rx.byte1 == 0xFA || rx.byte1 == 0xFC){
			clock_pulses = 0;
		}

		if (rx.byte1 == 0xF8){
			clock_pulses++;

			if (clock_pulses == 24){
				digitalWrite(LED_BUILTIN, HIGH);
				blink_timer.OneShot(50, unblink);
				clock_pulses = 0;
			}
		}
  }

  if (Serial.available()){
    control_change(CC_CHANNEL, Serial.read() - 48, 66);
    MidiUSB.flush();
  }
}

