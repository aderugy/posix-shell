SUBDIRS = ast \
		  streams \
		  mbtstr \
		  lexer \
				utils \
				builtins \
				eval
bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD =  \
    eval/libeval.a \
	ast/libast.a \
	lexer/liblex.a \
	utils/libutils.a \
	streams/libstreams.a \
	builtins/libbuiltins.a \
	mbtstr/libmbtstr.a 
