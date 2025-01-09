lib_LIBRARIES = libutils.a

libutils_a_SOURCES =  \
  logger.c \
  logger.h \
  fill_input.h \
  fill_input.c  \
  linked_list.c \
  linked_list.h

libutils_a_CPPFLAGS = -I$(top_srcdir)/src

libutils_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic


noinst_LIBRARIES = libutils.a
