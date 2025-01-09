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

test_echo_basic()
{
    echo "========== ECHO BEGIN =========="
    tes "echo     "
    tes 'echo     a'
    tes echo 'a'
    tes 'echo "a"'
    tes echo aaa bbb ccc
    tes echo echo
    tes echo "\nouaaaaa\n\n\nla dinguerie\\\n\t"
    tes 'echo' 'a'
    tes "echo foo; echo 'a'"
    echo "========== ECHO END =========="
}
test_echo_options()
{
    echo "========== ECHO OPTIONS BEGINING =========="
    tes echo -n "just a chill line\n"
    tes echo -e "\n\nhello\nici\n\n\t"
    tes echo -e "\nimpossible\n\n\nla dinguerie\n"
    tes echo -e "\tde ouf\t\t\tla dinguerie\n\t"
    tes echo -e "ooooo \nc\n une\n dinguerie"
    tes echo -E "\nouaaaaa\n\n\nla dinguerie\n"
    tes echo -E "\tmaiiiis naaaaan\t\t\tla dinguerie\n\t"
    tes echo -E "\\juuuuuuure\\ c\\ une \\ dinguerie\\\t"
    tes echo -E "\tjuuuuuuure\nc\t une \t dinguerie\\\n"
    tes echo -nE "\tjuuuuuuure\nc\t une \t dinguerie\\\n"
    tes echo -e "\tjuuuuuuure\nc\t une \t dinguerie\\\n"
    tes echo -neE "\tjuuuuuuure\nc\t une \t dinguerie\\\n"
    tes echo -Ee "\tjuuuuuuure\nc\t une \t dinguerie\\\n"
    echo "========== ECHO OPTIONS END =========="
}
test_if()
{
    echo "========== IF BEGIN =========="
    tes "if true; then echo a; fi" 
    tes "if false; then echo a; fi" 
    tes "if false; then echo a; echo b; fi" 
    tes "if true; then if true; then if true; then echo a; fi fi fi"
    echo "========== IF END =========="
}
test_elif()
{
    echo "========== ELIF BEGIN =========="
    tes "if false; then echo a; elif true; then echo b; fi"
    tes "if false; then echo a; elif false; then echo b;
                        elif true; then echo c; fi"
    echo "========== ELIF END =========="
}
test_else()
{
    echo "========== ELSE BEGIN =========="
    tes "if false; then echo a; else echo b; fi"
    tes "if true; then echo a; else echo b; fi"
    echo "========== ELSE END =========="
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
testsuite()
{
    test_echo_basic
    test_echo_options
    test_if
    test_elif
    test_else
    test_comment
    test_mix
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
