TARGET := pico-control
FQBN := arduino:avr:leonardo
PORT := /dev/ttyACM0

compile:
	arduino-cli compile --fqbn $(FQBN) ../$(TARGET)

upload: compile
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) ../$(TARGET)

