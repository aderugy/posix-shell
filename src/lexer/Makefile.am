lib_LIBRARIES = liblex.a

liblex_a_SOURCES =  \
  lexer.c \
  splitter.c \
  splitter.h \
  lexer.h \
  token.h 

liblex_a_CPPFLAGS = -I$(top_srcdir)/src

liblex_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -g -O0 

noinst_LIBRARIES = liblex.a
