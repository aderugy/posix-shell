lib_LIBRARIES = libstreams.a

libstreams_a_SOURCES =  \
  streams.c \
  streams.h

libstreams_a_CPPFLAGS = -I$(top_srcdir)/src

libstreams_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -O0 -g -fsanitize=address

noinst_LIBRARIES = libstreams.a

