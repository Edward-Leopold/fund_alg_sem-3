#include <stdio.h>
#include "strings.h"


int main(){
    String *s = create_string("Hello, world!");
    printf("%s\n", s->text);
    delete_string(s);

    return 0;
}