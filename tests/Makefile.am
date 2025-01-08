TESTS = token_tests
bin_PROGRAMS = token_tests
token_tests_SOURCES = token_tests.c

token_tests_CPPFLAGS = -I$(top_srcdir)/src

token_tests_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

token_tests_LDADD = \
	$(top_builddir)/src/lexer/liblex.a \
	$(top_builddir)/src/utils/libutils.a \
	$(top_builddir)/src/ast/libast.a \
	$(top_builddir)/src/parser/libpar.a \
	$(top_builddir)/src/mbtstr/libmbtstr.a \
	$(top_builddir)/src/streams/libstreams.a \
	$(top_builddir)/src/builtins/libbuiltins.a

token_tests_LDFLAGS = \
	-L$(top_builddir)/src/utils \
	-L$(top_builddir)/src/ast \
	-L$(top_builddir)/src/parser \
	-L$(top_builddir)/src/mbtstr \
	-L$(top_builddir)/src/streams \
	-L$(top_builddir)/src/lexer \
	-L$(top_builddir)/src/builtins
