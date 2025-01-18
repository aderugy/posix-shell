#include "my_itoa.h"

#include <stdio.h>

void str_revert(char str[])
{
    int len = 0;
    while (str[len] != 0)
    {
        len++;
    }
    len--;
    int i = 0;
    while (i < len)
    {
        char st = str[i];
        str[i] = str[len];
        str[len] = st;
        i++;
        len--;
    }
}

char *my_itoa(int value, char *s)
{
    if (value == 0)
    {
        s[0] = '0';
        s[1] = 0;
        return s;
    }
    if (value > 0)
    {
        int i = 0;
        while (value != 0)
        {
            s[i] = (value % 10 + '0');
            value = value / 10;
            i++;
        }
        s[i] = 0;
        str_revert(s);
        return s;
    }
    return s;
}
