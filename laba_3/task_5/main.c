#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <linux/limits.h>

typedef enum errCodes{
    MALLOC_ERR,
    REALLOC_ERR,
    SAME_FILE_ERR,
    IVNALID_NUM_ARGUEMENTS,
    UNABLE_TO_OPEN_FILE,
    INCORRECT_INPUT,
    
    SUCCESS
} errCodes;

typedef struct Student{
    unsigned int id;
    char *name;
    char *surname;
    char *group;
    unsigned char *grades;
} Student;

typedef struct {
    Student *students;
    int size;
} StudentArray;

void clear_student(Student *student) {
    if (student) {
        free(student->name);
        free(student->surname);
        free(student->group);
        free(student->grades);
    }
}

void clear_students_arr(StudentArray *student_array) {
    if (student_array && student_array->students) {
        for (int i = 0; i < student_array->size; i++) {
            clear_student(&student_array->students[i]);
        }
        free(student_array->students);
        student_array->students = NULL;
        student_array->size = 0;
    }
}

errCodes is_same_file(const char *a, const char *b){
    errCodes res = SUCCESS;
    char input_path[PATH_MAX];
    char output_path[PATH_MAX];
    realpath(a, input_path);
    realpath(b, output_path);
    int is_not_same = 0;
    
    for (int i = 0; (input_path[i] && output_path[i]); i++) {       
        if (input_path[i] != output_path[i]) is_not_same = 1;
    }   
    if (!is_not_same) res = SAME_FILE_ERR;

    return res;
}

errCodes getArgs(int argc, char** argv, char** filename_in, char** filename_out){
    if (argc != 3) return IVNALID_NUM_ARGUEMENTS;

    errCodes err_status = is_same_file(argv[1], argv[2]);
    if (err_status != SUCCESS) return err_status;

    *filename_in = argv[1];
    *filename_out = argv[2]; 

    return SUCCESS;
}

int is_latin_str(const char *str){
    int len = strlen(str);
    for(int i = 0; i < len; i++) {
        if (!isalpha(str[i])) return 0;
    }
    return 1;
}

errCodes get_students(const char* filename_in, StudentArray *result){
    FILE* input = fopen(filename_in, "r");
    if (!input) return UNABLE_TO_OPEN_FILE;

    int mem_size = 10;
    StudentArray students;
    students.size = 0;
    students.students = (Student *)malloc(sizeof(Student) * (mem_size + 1));
    if (!students.students) {
        fclose(input);
        return MALLOC_ERR;
    }

    const int size_of_line = 100000;
    char str[size_of_line];
    while ((fgets(str, size_of_line, input)) != NULL){
        int is_empty = 1;
        for (int i = 0; str[i]; i++) {
            if (!isspace(str[i])) {
                is_empty = 0;
                break;
            }
        }
        if (is_empty) continue;

        if(students.size == mem_size){
            mem_size *= 2;
            Student *temp = (Student *)realloc(students.students, sizeof(Student) * (mem_size + 1));
            if(!temp){
                fclose(input);
                free(students.students);
                return REALLOC_ERR;
            }
            students.students = temp;
        }

        int field_size = 100;
        int id; 
        char name[field_size], surname[field_size], group[field_size];
        int grades[5];

        int scan_status = sscanf(str, "%d %99s %99s %99s %d %d %d %d %d", &id, name, surname, group, &grades[0], &grades[1], &grades[2], &grades[3], &grades[4]);
        if (scan_status != 9){
            continue;
        }
        if(id < 0 || !is_latin_str(name) || !is_latin_str(surname)) {
            continue;
        }
        for (int i = 0; i < 5; i++) {
            if (grades[i] > sizeof(unsigned char) || grades[i] < 0) continue;
        } 
        for (int i = 0; i < students.size; i++){
            if (id == students.students->id) continue;
        }
        
        Student stud;
        stud.name = malloc(100 * sizeof(char));
        stud.surname = malloc(100 * sizeof(char));
        stud.group = malloc(100 * sizeof(char));
        stud.grades = malloc(5 * sizeof(unsigned char));
        if (!stud.name || !stud.surname || !stud.group || !stud.grades) {
            if (stud.grades) free(stud.grades);
            if (stud.name) free(stud.name);
            if (stud.surname) free(stud.surname);
            if (stud.group) free(stud.group);
            free(students.students);
            fclose(input);
            return MALLOC_ERR;
        }

        stud.id = id;
        strcpy(stud.name, name);
        strcpy(stud.surname, surname);
        strcpy(stud.name, name);
        for (int i = 0; i < 5; i++) {
            stud.grades[i] = (unsigned char)grades[i];
        }

        students.students[students.size] = stud;
        students.size++; 
    }

    *result = students;

    fclose(input);
    return SUCCESS;
}

int main(int argc, char** argv) {
    char *filename_in, *filename_out;

    errCodes args_status = getArgs(argc, argv, &filename_in, &filename_out);
    if (args_status != SUCCESS){
        switch (args_status){
        case IVNALID_NUM_ARGUEMENTS:
            printf("Invalid number of arguements, must be 2\n");
            break;
        case SAME_FILE_ERR:
            printf("Same file for input and output\n");
            break;
        default:
            break;
        }
        return 1;
    }
    
    StudentArray students;
    errCodes get_students_status = get_students(filename_in, &students);
    if (get_students_status != SUCCESS){
        switch (get_students_status){
        case MALLOC_ERR:
            printf("malloc error occured while reading students from file\n");
            break;
        case REALLOC_ERR:
            printf("realloc error occured while reading students from file\n");
            break;
        case UNABLE_TO_OPEN_FILE:
            printf("can't open file %s\n", filename_in);
            break;
        default:
            break;
        }
        return 1;
    }

    FILE* output = fopen(filename_out, "w");
    if (!output){
        printf("Can't open output file %s\n", filename_out);
        clear_students_arr(&students);
        return 1;
    }

    while(1){
        printf("\nMenu:\n");
        printf("1: Search student by id\n");
        printf("3: Search student by surname\n");
        printf("2: Search student by name\n");
        printf("4: Search student by group\n");
        printf("5: Sort students by id\n");
        printf("6: Sort students by surname\n");
        printf("7: Sort students by name\n");
        printf("8: Sort students by group\n");
        printf("9: Print students with grades above average\n");
        printf("0: Exit\n");
        printf("Enter command number: ");

        int command;
        if (scanf("%d", &command) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (command){
        case 0:
            fclose(output);
            clear_students_arr(&students);
            return 0;
        case 1:
            printf("some command");
            break;
        default:
            printf("Invalid command.\n");
            break;
        }
    }

    clear_students_arr(&students);
    return 0;
}