#!/usr/bin/env node

const SerialPort = require('serialport')

async function run(argv){
  console.log(argv)

  var port = new SerialPort('/dev/ttyACM0', {baudRate: 115200})
  port.on('data', data => console.log(data.toString()))

  var controlType = (argv.e ? 'E': 'S').charCodeAt(0)

  // start byte: 0xFE
  // byte 1: E = encoder, S = slider
  // byte 2: physical control number (0-3 = encoders, 4 = slider)
  // byte 3: CC number
  // byte 4: CC increment value (encoders only)
  // byte 5: CC decrement value (encoders only)
  // end byte: 0xFF
  var buf = Buffer.from([0xFE, controlType, argv.p, argv.cc, argv.inc, argv.dec, 0xFF])
  console.log(buf)
  port.write(buf)
}

if (!module.parent){
  run(require('yargs')
    .boolean('e')
    .number('p')
    .number('inc')
    .number('dec')
    .number('cc')
    .argv
  )
}

