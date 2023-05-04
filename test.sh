#!/bin/bash

function assert() {
	expected=$1
	input=$2
	output=`./pl0 "$input"`
	if [ "$output" = "$expected" ]; then
		echo "{$input} => $output"
	else
		echo "expected $expected, but got $output"
		exit 1
	fi
}

assert 3 'return 1 + 2.'
assert 3 'return 1 * 3.'
assert 7 'return 1 + 2 * 3.'
assert 10 'return 10 / 2 + 5.'
assert 3 'return 10 - 2 - 5.'
assert 3 'return 10 - (8 - 1).'
assert 3 'return 4/2+4/4.'
assert -5 'return 5-10.'
assert 2 'return (1-(1-2)).'
assert 0 'return (((0))).'
assert 1 'return - -1.'
assert 1 'if 1=1 then return 1.'

echo done!
