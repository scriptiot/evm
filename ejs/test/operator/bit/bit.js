print('test -- bitwise-logic');
assert((5 & 2) === 0);
assert((2 & 2) === 2);
assert((5 | 2) === 7);
assert((5 | 5) === 5);
assert((5 ^ 2) === 7);
assert((5 ^ 5) === 0);
assert((~5) == -6);