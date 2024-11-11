#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <linux/limits.h>

#define STR_FIELD_SIZE 100 

typedef enum errCodes{
    MALLOC_ERR,
    REALLOC_ERR,
    SAME_FILE_ERR,
    IVNALID_NUM_ARGUEMENTS,
    UNABLE_TO_OPEN_FILE,
    INCORRECT_INPUT,
    NO_SUCH_STUDENTS,
    CANT_SAVE_OUTPUT,

    SUCCESS
} errCodes;

typedef struct Student{
    unsigned int id;
    char *name;
    char *surname;
    char *group;
    unsigned char *grades;
} Student;

typedef struct StudentArray{
    Student *students;
    int size;
} StudentArray;

typedef int (*CompareFunc)(const Student *student, const char *value);

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

double calculate_average(const Student *student) {
    if (!student || !student->grades) return 0.0;
    double sum = 0.0;
    for (int i = 0; i < 5; i++) {
        sum += student->grades[i];
    }
    return sum / 5.0;
}

double calculate_total_average(const StudentArray *studs) {
    if (!studs || studs->size == 0) return 0.0;
    double total = 0.0;
    for (int i = 0; i < studs->size; i++) {
        total += calculate_average(&studs->students[i]);
    }
    return total / studs->size;
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

        int is_duplicate_id = 0;
        for (int i = 0; i < students.size; i++) {
            if (id == students.students[i].id) {
                is_duplicate_id = 1;
                break;
            }
        }

        if (is_duplicate_id) {
            continue; 
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
        strcpy(stud.group, group);
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

// search functions

Student *search_by_id(const StudentArray *students, unsigned int id) {
    for (int i = 0; i < students->size; i++) {
        if (students->students[i].id == id) {
            return &students->students[i];
        }
    }
    return NULL;
}


errCodes search_by_str_fied(const StudentArray *students, StudentArray *result, const char *value, CompareFunc cmp) {
    StudentArray res_students;
    int mem_size = 10;
    res_students.size = 0;
    res_students.students = (Student *)malloc(sizeof(Student) * (mem_size + 1));
    if (!res_students.students) return MALLOC_ERR;
    
    for (int i = 0; i < students->size; i++) {
        if (cmp(&students->students[i], value)) {
            if(res_students.size == mem_size){
                mem_size *= 2;
                Student *temp = (Student *)realloc(res_students.students, sizeof(Student) * (mem_size));
                if(!temp){
                    free(res_students.students);
                    return REALLOC_ERR;
                }
                res_students.students = temp;
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
                free(res_students.students);
                return MALLOC_ERR;
            }

            stud.id = students->students[i].id;
            strcpy(stud.name, students->students[i].name);
            strcpy(stud.surname, students->students[i].surname);
            strcpy(stud.group, students->students[i].group);
            for (int j = 0; j < 5; j++) {
                stud.grades[j] = (unsigned char)students->students[i].grades[j];
            }

            res_students.students[res_students.size] = stud;
            res_students.size++; 
        }
    }   

    if (res_students.size == 0) {
        free(res_students.students);
        return NO_SUCH_STUDENTS;
    }

    *result = res_students;
    return SUCCESS;
}

int compare_by_surname(const Student *student, const char *surname) {
    return strcmp(student->surname, surname) == 0;
}

int compare_by_name(const Student *student, const char *name) {
    return strcmp(student->name, name) == 0;
}

int compare_by_group(const Student *student, const char *group) {
    return strcmp(student->group, group) == 0;
}

errCodes search_by_surname(const StudentArray *students, StudentArray *result, const char *surname) {
    return search_by_str_fied(students, result, surname, compare_by_surname);
}

errCodes search_by_name(const StudentArray *students, StudentArray *result, const char *name) {
    return search_by_str_fied(students, result, name, compare_by_name);
}

errCodes search_by_group(const StudentArray *students, StudentArray *result, const char *group) {
    return search_by_str_fied(students, result, group, compare_by_group);
}

errCodes save_to_output(FILE *output, const Student *student) {
    if (!student || !output) return CANT_SAVE_OUTPUT;

    int print_status = fprintf(output, "id: %u, Full name: %s %s, Group: %s, Average Grade: %.2f\n", 
                                student->id, student->name, student->surname, student->group, calculate_average(student));
    if (print_status < 0) return CANT_SAVE_OUTPUT;
    return SUCCESS;
}

// comparators for qsort
int comparator_by_id(const void *a, const void *b) {
    const Student *student_a = (const Student *) a;
    const Student *student_b = (const Student *) b;
    if (student_a->id < student_b->id) return -1;
    if (student_a->id > student_b->id) return 1;
    return 0;
}

int comparator_by_name(const void *a, const void *b) {
    const Student *student_a = (const Student *) a;
    const Student *student_b = (const Student *) b;
    return strcmp(student_a->name, student_b->name);
}

int comparator_by_surname(const void *a, const void *b) {
    const Student *student_a = (const Student *) a;
    const Student *student_b = (const Student *) b;
    return strcmp(student_a->surname, student_b->surname);
}

int comparator_by_group(const void *a, const void *b) {
    const Student *student_a = (const Student *) a;
    const Student *student_b = (const Student *) b;
    return strcmp(student_a->group, student_b->group);
}

// above average list
void write_above_average_students(const StudentArray *stud_arr, FILE *output) {
    if (!output) {
        return;
    }

    double total_average = calculate_total_average(stud_arr);
    fprintf(output, "Students with above average grade (%.2f):\n", total_average);

    for (int i = 0; i < stud_arr->size; i++) {
        double student_average = calculate_average(&stud_arr->students[i]);
        if (student_average > total_average) {
            fprintf(output, "%s %s, Group: %s, Average Grade: %.2f\n",
                    stud_arr->students[i].name,
                    stud_arr->students[i].surname,
                    stud_arr->students[i].group,
                    student_average);
        }
    }
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

    if(students.size == 0) {
        clear_students_arr(&students);
        printf("No students in correct format in input file\n");
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
        printf("2: Search student by surname\n");
        printf("3: Search student by name\n");
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
            printf("Enter student id: ");
            unsigned int id;
            if (scanf("%u", &id) == 1) {
                Student *student = search_by_id(&students, id);
                if (student) {
                    printf("Found student: Full name %s %s, Group: %s, Average grade: %.2f\n",
                            student->name, student->surname,
                            student->group, calculate_average(student));

                    printf("Do you want to save this info to trace file? (1 - yes, 0 - no): ");
                    int save_choice;
                    if (scanf("%d", &save_choice) != 1) printf("Invalid input. Please enter a number.\n");
                    switch (save_choice) {
                        case 0:
                            printf("no\n");
                            break;
                        case 1:
                            printf("yes\n");
                            if (save_to_output(output, student) == SUCCESS){
                                printf("Information saved to trace file\n");
                            } else{
                                printf("An error during writing to trace file occured\n");
                            }
                            break;
                        default:
                            printf("no\n");
                            break;
                    }
                } else {
                    printf("Student with id %u not found.\n", id);
                }
            } else {
                printf("Invalid id format.\n");
                while (getchar() != '\n');
            }
            break;
        case 2:
            printf("Enter student's surname: ");
            char *surname = malloc(sizeof(char) * STR_FIELD_SIZE);
            if(!surname){
                printf("malloc error occured while getting your answer\n");
                break;
            }
            if(scanf("%99s", surname)){
                StudentArray studs;
                errCodes search_status = search_by_surname(&students, &studs, surname);
                if(search_status != SUCCESS){
                    switch (search_status){
                        case MALLOC_ERR:
                            printf("malloc error occured while searching students\n");
                            break;
                        case REALLOC_ERR:
                            printf("realloc error occured while searching students\n");
                            break;
                        case NO_SUCH_STUDENTS:
                            printf("No such students with surname %s\n", surname);
                            break;
                        default:
                            break;
                    }
                } else{
                    for(int i = 0; i < studs.size; i++){
                        printf("Student %s %s, id: %d, group: %s\n", studs.students[i].name, studs.students[i].surname, studs.students[i].id, studs.students[i].group);
                    }
                    clear_students_arr(&studs);
                }
            }else{
                printf("Invalid surname format\n");
                while (getchar() != '\n');
            }
            free(surname);
            break;
        case 3:
            printf("Enter student's name: ");
            char *name = malloc(sizeof(char) * STR_FIELD_SIZE);
            if(!name){
                printf("malloc error occured while getting your answer\n");
                break;
            }
            if(scanf("%99s", name)){
                StudentArray studs;
                errCodes search_status = search_by_name(&students, &studs, name);
                if(search_status != SUCCESS){
                    switch (search_status){
                        case MALLOC_ERR:
                            printf("malloc error occured while searching students\n");
                            break;
                        case REALLOC_ERR:
                            printf("realloc error occured while searching students\n");
                            break;
                        case NO_SUCH_STUDENTS:
                            printf("No such students with name %s\n", name);
                            break;
                        default:
                            break;
                    }
                } else{
                    for(int i = 0; i < studs.size; i++){
                        printf("Student %s %s, id: %d, group: %s\n", studs.students[i].name, studs.students[i].surname, studs.students[i].id, studs.students[i].group);
                    }
                    clear_students_arr(&studs);
                }
            }else{
                printf("Invalid name format\n");
                while (getchar() != '\n');
            }
            free(name);
            break;
        case 4:
            printf("Enter student's group: ");
            char *group = malloc(sizeof(char) * STR_FIELD_SIZE);
            if(!group){
                printf("malloc error occured while getting your answer\n");
                break;
            }
            if(scanf("%99s", group)){
                StudentArray studs;
                errCodes search_status = search_by_group(&students, &studs, group);
                if(search_status != SUCCESS){
                    switch (search_status){
                        case MALLOC_ERR:
                            printf("malloc error occured while searching students\n");
                            break;
                        case REALLOC_ERR:
                            printf("realloc error occured while searching students\n");
                            break;
                        case NO_SUCH_STUDENTS:
                            printf("No such students from group %s\n", group);
                            break;
                        default:
                            break;
                    }
                } else{
                    for(int i = 0; i < studs.size; i++){
                        printf("Student %s %s, id: %d, group: %s\n", studs.students[i].name, studs.students[i].surname, studs.students[i].id, studs.students[i].group);
                    }
                    clear_students_arr(&studs);
                }
            }else{
                printf("Invalid group format\n");
                while (getchar() != '\n');
            }
            free(group);
            break;
        case 5:
            qsort(students.students, students.size, sizeof(Student), comparator_by_id);
            printf("\nSorted student list by id:\n");
            for (int i = 0; i < students.size; i++) {
            printf("student id: %d, full name: %s %s, group: %s\n",
                    students.students[i].id,
                    students.students[i].name,
                    students.students[i].surname,
                    students.students[i].group);
            }
            break;
        case 6:
            qsort(students.students, students.size, sizeof(Student), comparator_by_surname);
            printf("\nSorted student list by surname:\n");
            for (int i = 0; i < students.size; i++) {
            printf("student id: %d, full name: %s %s, group: %s\n",
                    students.students[i].id,
                    students.students[i].name,
                    students.students[i].surname,
                    students.students[i].group);
            }
            break;
        case 7:
            qsort(students.students, students.size, sizeof(Student), comparator_by_name);
            printf("\nSorted student list by name:\n");
            for (int i = 0; i < students.size; i++) {
            printf("student id: %d, full name: %s %s, group: %s\n",
                    students.students[i].id,
                    students.students[i].name,
                    students.students[i].surname,
                    students.students[i].group);
            }
            break;
        case 8:
            qsort(students.students, students.size, sizeof(Student), comparator_by_group);
            printf("\nSorted student list by group:\n");
            for (int i = 0; i < students.size; i++) {
            printf("student id: %d, full name: %s %s, group: %s\n",
                    students.students[i].id,
                    students.students[i].name,
                    students.students[i].surname,
                    students.students[i].group);
            }
            break;
        case 9:
            printf("Would you like to save the output to a file as well? (1 = Yes, other number = No): ");
    
            int option;
            if (scanf("%d", &option) == 1) {
                if (option == 1) {
                    printf("yes\n");
                    write_above_average_students(&students, output);
                    printf("Info about students with above average grade will be added to trace file after exit\n");
                } else{
                    printf("no\n");
                }
                double total_average = calculate_total_average(&students);

                printf("\nStudents with above average grade (%.2f):\n", total_average);
                for (int i = 0; i < students.size; i++) {
                    double student_average = calculate_average(&students.students[i]);
                    if (student_average > total_average) {
                        printf("%s %s, Group: %s, Average Grade: %.2f\n",
                            students.students[i].name,
                            students.students[i].surname,
                            students.students[i].group,
                            student_average);
                    }
                }

                
            } else {
                printf("Invalid input.\n");
                while (getchar() != '\n'); 
            }
            break;
        default:
            printf("Invalid command.\n");
            break;
        }
    }

    fclose(output);
    clear_students_arr(&students);
    return 0;
}