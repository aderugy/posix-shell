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
BIN="src/42sh"
F=$BIN_PATH
# for colors
G="\033[0;32m"
R="\033[0;31m"
D="\033[0m"

OUTPUT_CODE_RETURN=0

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
  OUTPUT_CODE_RETURN=$?
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
  "$F" -c "$@" 2>/dev/null
  ACTUAL_ERR="$?"
  ACTUAL_MSG=$("$F" -c "$@" 2>&1)
  if [ $ACTUAL_ERR -eq $ERR ] && [ -n "$ACTUAL_MSG" ]; then
    echo "$G[OK]$D ./$BIN -c \""$@"\"$D"
    PASSED_TEST=$((PASSED_TEST + 1))
  else
    echo "$R[KO]$D"
    echo "COMMAND RUN : $R$F -c \""$@"\"$D"
    if ! [ $ACTUAL_ERR -eq $ERR ]; then
      echo "EXPECTED $G$ERR$D. GOT $R$ACTUAL_ERR$D"
    fi
    if [ -z "$ACTUAL_MSG" ]; then
      echo "EXPECTED ERROR MESSAGE !"
    fi
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
    if [ $? -eq 0 ] && [ $OUTPUT_CODE_RETURN -eq 0 ]; then
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

test_var() {
  echo "========== VARIABLES BEGIN =========="
  for i in $(find step2/assignement_substitution -name "*sh"); do
    test_from_file $i
    test_from_stdin $i
  done
  echo "========== VARIABLES END =========="
}

test_for() {
  echo "========== FOR BEGIN =========="
  tes "for i in 1 2 3; do echo \"Value: \$i\"; done"
  tes "for cmd in ls nonexistentcommand pwd; do \$cmd || echo \"Error with \$cmd\"; done"
  tes "for i in 1 2; do for j in a b; do echo \"\$i\$j\"; done; done"
  echo "========== FOR END =========="
}

test_errs() {
  echo "========== ERROR_CODE BEGIN =========="
  # PARSE ERRS
  test_code_error 0 "echo a; echa b; echo c"
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
  test_code_error 2 "if then echo a"
  test_code_error 2 "if ;then echo a"
  test_code_error 2 "until test"
  # QUOTE ERRS
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
  for i in $(find step2/special_var -name "*sh"); do
    test_from_file $i 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
    test_from_stdin $i
  done
  echo "========== SPE_VARS END =========="
}
test_quoting() {
  echo "========== QUOTING BEGIN =========="
  tes 'echo "42\nsh"'
  tes 'echo "| & ; < > ( ) % ^ \\ * - + =  # ~ !"'
  tes 'echo "\$\`\\"'
  tes 'echo "imagine\n" "mais attent le truc de \nfou\n"'
  tes 'A=mmmmm; echo "\$Apasse stp mon reuf"'
  tes 'echo "JEsuisTONpere\$JEsuisTONpere\=JEsuisTONpere\-tuJEsuisTONpere\%tiJEsuisTONpere\^ta\_"'
  tes 'echo "\% \# \^ \\\ \: \;"'
  tes "echo c\'est le quantine"
  tes "echo "\"\"\'""
  echo "========== QUOTING END =========="
}
testsuite() {
  test_var
  #test_mix_grammar
  #test_quoting
  test_special_vars
  test_errs
}

testsuite
echo -e "TEST : $TOTAL_TEST\nPASSED TEST : $PASSED_TEST\n"
res=$((100 * $PASSED_TEST / $TOTAL_TEST))
echo "STEP2 : $res% PASSED"
#================== making sure every file exists before deleting them
touch $EXPECTED_OUT
touch $ACTUAL_OUT
touch $EXPECTED_ERR_OUT
touch $ACTUAL_ERR_OUT
touch $DIFF_OUT
touch $DIFF_ERR
touch $SCRIPT
touch dum.out
#==================-^
rm $EXPECTED_OUT
rm $ACTUAL_OUT
rm $EXPECTED_ERR_OUT
rm $ACTUAL_ERR_OUT
rm $DIFF_OUT
rm $DIFF_ERR
rm $SCRIPT
rm dum.out
