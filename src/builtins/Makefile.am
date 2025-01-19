lib_LIBRARIES = libbuiltins.a

libbuiltins_a_SOURCES =  \
  echo.c \
  builtins.h \
true_and_false_builtin.c \
run_command.c \
run_command.h \
commands.c \
exit.c \
cd.c \
export.c \
unset.c \
commands.h 

libbuiltins_a_CPPFLAGS = -I$(top_srcdir)/src

libbuiltins_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -g -O0 

noinst_LIBRARIES = libbuiltins.a
