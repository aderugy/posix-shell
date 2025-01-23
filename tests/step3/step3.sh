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
F="$BIN_PATH"
# for colors
#G=""
#R=""
#D=""

G="\033[0;32m"
R="\033[0;31m"
D="\033[0m"

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
test_pars_lex_error() {
  ERR="$1"
  shift
  "$F" -c "$@" >/dev/null 2>/dev/null
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

# @brief output the results
# @details compares stdouts and stderrs and prints the differences
# @params the command that was run
# @remark change the options of diff as u like eg. try -y (column), -u, -q
output_test() {
  FA=$1
  shift
  diff -y --color="always" "$EXPECTED_OUT" "$ACTUAL_OUT" >"$DIFF_OUT" 2>&1
  if [ $? -eq 0 ]; then
    diff -y --color="always" "$EXPECTED_ERR_OUT" "$ACTUAL_ERR_OUT" >"$DIFF_ERR" 2>&1
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

test_cd() {
  echo "========== CD BEGIN =========="
  tes 'cd . && echo $PWD'
  tes 'cd && echo $PWD'
  tes 'cd .. && echo $PWD'
  tes 'cd ../ && echo $PWD'
  echo "========== CD END =========="
}
test_functions() {
  echo "========== FUNCTIONS BEGIN =========="
  for i in $(find step3/"functions" -name "*sh"); do
    test_from_file $i 1 2 4
    test_from_stdin $i
  done
  echo "========== FUNCTIONS END =========="
}
test_subshells() {
  echo "========== SUBSHELL BEGIN =========="
  for i in $(find step3/"subshell" -name "*sh"); do
    test_from_file $i
    test_from_stdin $i
  done
  echo "========== SUBSHELL END =========="
}
testsuite() {
  test_cd
  test_functions
  test_subshells
}

testsuite

echo $F
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
