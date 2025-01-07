#!/bin/sh

# for storing stdout
EXPECTED_OUT=".original_fun.out"
ACTUAL_OUT=".my_fun.out"

# for storing stderr
EXPECTED_ERR_OUT="original_err.out"
ACTUAL_ERR_OUT="my_fun_err.out"

# stores the difference between the two stdout and stderr
DIFF_OUT="diff.out"
DIFF_ERR="diff_err.out"

#stores the shells commands
SCRIPT="script.sh"

# bin name and path
BIN="42sh"
F="./src/$BIN"

# for colors
G="\033[0;92m"
R="\033[0;91m"
D="\033[00m"

# @brief runs a test on all possible input ways
# @params: a list of strings
tes() {
    test_from_direct_input "$@"
    echo "$@" > "$SCRIPT"
    test_from_file "$SCRIPT"
    test_from_stdin "$SCRIPT"
}

# @params: a list of files
test_from_stdin() {
    bash --posix < "$@" > "$EXPECTED_OUT" 2> "$EXPECTED_ERR_OUT"
    "$F" < "$@" > "$ACTUAL_OUT" 2> "$ACTUAL_ERR_OUT"
    output_test "./$BIN < $@"
}

# @params: a list of files
test_from_file() {
    bash --posix "$@" > "$EXPECTED_OUT" 2> "$EXPECTED_ERR_OUT"
    "$F" "$@" > "$ACTUAL_OUT" 2> "$ACTUAL_ERR_OUT"
    output_test "./$BIN $@"
}

# @params: a list of strings
test_from_direct_input() {
    bash --posix -c "$@" > "$EXPECTED_OUT" 2> "$EXPECTED_ERR_OUT"
    "$F" -c "$@" > "$ACTUAL_OUT" 2> "$ACTUAL_ERR_OUT"
    output_test "./$BIN -c $@"
}

# @brief output the results
# @details compares stdouts and stderrs and prints the differences
# @params the command that was run
# @remark change the options of diff as u like eg. try -y (column), -u, -q
output_test() {
    diff -y --color="always" "$EXPECTED_OUT" "$ACTUAL_OUT" > "$DIFF_OUT" 2>&1
    if [ $? -eq 0 ]; then
        diff -y --color="always" "$EXPECTED_ERR_OUT" "$ACTUAL_ERR_OUT" > "$DIFF_ERR" 2>&1
        if [ $? -eq 0 ]; then
            echo "$G[OK]$D $1"
        else
            echo "$R[KO]$D"
            echo "$1"
            cat "$DIFF_ERR"
        fi
    else
            echo "$R[KO]$D"
            echo "$1"
            cat "$DIFF_OUT"
    fi
}

test_echo()
{
    echo "========== ECHO BEGIN =========="
    tes echo
    tes echo a
    tes echo 'a'
    tes echo aaa bbb ccc
    tes echo echo
    tes 'echo' 'a'
    echo "========== ECHO END =========="
}
testsuite()
{
    test_echo
}

testsuite
#================== making sure every file exists before deleting them
touch $EXPECTED_OUT
touch $ACTUAL_OUT
touch $EXPECTED_ERR_OUT
touch $ACTUAL_ERR_OUT
touch $DIFF_OUT
touch $DIFF_ERR
touch $SCRIPT
#==================-^
rm $EXPECTED_OUT
rm $ACTUAL_OUT
rm $EXPECTED_ERR_OUT
rm $ACTUAL_ERR_OUT
rm $DIFF_OUT
rm $DIFF_ERR
rm $SCRIPT
