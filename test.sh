#!/bin/bash
assert(){
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
	cc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]
       	then
		echo "$input => $actual"
	else
		echo "$input => $expected expected but get $actual"
		exit 1
	fi
}

assert 0 "5==1+3"
assert 1 "1 < 2"
assert 0 "-10 > 3*4"
assert 10 "-15*+2+40"

echo OK
