lib_LIBRARIES = libbuiltins.a

libbuiltins_a_SOURCES =  \
  echo.c \
  builtins.h \
true_and_false_builtin.c 

libbuiltins_a_CPPFLAGS = -I$(top_srcdir)/src

libbuiltins_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libbuiltins.a
