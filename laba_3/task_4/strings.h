#ifndef MY_STRINGS_H
#define MY_STRINGS_H

#include <string.h>
#include <stdlib.h>

typedef struct String{
    char *text;
    int len;
}String;

String* create_string(char* str);
void clear_string(String* string);

int compare_strings(const String* s1, const String* s2);
int equals_strings(const String* s1, const String* s2);
int copy_stings(String* s1, const String *s2);
String* copy_new_string(const String* s);
int concat_strings(String* s1, const String* s2);

#endif