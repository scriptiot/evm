var emitter = require('events').EventEmitter();

emitter.addListener('event', function() {
  print('emit event');
});

emitter.emit('event');
