#TESTS = tests/testsuite.sh tests/syntax/good/tests.sh tests/syntax/bad/tests.sh

LOG_COMPILER = /bin/bash

SUBDIRS = src \
		  tests

check:
	tests/testsuite.sh
clean:
	rm -rf compile configure config.log ar-lib depcomp install-sh missing 
	rm -rf *.m4 autom4te.cache/
	rm -rf config.status Makefile src/42sh
	rm -rf test-driver tests/atconfig
	find \( -name *.log -o -name *.trs -o -name *.Po -o -name Makefile \) -delete
	find \( -name *.dirstamp \) -delete
	find \( -name *.deps -o -name *.[oa] -o -name *.in \) -delete
