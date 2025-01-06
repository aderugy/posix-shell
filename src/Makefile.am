SUBDIRS = ast \
      parser \
      lexer
bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD =  \
       ast/libast.a \
       parser/libpar.a \
       lexer/liblex.a
