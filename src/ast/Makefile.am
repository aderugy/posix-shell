lib_LIBRARIES = libast.a

libast_a_SOURCES =  \
  ast.h \
  nodes/eval_ctx.h \
  nodes/eval_ctx.c \
  expansion/expansion.h \
  expansion/expansion.c \
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
  nodes/pipeline.c \
  nodes/pipeline.h \
  nodes/clist.c \
  nodes/clist.h \
  nodes/shell_command.c \
  nodes/shell_command.h \
  nodes/simple_command.c \
  nodes/simple_command.h \
  nodes/ionumber.c \
  nodes/ionumber.h \
  nodes/redirection.c \
  nodes/redirection.h \
  nodes/prefix.c \
  nodes/prefix.h \
  nodes/else.c \
  nodes/else.h \
  nodes/pipeline.c \
  nodes/pipeline.h \
  nodes/while.c \
  nodes/while.h \
  nodes/until.c \
  nodes/until.h \
  nodes/for.c \
  nodes/for.h \
  nodes/node.h

libast_a_CPPFLAGS = -I$(top_srcdir)/src

libast_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -g -O0

noinst_LIBRARIES = libast.a
