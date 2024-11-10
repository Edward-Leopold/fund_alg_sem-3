#include "strings.h"

String* create_string(char* str){
    String* string = (String*)malloc(sizeof(String));
    if (!string) return NULL;

    string->len = (int)strlen(str);
    string->text = (char*)malloc(sizeof(char) * (string->len + 1));
    if(!string->text){
        free(string);
        return NULL;
    }

    for (int i = 0; i < string->len; i++) string->text[i] = str[i];
    string->text[string->len] = '\0';

    return string;
}

void clear_string(String* string){
    if (string){
        free(string->text);
        string->len = 0;
    }
}

void delete_string(String* string){
    if (string){
        clear_string(string);
        free(string);
        string = NULL;
    }
}

int compare_strings(const String* s1, const String* s2){
    if(s1->len < s2->len) return 1;
    else if(s1->len > s2->len) return -1;

    for (int i = 0; i < s1->len; i++){
        if(s1->text[i] > s2->text[i]) return 1;
        else if(s1->text[i] < s2->text[i]) return -1;
    }

    return 0;
}

int equals_strings(const String* s1, const String* s2){
    if ((s1->len - s2->len) != 0) return 0;

    for (int i = 0; i < s1->len; i++){
        if(s1->text[i] != s2->text[i]) return 0;
    }

    return 1;
}

int copy_stings(String* s1, const String *s2){
    char* temp = (char*)realloc(s1->text, sizeof(char) * (s2->len + 1));
    if (!temp){
        return 0;
    }
    s1->text = temp;
    s1->len = s2->len;
    for (int i = 0; i < s2->len; i++){
        s1->text[i] = s2->text[i];
    }
    s1->text[s1->len] = '\0';

    return 1;
}

String* copy_new_string(const String* s){
    String* str = (String*)malloc(sizeof(String));
    if (!str) return NULL;

    str->len = s->len;
    str->text = (char*)malloc(sizeof(char) * (str->len + 1));
    if(!str->text){
        free(str);
        return NULL;
    }

    for (int i = 0; i < str->len; i++) str->text[i] = s->text[i];
    str->text[str->len] = '\0';

    return str;
}

int concat_strings(String* s1, const String* s2){
    char* temp = (char*)realloc(s1->text, sizeof(char) * (s1->len + s2->len + 1));
    if (!temp) return 0;

    s1->text = temp;
    for (int i = 0; i < s2->len; i++) s1->text[i + s1->len] = s2->text[i];
    s1->text[s1->len + s2->len] = '\0';
    s1->len += s2->len;

    return 1;

}
