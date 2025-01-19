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
DUMMY="dum.out"

# bin name and path

F=$BIN_PATH
# for colors
G=""
R=""
D=""

# @brief runs a test on all possible input ways
# @params: a list of strings
tes() {
    echo "$@" >"$SCRIPT"
    test_from_direct_input "$@"
    test_from_file "$SCRIPT"
    test_from_stdin "$SCRIPT"
}

# @params: a list of files
test_from_stdin() {
    bash --posix <"$@" >"$EXPECTED_OUT" 2>"$EXPECTED_ERR_OUT"
    "$F" <"$@" >"$ACTUAL_OUT" 2>"$ACTUAL_ERR_OUT"
    output_test "./$BIN < $@"
}

# @params: a list of files
test_from_file() {
    bash --posix "$@" >"$EXPECTED_OUT" 2>"$EXPECTED_ERR_OUT"
    "$F" "$@" >"$ACTUAL_OUT" 2>"$ACTUAL_ERR_OUT"
    output_test "./$BIN" "$@"
}

# @params: a list of strings
test_from_direct_input() {
    bash --posix -c "$@" >"$EXPECTED_OUT" 2>"$EXPECTED_ERR_OUT"
    "$F" -c "$@" >"$ACTUAL_OUT" 2>"$ACTUAL_ERR_OUT"
    output_test "./$BIN -c" "$@"
}

# @params: err code and then a list of strings
test_code_error() {
    TOTAL_TEST=$((TOTAL_TEST + 1))
    ERR="$1"
    shift
    "$F" -c "$@"
    ACTUAL_ERR="$?"
    if [ $ACTUAL_ERR -eq $ERR ]; then
        echo "$G[OK]$D $BIN -c \""$@"\"$D"
        PASSED_TEST=$((PASSED_TEST + 1))
    else
        echo "COMMAND RUN : $R$F -c \""$@"\"$D"
        echo "EXPECTED $G$ERR$D. GOT $R$ACTUAL_ERR$D"
    fi
}

# @params: a list of strings
test_redirections() {
    "$F" -c "$@ $ACTUAL_OUT"
    bash --posix -c "$@ $EXPECTED_OUT"
    output_test "./$BIN" "$@"
}
# @brief output the results
# @details compares stdouts and stderrs and prints the differences
# @params the command that was run
# @remark change the options of diff as u like eg. try -y (column), -u, -q
output_test() {
    TOTAL_TEST=$((TOTAL_TEST + 1))
    FA=$1
    shift
    diff -y --color="always" "$EXPECTED_OUT" "$ACTUAL_OUT" >"$DIFF_OUT" 2>&1
    if [ $? -eq 0 ]; then
        diff -y --color="always" "$EXPECTED_ERR_OUT" "$ACTUAL_ERR_OUT" >"$DIFF_ERR" 2>&1
        if [ $? -eq 0 ]; then
            echo "$G[OK]$D $FA \""$@"\""
            PASSED_TEST=$((PASSED_TEST + 1))
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
test_pipeline() {
    echo "========== PIPELINE BEGIN =========="
   tes "echo Hello '|' tr a e"
   echo "========== PIPELINE END =========="
}


test_for() {
    echo "========== FOR BEGIN =========="
    tes "for i in 1 2 3; do echo \"Value: \$i\"; done"
    tes "for cmd in ls nonexistentcommand pwd; do \$cmd || echo \"Error with \$cmd\"; done"
    tes "for i in 1 2; do for j in a b; do echo \"\$i\$j\"; done; done"
    echo "========== FOR END =========="
}

test_while_loops() {
    echo "========== WHILE LOOP BEGIN =========="
    tes "while read -r line; do echo \"Line: \$line\"; done < <(echo -e \"first\\nsecond\\nthird\")"
    echo "========== WHILE LOOP END =========="
}
test_var() {
    echo "========== VARIABLES BEGIN =========="
    for i in $(find step2/assignement_substitution -name "*sh"); do
        test_from_file $i
        test_from_stdin $i
    done
    echo "========== VARIABLES END =========="
}

test_non_builtin() {
    echo "========== NON_BUILTIN BEGIN =========="
    tes "ls -a; ls; ls"
    tes "tree -L 2"
    tes "find -name *.c"
    echo "========== NON_BUILTIN END =========="
}

test_redirections() {
    echo "========== REDIRECTIONS BEGIN =========="
    tes "echo lalalalalala > $DUMMY;echo < $DUMMY"
    tes "echo <> $DUMMY"
    tes "ls | echo > $DUMMY; echo < $DUMMY"
    tes "echo tchou > $DUMMY;echo bebe >> $DUMMY; cat $DUMMY"
    tes "echo tchou > $DUMMY;echo boubou > $DUMMY; cat $DUMMY"
    tes "echo <1 ls && echo bbbb > $DUMMY; echo $DUMMY"
    tes "echo <1 ls && echo bbbb 1> $DUMMY; echo < $DUMMY"
    tes "ls 1> $DUMMY; cat $DUMMY"
    tes "echo 'Hello, World!' > file1; cat file1"
    tes "echo 'Line 1' > file1; echo 'Line 2' >> file1; cat file1"
    tes "ls valid_file invalid_file > out.txt 2> err.txt; cat out.txt err.txt"
    tes "ls valid_file invalid_file > combined.txt 2>&1; cat combined.txt"
    tes "cat < file1"
    tes "cat < file1 > out.txt 2>&1; cat out.txt"
    tes "echo 'Message' 1> file1 2> file2; cat file1 file2; rm file1 file2"
    tes "ls non_existing_file 3>&2 2>&1 1>&3"
    tes "echo 'Test' > /root/protected_file 2>&1"
    tes "cat non_existent_file 2> err.txt; cat err.txt"
    tes "echo 'This will not appear' > /dev/null"
    tes "ls invalid_file 2> /dev/null"
    tes "ls > text ; sort < text ; rm text"
    tes "echo Salut |> text; rm text"
    tes "echo Salut >| text; rm text"

    echo "========== REDIRECTIONS END =========="
}
test_comment() {
    echo "========== COMMENT BEGIN =========="
    tes echo "Ya un commentaire m c chill #de ouf c chill"
    tes echo "Ya un commentaire m c chill \# mais moi je suis pas un comment"
    tes 'echo \escaped \#escaped "#"quoted not#first #commented'
    echo "========== COMMENT END =========="
}
test_mix_grammar() {
    echo "========== MIXED GRAMMAR TESTS BEGIN =========="
    tes "for word in \$(echo \"one two three\" | tr ' ' '\n'); do echo \"Word: \$word\" | rev; done"
    echo "========== MIXED GRAMMAR TESTS END =========="
}
test_errs() {
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
    test_code_error 2 "'if' true; then echo a; fi"
    test_code_error 127 "'if' 'true'; 'then' echo a; 'fi'"
    # LEXER ERRS
    test_code_error 2 "if true; then echo a; \"fi"
    test_code_error 2 "\""
    test_code_error 2 "\"\"\""
    test_code_error 2 "for test"
    test_code_error 2 "while test"
    test_code_error 127 "''"
    test_code_error 2 "'"
    echo "========== ERROR_CODE END =========="
}


test_special_vars() {
    echo "========== SPE_VARS BEGIN =========="
    for i in $(find tests/step2/special_var -name "*sh"); do
        test_from_file $i 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
        test_from_stdin $i
    done
    echo "========== SPE_VARS END =========="
}
testsuite() {
    test_pipeline
    test_var
    #test_comment
    #test_for
    test_while_loops
    #test_mix_grammar
    #test_quoting
    test_redirections
    test_special_vars
    test_errs
}

testsuite
echo -e "TEST : $TOTAL_TEST\nPASSED TEST : $PASSED_TEST\n"
res=$((100 * $PASSED_TEST / $TOTAL_TEST))
echo "COVERAGE : $res%"
#================== making sure every file exists before deleting them
touch $EXPECTED_OUT
touch $ACTUAL_OUT
touch $EXPECTED_ERR_OUT
touch $ACTUAL_ERR_OUT
touch $DIFF_OUT
touch $DIFF_ERR
touch $SCRIPT
touch $DUMMY
#==================-^
rm $EXPECTED_OUT
rm $ACTUAL_OUT
rm $EXPECTED_ERR_OUT
rm $ACTUAL_ERR_OUT
rm $DIFF_OUT
rm $DIFF_ERR
rm $SCRIPT
rm $DUMMY
