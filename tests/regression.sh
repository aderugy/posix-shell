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
#BIN="42sh"
F=$BIN_PATH
# for colors
#G="\033[0;32m"
#R="\033[0;31m"
#D="\033[0m"
G=""
R=""
D=""

TOTAL_TEST=0
PASSED_TEST=0
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
  output_test "./$BIN < $@"
  OUTPUT_CODE_RETURN=$?
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
# @params: err code and then a list of strings
test_pars_lex_error() {
  TOTAL_TEST=$((TOTAL_TEST + 1))
  ERR="$1"
  shift
  "$F" -c "$@"
  ACTUAL_ERR="$?"
  if [ $ACTUAL_ERR -eq $ERR ]; then
    echo "$G[OK]$D $F -c \""$@"\"$D"
    PASSED_TEST=$((PASSED_TEST + 1))
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
      echo "expected"
      E=$(cat -e "$EXPECTED_ERR")
      echo "actual"
      A=$(cat -e "$ACTUAL_ERR")
      echo "$G$E$D"
      echo "$R$A$D"
    fi
  else
    echo "$R[KO]$D"
    echo "$FA \""$@"\""

    echo "expected"
    E=$(cat -e "$EXPECTED_OUT")

    echo "actual"
    A=$(cat -e "$ACTUAL_OUT")
    echo "$G$E$D"
    echo "$R$A$D"
  fi
}

for_coverage() {
    "$F" -c "fun () { ls; echo a;}; fun && echo B || echo C"
    "$F" aaaaa
    "$F" -vc "echo a"
    "$F" -tc "echo a"
    "$F" -sc "echo a"
}

test_echo_basic() {
  echo "========== ECHO BEGIN =========="
  tes "echo     "
  tes 'echo     a'
  tes echo 'a'
  tes 'echo "a"'
  tes echo aaa bbb ccc
  tes echo echo
  tes echo "ouaaaaala dinguerie\n"
  tes 'echo' 'a'
  tes "echo foo; echo 'a'"
  tes "echo foo echo 'a'"
  tes "echo hello!"
  tes "echo hello!hello"
  tes "echo !heelo"
  tes "echo !heelo; echo yooo!o!!o!o!!!o"
  tes "echo -e !heelo;echo yooo!o!!o!o!!!o"
  tes "echo -n !heelo;echo yooo!o!!o!o!!!o"
  tes "echo -E !heelo;echo yooo!o!!o!o!!!o"
  tes "echo jepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbro"
  tes "echo j\epeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbro"
  tes "echo -e j\\epeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbro"
  tes "echo -E j\\epeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbro"
  tes "echo -n j\\e\peuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbrojepeuxecrirecombiendecharbro"
  echo "========== ECHO END =========="
}
test_non_builtin() {
  echo "========== NON_BUILTIN BEGIN =========="
  tes "ls -a"
  tes "ls -a; ls; ls"
  tes "tree -L 2"
  tes "find -name '*.c'"
  echo "========== NON_BUILTIN END =========="
}
test_if() {
  echo "========== IF BEGIN =========="
  tes "if true; then echo a; fi"
  tes "if false; then echo a; fi"
  tes "if false; then echo a; echo b; fi"
  tes "if true; then if true; then if true; then echo a; fi fi fi"
  echo "========== IF END =========="
}
test_elif() {
  echo "========== ELIF BEGIN =========="
  tes "if false; then echo a; elif true; then echo b; fi"
  tes "if false; then echo a; elif false; then echo b;
                        elif true; then echo c; fi"
  echo "========== ELIF END =========="
}
test_else() {
  echo "========== ELSE BEGIN =========="
  tes "if false; then echo a; else echo b; fi"
  tes "if true; then echo a; else echo b; fi"
  tes "if false; then echo a; elif false; then echo b; else echo c; fi"
  tes "if false; then echo a; elif false; then echo b; else echo c; fi"
  echo "========== ELSE END =========="
}
test_comment() {
  echo "========== COMMENT BEGIN =========="
  tes echo "Ya un commentaire m c chill #de ouf c chill"
  tes echo "echo #yo"
  tes echo "#yo"
  tes echo "Ya un commentaire m c chill # mais moi je suis pas un comment"
  tes echo "Ya un autre commentaire m c chill # mais moi je suis pas comment"
  tes echo "Ya un commentaire m c chill # mais moi je suis pas un comment"
  tes '# echo'
  tes 'echo \escaped \#escaped "#"quoted not#first #commented'
  tes echo "Ya un commentaire m c chill \# mais moi je suis pas un comment"
  tes echo "Ya un commentaire m c chill #de ouf c chill"
  echo "========== COMMENT END =========="
}

test_pipeline() {
  echo "========== PIPELINE BEGIN =========="
  tes "echo Hello | tr a e"
  tes "echo Hello | tr a e | tr e a"
  tes "echo Hello | tr a e | tr e a | tr a e"
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

test_redirections() {
  echo "========== REDIRECTION BEGIN =========="
  tes "cat non_existent_file 2> err.txt; cat err.txt; rm err.txt"
  tes "echo 'This will not appear' > /dev/null"
  tes "ls invalid_file 2> /dev/null"
  tes "ls > text ; sort < text ; cat text;rm text"
  tes "echo tchou > dum.out;echo bebe >> dum.out; cat dum.out; rm dum.out"
  tes "echo Salut >| text; cat text;rm text"
  tes "cat non_existent_file 2> err.txt; cat err.txt"
  tes "echo 'This will not appear' > /dev/null"
  tes "ls invalid_file 2> /dev/null"
  tes "ls > text ; sort < text ; cat text;rm text"
  tes "ls 1> dum.out; cat dum.out"
  tes "echo 'Hello, World!' > file1; cat file1"
  tes "echo 'Line 1' > file1; echo 'Line 2' >> file1; cat file1;"
  tes "cat < file1"
  tes "cat < file1 > out.txt 2>&1; cat out.txt"
  tes "ls | echo > dum.out; echo < dum.out"
  tes "echo tchou > dum.out;echo bebe >> dum.out; cat dum.out; rm dum.out"
  tes "echo tchou > dum.out;echo boubou > dum.out; cat dum.out; rm dum.out"
  tes "echo aads 1<&2 cat coverage.sh"
  tes "echo aads 1>&2 cat coverage.sh"

  touch dum.out
  rm dum.out
  touch file1
  rm file1
  touch err.txt
  rm err.txt
  touch out.txt
  rm out.txt
  echo "========== REDIRECTION END =========="

}
test_quoting() {
  echo "========== QUOTING BEGIN =========="
  for i in $(find step2/quoting -name "*sh"); do
    test_from_file $i
    test_from_stdin $i
  done
  tes "echo Hello World!"
  tes "'echo' hello 'my' dream"
  tes "'ls'"
  echo "========== QUOTING END =========="
}
test_cd() {
  echo "========== CD ========="
  tes 'cd && echo $PWD'
  echo "========== CD END ========="
}
test_exit() {
  echo "========== exit ========="
  test_code_error 1 "exit 1"
  echo "========== exit END ========="
}
test_var_local() {
  echo "========== ASSIGNMENT BEGIN =========="
  tes "A=2; echo $A"
  tes "A=42; B=55; echo $A $B"
  tes "A=42; A=55; A=58; A=59; A=kjhgfrtyj; echo $A"
  tes "A=42; B=$A; echo $A$B$C"
  echo "========== ASSIGNMENT END =========="
}
test_errs() {
  echo "========== ERROR_CODE BEGIN =========="
  # PARSER ERRS
  test_pars_lex_error 2 "fi fi"
  test_pars_lex_error 2 "then fi"
  test_pars_lex_error 2 "if fi"
  test_pars_lex_error 2 "else fi"
  test_pars_lex_error 2 "if true; then if fi"
  test_pars_lex_error 2 "if true; then if fi"
  test_pars_lex_error 2 "elif true; then if fi"
  test_pars_lex_error 2 "if true; then if fi"
  test_pars_lex_error 2 "if if; then if fi"
  test_pars_lex_error 2 "then true; then if fi"
  test_pars_lex_error 2 "true; then if fi"
  # LEXER ERRS
  test_pars_lex_error 2 "if true; then echo a; \"fi"
  test_pars_lex_error 2 "\""
  test_pars_lex_error 2 "cd a a"
  echo "========== ERROR_CODE END =========="
}
test_while() {
  echo "========= WHILE BEGIN ============="
  for i in $(find step2/loops -name "while*sh"); do
    test_from_file $i
    test_from_stdin $i
  done
  echo "========= WHILE END ============="
}
test_until() {
  echo "========= UNTIL BEGIN ============="
  for i in $(find step2/loops -name "until*sh"); do
    test_from_file $i
    test_from_stdin $i
  done
  echo "========= WHILE END ============="
}
test_for() {
  echo "========= FOR LOOP BEGIN =========="
  tes 'for i in a b; do echo $i; done'
  tes 'for i in ls; do echo $i; done'
  echo "========= FOR LOOP END =========="
}
test_export() {
  echo "========== EXPORT BEGIN ==="
  tes 'export ABC=5; echo $ABC;'
  echo "========== EXPORT END ====="
}
test_unset() {
  echo "========== UNSET BEGIN ==="
  tes 'export ABCD=5; echo $ABCD; unset ABCD;'
  tes 'a=99; echo $a; export a; unset a; echo $a'
  echo "========== UNSET END ====="
}
test_blocks() {
  echo "========== BLOCKS BEGIN ==="
  tes '{ echo a; echo b; } | tr b h'
  tes '{ echo c; { echo a; echo b; } ; } | tr b h'
  tes '{ echo a;} | tr a h'
  tes '{ { { { { { { echo a; } } } } } } } | tr a h'
  echo "========== BLOCKS END ====="
}
test_var() {
    echo "========== VARIABLES BEGIN =========="
    for i in $(find step2/assignement_substitution -name "*sh"); do
        test_from_file $i
        test_from_stdin $i
    done
    echo "========== VARIABLES END =========="
}
test_function() {
  echo "========== FUNCTIONS BEGIN ==="
  tes 'fun ( ) { echo a; }; fun'
  tes 'ls ( ) { echo a; }; ls'
  tes 'fun ( ) { echo a; }; fun'
  tes 'fun ( ) { echo a; }; fun; fun ( ) { echo b; }; fun'
  tes 'fun ( ) { foo ( ) { echo b; }; echo a; foo; }; fun'
  tes 'fun ( ) { foo ( ) { ls; }; echo a; foo; }; ls fun fun'
  tes 'fun(       )      {    foo( ) { ls; }; echo a; foo; }; ls fun fun'
  tes 'fun() { foo() { ls; }; echo a; foo; }; ls fun fun'
  tes 'A=1; echo $A; fun ( ) { A=2; echo $A; }; fun ;echo $A'
  echo "========== FUNCTIONS END ====="
}
testsuite() {
  test_echo_basic
  test_non_builtin
  test_if
  test_elif
  test_else
  test_comment
  test_ops
  test_neg_pipeline
  test_pipeline
  test_redirections
  test_quoting
  test_errs
  test_cd
  test_exit
  test_var_local
  test_var
  test_quoting
  test_for
  test_while
  test_until
  test_export
  test_unset
  test_blocks
  test_function
  for_coverage
}

if [ "$COVERAGE" = "yes" ]; then
  #./step1.sh
  ./step2/step2.sh
  ./step3/step3.sh
  testsuite
  echo -e "TEST : $TOTAL_TEST\nPASSED TEST : $PASSED_TEST\n"
  res=$((100 * $PASSED_TEST / $TOTAL_TEST))
  echo "REGRESSION: $res% PASSED"
else
  testsuite
  res=$((100 * $PASSED_TEST / $TOTAL_TEST))
  echo -e "$res" >"$OUTPUT_FILE"
  echo "$res%"
fi

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
