lib_LIBRARIES = libutils.a

libutils_a_SOURCES =  \
  logger.c \
  logger.h \
  fill_input.h \
  fill_input.c \
  test_lexer.h \
  test_lexer.c

libutils_a_CPPFLAGS = -I$(top_srcdir)/src

libutils_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic


noinst_LIBRARIES = libutils.a
