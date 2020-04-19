TARGET := pico-control
FQBN := arduino:avr:leonardo
PORT := /dev/ttyACM0
BAUD := 115200
CONF_APP := update-conf.js
BUILD := build

compile:
	arduino-cli compile --fqbn $(FQBN) ../$(TARGET)

upload: compile
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) ../$(TARGET)

serial:
	picocom -b $(BAUD) $(PORT)

pkg:
	pkg $(CONF_APP) --out-path $(BUILD)

