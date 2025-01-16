lib_LIBRARIES = libutils.a

libutils_a_SOURCES =  \
  logger.c \
  logger.h \
  fill_input.h \
  fill_input.c  \
  linked_list.c \
  linked_list.h \
	mypipe.c \
	mypipe.h \
	hash_map.h \
	hash_map.c \
	naming.h \
	naming.c \
	stack.c \
	stack.h \
	err_utils.h

libutils_a_CPPFLAGS = -I$(top_srcdir)/src

libutils_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -g -O0


noinst_LIBRARIES = libutils.a
