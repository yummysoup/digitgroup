#!/bin/sh

digitgroup_cmd="/tmp/digitgroup_$$"
gcc digitgroup.c -o "$digitgroup_cmd"
if [ $? -ne 0 ]; then
    echo "Compilation failed"
    exit 1
fi

cleanup() {
    rm -f "$digitgroup_cmd"
}

trap cleanup EXIT

testcase() {
    input=$1
    expected_output=$2
    actual_output=$(echo "$input" | "$digitgroup_cmd")
    exp_b64=$(echo "$expected_output" | base64)
    act_b64=$(echo "$actual_output" | base64)

    if [ "$act_b64" = "$exp_b64" ]; then
        echo "PASS: '$input' -> '$expected_output'"
    else
        echo "FAIL: '$input' -> Expected: '$expected_output', Got: '$actual_output'"
        echo "FAIL: '$input' -> Expected: '$exp_b64', Got: '$act_b64'"
        exit 1
    fi
}

START="\033[31m"
END="\033[0m"

testcase '123' '123'
testcase '1234' "${START}1${END}234"
testcase 'hello, world!' 'hello, world!'
testcase '$123456789.123' "\$123${START}456${END}789.123"
testcase '1 22 333 4444 55555 666666 7777777 88888888 999999999 0000000000' "1 22 333 ${START}4${END}444 ${START}55${END}555 ${START}666${END}666 7${START}777${END}777 88${START}888${END}888 999${START}999${END}999 ${START}0${END}000${START}000${END}000"
testcase "1 22 333\n4444 55555 666666\n7777777 88888888 999999999 0000000000" "1 22 333\n${START}4${END}444 ${START}55${END}555 ${START}666${END}666\n7${START}777${END}777 88${START}888${END}888 999${START}999${END}999 ${START}0${END}000${START}000${END}000"

