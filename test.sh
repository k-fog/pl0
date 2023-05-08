#!/bin/bash

function assert() {
	expected=$1
	input=$2
	output=`./pl0 "$input"`
	if [ "$output" = "$expected" ]; then
		echo "{$input} => $output"
	else
		echo "{$input}: expected $expected, but got $output"
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
assert 1 'begin x:=1; return x; end.'
assert 3 'begin x:=1; y:=2; return x+y; end.'
assert 2 'begin x:=1; x:=2; return x; end.'
assert 1 'begin x:=0; begin x:=1; end; return x; end.'
assert 55 'begin x:=0; i:=0; while i<=10 do begin x:=x+i; i:=i+1; end; return x; end.'
assert 2 'function f() return 2; return f().'
assert 10 'function add(x,y) return x+y; return add(2,8).'
assert 6 'function mul(x,y,z) return x*y*z; return mul(1,2,3).'
assert 120 'function fact(n) begin if n<=1 then return 1; return n*fact(n-1); end; return fact(5).'

echo OK!
