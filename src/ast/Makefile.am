lib_LIBRARIES = libast.a

libast_a_SOURCES =  \
  ast.h \
  nodes/and_or.c \
  nodes/and_or.h \
  nodes/command.c \
  nodes/command.h \
  nodes/element.c \
  nodes/element.h \
  nodes/if.c \
  nodes/if.h \
  nodes/input.c \
  nodes/input.h \
  nodes/list.c \
  nodes/list.h \
  nodes/node.c \
  nodes/node.h \
  nodes/pipeline.c \
  nodes/pipeline.h \
  nodes/clist.c \
  nodes/clist.h \
  nodes/shell_command.c \
  nodes/shell_command.h \
  nodes/simple_command.c \
  nodes/simple_command.h
  

libast_a_CPPFLAGS = -I$(top_srcdir)/src

libast_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -g -fsanitize=address -O0

noinst_LIBRARIES = libast.a
