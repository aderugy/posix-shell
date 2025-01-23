#include "naming.h"

#include <ctype.h>
#include <string.h>

#include "lexer/splitter.h"

static char *keywords[] = { "then",
                            "elif",
                            "if",
                            "fi",
                            "else",
                            "do",
                            "for",
                            "done",
                            "while",
                            "until",
                            /*"{",    "}",  */ "(",
                            ")",
                            NULL };

bool is_keyword(char *word)
{
    for (size_t i = 0; keywords[i]; i++)
    {
        if (strcmp(keywords[i], word) == 0)
        {
            return true;
        }
    }
    return false;
}

int XDB_valid(char *name)
{
    if (isdigit(*name))
    {
        return 0;
    }

    while (*name && (isalnum(*name) || *name == '_'))
    {
        ++name;
    }

    return *name == 0;
}
