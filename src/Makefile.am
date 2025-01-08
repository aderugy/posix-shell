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
	lexer/liblex.a \
	utils/libutils.a \
	ast/libast.a \
	parser/libpar.a \
	streams/libstreams.a \
	builtins/libbuiltins.a \
	mbtstr/libmbtstr.a 
