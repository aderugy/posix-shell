# uncomment for the autotools testsuite
#TESTS = tests/testsuite.sh tests/syntax/good/tests.sh tests/syntax/bad/tests.sh

LOG_COMPILER = /bin/bash

SUBDIRS = src \
		  tests
#uncommenting the line above creates an infinite loop before the testsuite runs
#check: tests/testsuite.sh
#	tests/testsuite.sh

#check:
#	tests/regression.sh
s1:
	tests/step1.sh
s2:
	tests/step2/step2.sh
s3:
	tests/step3/step3.sh
checkall:
	tests/regression.sh
	tests/step1.sh
	tests/step2/step2.sh

clean:
	rm -rf compile configure config.log ar-lib depcomp install-sh missing 
	rm -rf configure~ *.out filemake
	rm -rf *.m4 autom4te.cache/ ~configure
	rm -rf config.status Makefile src/42sh
	rm -rf test-driver tests/atconfig tests/token_tests
	find \( -name *.log -o -name *.trs -o -name *.Po -o -name Makefile \) -delete
	find \( -name *.dirstamp \) -delete
	find . -type f \( -name "*.gcno" -o -name "*.gcda" \) -delete
	find . -type f -name "*.html" -delete
	rm -rf src/ast/.deps *.txt
	find \( -name *.deps -o -name *.[oa] -o -name *.in \) -delete
	rm 42sh-1.0.tar.gz
builtins:
	make
	./tests/builtt_tests
splitter:
	make
	./tests/splitter_tests
token:
	make
	./tests/token_tests

