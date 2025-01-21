#ifndef NAMING_H
#define NAMING_H

#include <stdbool.h>

bool is_keyword(char *data);
int convention_check(char *name, int len);
int dollar_valid(int state);
int regular(int c);
int XDB_valid(char *name);

#endif // !NAMING_H
