lib_LIBRARIES = libmbtstr.a

libmbtstr_a_SOURCES = \
    mbt_cview_cmp.c \
    mbt_cview_contains.c \
    mbt_cview_fprint.c \
    mbt_str_ctor.c \
    mbt_str_dtor.c \
    mbt_str_free.c \
    mbt_str_init.c \
    mbt_str_pushc.c \
    mbt_str_pushcstr.c \
	mbt_str_pop.c \
	mbt_str_fill.c \
	mbt_str_merge.c \
	utils.h \
	str.h \
	view.h

libmbtstr_a_CPPFLAGS = -I$(top_srcdir)/src

libmbtstr_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -g -O0

noinst_LIBRARIES = libmbtstr.a
