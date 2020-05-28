# pico-control
simple MIDI controller

## cc messages
* slider: `cc=0x00`, `value=0-127`
* encoders: `cc=0x01-0x05`, `cw=1`, `ccw=127`
* buttons: `cc=0x06-0x0B`, `value=127`
* longpress: `cc=0x0C-0x11`, `value=127`

## upload
`make upload`

## debug
* `#define DEBUG`
* `make serial`

## mixxx
see `mixxx/pico-control.js` for examples
