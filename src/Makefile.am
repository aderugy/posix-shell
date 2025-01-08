SUBDIRS = ast \
		  parser \
		  streams \
		  mbtstr \
		  lexer \
				utils \
				builtins
bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD =  \
	parser/libpar.a \
	ast/libast.a \
	lexer/liblex.a \
	utils/libutils.a \
	streams/libstreams.a \
	builtins/libbuiltins.a \
	mbtstr/libmbtstr.a \
    eval/libeval.a 
