lib_LIBRARIES = libutils.a

libutils_a_SOURCES =  \
	eval_output_structure.h \
	eval_output_structure.c \
  logger.c \
  logger.h \
  fill_input.h \
  fill_input.c  \
  linked_list.c \
  linked_list.h \
  xalloc.c \
  xalloc.h \
  mergestr.c \
  mergestr.h \
  mypipe.c \
  mypipe.h \
  hash_map.h \
  hash_map.c \
  naming.h \
  naming.c \
  stack.c \
  stack.h \
  err_utils.h \
  my_itoa.h \
  my_itoa.c \
  my_strcat.c \
  my_strcat.h

libutils_a_CPPFLAGS = -I$(top_srcdir)/src

libutils_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -g -O0 


noinst_LIBRARIES = libutils.a
