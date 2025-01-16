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
BIN="42sh"
F="./src/$BIN"

# for colors
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
test_code_error() {
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
test_pipeline() {
  echo "========== PIPELINE BEGIN =========="
  tes "echo Hello | tr a e"
  tes "echo Hello | tr a e | tr e a"
  tes "echo Hello | tr a e | tr e a | tr a e"
  tes "find -name *.c | echo"
  tes "tree -L 2 | echo | tr e a | tr c b"
  echo "========== PIPELINE END =========="
}
test_neg_pipeline() {
  echo "========== NEGATION PIPELINE BEGIN =========="
  test_code_error 0 "! false | true | true | true | false"
  test_code_error 0 "! false | true | false | false | false | false"
  test_code_error 0 "! true | false"
  test_code_error 1 "! false | true"
  test_code_error 1 "! ls | echo"
  echo "========== NEGATION PIPELINE END =========="
}
test_ops() {
  echo "========== OPS BEGIN =========="
  tes "true && false && false || echo a"
  tes "true && echo b && true || echo a"
  tes "true && ls && echo b || echo a"
  tes "true && false && echo b || echo a"
  tes "false && false && echo b || echo a"
  tes "false && false && echo b; echo c && ls || echo a"
  tes "false && false || echo b; echo c && ls || echo a"
  tes "false && echo b || echo a && true && false || true"
  tes "echo a && false || echo h"
  tes "echo a && ls || echo h"
  tes "echo afasfag && echo asfbfhsafbs && tree || echo h"
  echo "========== OPS END =========="
}
test_for() {
  echo "========== FOR BEGIN =========="
  tes "for i in 1 2 3; do echo \"Value: \$i\"; done"
  tes "for file in \$(ls); do echo \"File: \$file\"; done"
  tes "for i in \$(seq 1 5); do echo \"Number: \$i\"; done"
  tes "for cmd in ls nonexistentcommand pwd; do \$cmd || echo \"Error with \$cmd\"; done"
  tes "for i in 1 2; do for j in a b; do echo \"\$i\$j\"; done; done"
  echo "========== FOR END =========="
}

test_while_loops() {
  echo "========== WHILE LOOP BEGIN =========="
  tes "i=1; while [ \$i -le 5 ]; do echo \"Number: \$i\"; i=\$((i + 1)); done"
  tes "while read -r line; do echo \"Line: \$line\"; done < <(echo -e \"first\\nsecond\\nthird\")"
  tes "i=0; while [ \$i -lt 3 ]; do echo \"Iteration \$i\"; i=\$((i + 1)); done"
  tes "i=1; while [ \$i -le 3 ]; do [ \$i -eq 2 ] && false || echo \"Step \$i\"; i=\$((i + 1)); done"
  tes "i=1; while [ \$i -le 2 ]; do j=1; while [ \$j -le 2 ]; do echo \"\$i\$j\"; j=\$((j + 1)); done; i=\$((i + 1)); done"
  echo "========== WHILE LOOP END =========="
}
test_var() {
  echo "========== VARIABLES BEGIN =========="
  for i in $(find tests/step2/assignement_substitution -name "*sh"); do
    test_from_file $i
    test_from_stdin $i
  done
  echo "========== VARIABLES BEGIN =========="
}
test_non_builtin() {
  echo "========== NON_BUILTIN BEGIN =========="
  tes "ls -a; ls; ls"
  tes "tree -L 2"
  tes "find -name *.c"
  echo "========== NON_BUILTIN END =========="
}
test_quoting() {
  echo "========== QUOTING BEGIN =========="
  for i in $(find tests/step2/quoting -name "*sh"); do
    test_from_file $i
    test_from_stdin $i
  done
  echo "========== QUOTING END =========="
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
  tes "cat $DUMMY I_DO_NOT_EXIST > $DUMMY 2>&1; cat $DUMMY"
  tes "ls brrrrrr . 2>&1"
  tes "ls &1<2 ls aaaa ."
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
  tes "for i in 1 2 3; do if [ \$((i % 2)) -eq 0 ]; then echo \"Even: \$i\" | tr a-z A-Z; else echo \"Odd: \$i\"; fi; done"
  tes "for i in 1 2; do j=1; while [ \$j -le \$i ]; do echo \"\$i,\$j\" >> output.txt; j=\$((j + 1)); done; done; cat output.txt; rm output.txt"
  tes "i=1; while [ \$i -le 3 ]; do for j in a b; do if [ \$i -eq 2 ] && [ \"\$j\" = \"b\" ]; then echo \"Special case: \$i\$j\"; else echo \"Regular case: \$i\$j\"; fi; done; i=\$((i + 1)); done"
  tes "i=1; while [ \$i -le 3 ]; do case \$i in 1) echo \"First\" ;; 2) echo \"Second\" ;; 3) echo \"Third\" ;; esac; i=\$((i + 1)); done"
  tes "for word in \$(echo \"one two three\" | tr ' ' '\n'); do echo \"Word: \$word\" | rev; done"
  tes "for i in 1 2; do echo \"Start \$i\"; while [ \$i -le 2 ]; do echo \"Inside \$i\"; i=\$((i + 1)); done; echo \"End \$i\"; done"
  tes "for i in 1 2; do while [ \$i -le 2 ]; do if [ \$i -eq 2 ]; then false || echo \"Error on \$i\"; else echo \"OK: \$i\"; fi; break; done; done"
  tes "for i in 1 2; do j=1; while [ \$j -le 2 ]; do if [ \$((i * j)) -gt 2 ]; then echo \"\$i * \$j = \$((i * j))\" >> output.txt; else echo \"Below threshold: \$i \$j\"; fi; j=\$((j + 1)); done; done; cat output.txt; rm output.txt"
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
  # LEXER ERRS
  test_code_error 2 "if true; then echo a; \"fi"
  test_code_error 2 "\""
  test_code_error 2 "\"\"\""
  test_code_error 2 "for test"
  test_code_error 2 "while test"
  echo "========== ERROR_CODE END =========="
}
testsuite() {
  test_pipeline
  test_neg_pipeline
  test_ops
  test_var
  test_comment
  test_for
  test_while_loops
  test_mix_grammar
  test_quoting
  test_redirections
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
