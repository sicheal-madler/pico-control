var PicoControl = {

  // increase/decrease by 5%
  // specify 'step' in conf below to override
  DEFAULT_STEP: 0.05,

  // this is hard coded to work with CC 1-5
  ENC_CONF: {
    1: {
      // master list of groups/keys is in mixxx doc
      group: '[Master]',
      key: 'volume',

      // range of the control (master volume is 0 --> 5.0)
      // also in mixxx doc
      spread: 5.0
    },
    2: {
      group: '[Master]',
      key: 'balance',

      // range is -1.0 -> 1.0
      spread: 2.0
    },
    3: {
      group: '[Channel1]',
      key: 'pitch_adjust',
      spread: 6.0
    },
    4: {
      group: '[Channel1]',
      key: 'rate',
      spread: 2.0
    },
    5: {
      group: '[Channel1]',
      key: 'volume',
      spread: 5.0
    }
  }
}

PicoControl.init = function(){}
PicoControl.shutdown = function(){}

PicoControl.encoder = function(chan, ctrl, val, status, group){

  // get conf based on cc
  var conf = this.ENC_CONF[ctrl]

  // for encoders, 127 is decrease and 1 is increase
  var mult = val == 0x7F ? -1: 1

  // add incremental value to current value
  var curVal = engine.getValue(conf.group, conf.key)
  var newVal = curVal + (mult * conf.spread * (conf.step || this.DEFAULT_STEP))

  // actually set value
  engine.setValue(conf.group, conf.key, newVal)
}

