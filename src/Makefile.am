SUBDIRS = ast \
		  streams \
		  mbtstr \
		  lexer \
          utils \
          builtins
bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -g -O0 -fsanitize=address -fsanitize=address

42sh_LDADD =  \
	ast/libast.a \
	lexer/liblex.a \
	utils/libutils.a \
	streams/libstreams.a \
	builtins/libbuiltins.a \
	mbtstr/libmbtstr.a 
