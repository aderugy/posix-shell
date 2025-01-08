# Init the 42sh project
AC_INIT([42sh], [1.0], [42sh@assistants.epita.fr])

# Setup Automake
AM_INIT_AUTOMAKE([subdir-objects] [foreign])

# Setup the test dir
AC_CONFIG_TESTDIR([tests])

# Pretty display of Makefile rules
AM_SILENT_RULES([yes])

# Enable ar for Makefile
AM_PROG_AR

# Check if ranlib is available
AC_PROG_RANLIB

# Check if a C compiler is available
AC_PROG_CC

# Check if a compiler has this list of flags
AX_COMPILER_FLAGS([],[],[],[-Wall -Wextra -Werror -Wvla -pedantic -std=c99 -g -O1])

# List Makefiles in subdirectories
AC_CONFIG_FILES([
                    Makefile
                    src/Makefile
                    src/ast/Makefile
                    src/streams/Makefile
                    src/mbtstr/Makefile
                    src/lexer/Makefile
                    src/parser/Makefile
                    src/utils/Makefile
                    src/builtins/Makefile
                    src/eval/Makefile
                    tests/Makefile
                ])
AC_OUTPUT
