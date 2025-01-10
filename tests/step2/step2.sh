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
    echo "$@" > "$SCRIPT"
    test_from_direct_input "$@"
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
    output_test "./$BIN" "$@"
}

# @params: a list of strings
test_from_direct_input() {
    bash --posix -c "$@" > "$EXPECTED_OUT" 2> "$EXPECTED_ERR_OUT"
    "$F" -c "$@" > "$ACTUAL_OUT" 2> "$ACTUAL_ERR_OUT"
    output_test "./$BIN -c" "$@"
}

# @params: err code and then a list of strings
test_code_error()
{
    ERR="$1"
    shift
    "$F" -c "$@"
    ACTUAL_ERR="$?"
    if [ $ACTUAL_ERR -eq $ERR ]; then
        echo "$G[OK]$D $BIN -c \""$@"\"$D"
    else
        echo "COMMAND RUN : $R$F -c \""$@"\"$D"
        echo "EXPECTED $G$ERR$D. GOT $R$ACTUAL_ERR$D"
    fi
}

# @brief output the results
# @details compares stdouts and stderrs and prints the differences
# @params the command that was run
# @remark change the options of diff as u like eg. try -y (column), -u, -q
output_test() {
    FA=$1
    shift
    diff -y --color="always" "$EXPECTED_OUT" "$ACTUAL_OUT" > "$DIFF_OUT" 2>&1
    if [ $? -eq 0 ]; then
        diff -y --color="always" "$EXPECTED_ERR_OUT" "$ACTUAL_ERR_OUT" > "$DIFF_ERR" 2>&1
        if [ $? -eq 0 ]; then
            echo "$G[OK]$D $FA \""$@"\""
        else
            echo "$R[KO]$D"
            echo "$FA \""$@"\""
            E=$(cat -e "$EXPECTED_ERR")
            A=$(cat -e "$ACTUAL_ERR")
            echo "$G$E$D"
            echo "$R$A$D"
        fi
    else
            echo "$R[KO]$D"
            echo "$FA \""$@"\""
            E=$(cat -e "$EXPECTED_OUT")
            A=$(cat -e "$ACTUAL_OUT")
            echo "$G$E$D"
            echo "$R$A$D"
    fi
}
test_pipeline()
{
    echo "========== PIPELINE BEGIN =========="
    tes "echo Hello | tr a e"
    tes "echo Hello | tr a e | tr e a"
    tes "echo Hello | tr a e | tr e a | tr a e"
    tes "find -name *.c | echo"
    echo "========== PIPELINE END =========="
}
test_neg_pipeline()
{
    echo "========== NEGATION PIPELINE BEGIN =========="
    test_code_error 0 "! true | false"
    test_code_error 1 "! false | true"
    test_code_error 0 "! false | true | false | false | false | false"
    echo "========== NEGATION PIPELINE END =========="
}
test_ops()
{
    echo "========== OPS BEGIN =========="
    tes "true && false && false || echo a"
    tes "true && echo b && true || echo a"
    tes "true && ls && echo b || echo a"
    tes "true && false && echo b || echo a"
    tes "false && false && echo b || echo a"
    tes "false && echo b || echo a && true && false || true"
    tes "echo a && false || echo h"
    echo "========== OPS END =========="
}
test_var()
{
    for i in $(find tests/step2/assignement_substitution -name "*sh");
    do
    test_from_file $i;
    test_from_stdin $i;
    done;
}
test_non_builtin()
{
    echo "========== NON_BUILTIN BEGIN =========="
    tes "ls -a"
    tes "tree -L 2"
    tes "find -name *.c"
    echo "========== NON_BUILTIN END =========="
}
test_comment()
{
    echo "========== COMMENT BEGIN =========="
    tes echo "Ya un commentaire m c chill #de ouf c chill"
    tes echo "Ya un commentaire m c chill \# mais moi je suis pas un comment"
    tes 'echo \escaped \#escaped "#"quoted not#first #commented'
    echo "========== COMMENT END =========="
}
test_mix()
{
    echo "========== MIX BEGIN =========="
    tes 'echo -e \escaped \#escaped "#"quoted not#first #commented'
    echo
    tes 'if true; then echo -Ee "\escaped \#escaped "#"quoted not#first
    #commented"; fi'
    echo "========== MIX END =========="
}
test_errs()
{
    echo "========== ERROR_CODE BEGIN =========="
    # PARSE ERRS
    test_code_error 2 "false | | true"
    test_code_error 2 "| false || true"
    test_code_error 2 "false | true |"
    test_code_error 2 "|| false"
    test_code_error 2 "|| false"
    test_code_error 2 "|| false"
    test_code_error 2 "|| true &&"
    test_code_error 2 "false || &&"
    test_code_error 2 "true && true && false || || true"
    test_code_error 2 "echo | | |"
    # PIPELINE ERRS
    test_code_error 1 "true | false"
    test_code_error 0 "false | true"
    test_code_error 1 "false | true | false | false | false | false"
    # LEXER ERRS
    test_code_error 2 "if true; then echo a; \"fi"
    test_code_error 2 "\""
    test_code_error 2 "\"\"\""
    echo "========== ERROR_CODE END =========="
}
testsuite()
{
   test_pipeline
   test_neg_pipeline
   test_ops
   test_var
   test_comment
   test_mix
   test_errs
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
