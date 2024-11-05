#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    SUCCESS
} errors;

typedef struct Student{
    unsigned int id;
    char *first_name;
    char *last_name;
    char *group;
    unsigned char *grades;
} Student;

typedef struct {
    Student *students;
    int size;
} StudentArray;

int main(int argc, char** argv) {

    

    return 0;
}