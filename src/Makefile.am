SUBDIRS = ast \
		  parser \
		  streams \
		  lexer \
		  mbtstr \
		  utils
bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD =  \
	utils/libutils.a \
	ast/libast.a \
	parser/libpar.a \
	mbtstr/libmbtstr.a \
	streams/libstreams.a \
	lexer/liblex.a 
