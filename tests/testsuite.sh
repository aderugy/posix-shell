#!/bin/sh
REF_OUT=".original_fun.out"
TEST_OUT=".my_fun.out"
REF_ERR_OUT="original_err.out"
TEST_ERR_OUT="my_fun_err.out"
DIFF_OUT="diff.out"
DIFF_ERR="diff_err.out"
BIN="42sh"
F="./src/$BIN"

G="\033[0;92m"
R="\033[0;91m"
D="\033[00m"

#generates file named $1 and change owner/group to $2/$3
tes() {
    "$F" "$@" > "$REF_OUT" 2> "$REF_ERR_OUT"
     "$@" | bash --posix> "$TEST_OUT" 2> "$TEST_ERR_OUT"
    output_test 
}
output_test() {
    diff -u --color="always" "$REF_OUT" "$TEST_OUT" > "$DIFF_OUT" 2>&1
    if [ $? -eq 0 ]; then
        diff -u --color="always" "$REF_ERR_OUT" "$TEST_ERR_OUT" > "$DIFF_ERR" 2>&1
        if [ $? -eq 0 ]; then
            echo "$G[OK]$D ./$BIN $@"
        else
            echo "$R[KO]$D"
            echo "./$BIN $@"
            cat "$DIFF_ERR"
        fi
    else
            echo "$R[KO]$D"
            echo "./$BIN $@"
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
touch $REF_OUT
touch $TEST_OUT
touch $REF_ERR_OUT
touch $TEST_ERR_OUT
touch $DIFF_OUT
touch $DIFF_ERR
rm $REF_OUT
rm $TEST_OUT
rm $REF_ERR_OUT
rm $TEST_ERR_OUT
rm $DIFF_OUT
rm $DIFF_ERR
