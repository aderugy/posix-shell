lib_LIBRARIES = libbuiltins.a

libbuiltins_a_SOURCES =  \
  builtins.h \
  echo.c \
  continue.c \
  break.c \
  true_and_false_builtin.c \
  run_command.c \
  run_command.h \
  commands.h  \
  commands.c \
  exit.c \
  cd.c \
  export.c \
  unset.c \
  dot.c 

libbuiltins_a_CPPFLAGS = -I$(top_srcdir)/src

libbuiltins_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -g -O0 

noinst_LIBRARIES = libbuiltins.a
