lib_LIBRARIES = libstr.a

libstr_a_SOURCES = \
    src/mbt_cview_cmp.c \
    src/mbt_cview_contains.c \
    src/mbt_cview_fprint.c \
    src/mbt_str_ctor.c \
    src/mbt_str_dtor.c \
    src/mbt_str_free.c \
    src/mbt_str_init.c \
    src/mbt_str_pushc.c \
    src/mbt_str_pushcstr.c \
    src/mbt_str_pushcv.c \
	src/utils.h \
	src/str.h \
	src/view.h

libstr_a_CPPFLAGS = -I$(top_srcdir)/src

libstr_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libstr.a
