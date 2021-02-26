var assert = require('assert');

assert.assert(1);
// OK

assert.assert(true);
// OK

assert.assert(false);
// throws "AssertionError: false == true"

assert.assert(0);
// throws "AssertionError: 0 == true"

assert.doesNotThrow(
  function() {
    assert.assert(1);
  }
);
// OK

assert.doesNotThrow(
  function() {
    assert.assert(0);
  }
)

assert.equal(1, 1);
assert.equal(1, '1');

assert.fail(1, 2, undefined, '>');
// AssertionError: 1 > 2

assert.notEqual(1, 2);

