var PicoControl = {
  DEFAULT_STEP: 0.05,

  ENC_CONF: {
    1: {
      group: '[Master]',
      key: 'volume',
      spread: 5.0
    },
    2: {
      group: '[Master]',
      key: 'balance',
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
  var conf = this.ENC_CONF[ctrl]
  var mult = val == 0x7F ? -1: 1
  var curVal = engine.getValue(conf.group, conf.key)
  var newVal = curVal + (mult * conf.spread * (conf.step || this.DEFAULT_STEP))

  engine.setValue(conf.group, conf.key, newVal)
}

