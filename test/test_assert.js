var assert = require('assert');

assert.assert(1);
// OK

assert.assert(true);
// OK

assert.assert(false);
// throws "AssertionError: false == true"

assert.assert(0);
// throws "AssertionError: 0 == true"