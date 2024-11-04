#include <stdio.h>
#include "strings.h"


int main(){
    String *s = create_string("Hello, world!");
    printf("%s\n", s->text);
    clear_string(s);
    free(s);

    return 0;
}