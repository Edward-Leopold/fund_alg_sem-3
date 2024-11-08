#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "strings.h"

#define STR_FIELD_SIZE 100 
#define MAX_LINE_LENGTH 10000

typedef enum errCodes{
    MALLOC_ERR,
    REALLOC_ERR,
    DATE_FORMAT_ERR,
    MAIL_NOT_FOUND,
    ADDRESS_INPUT_ERR,
    MAIL_ID_ERR,
    RESIDENT_NOT_FOUND,
    SUCCESS
}errCodes;

typedef struct Liver{
    String* surname;
    String* name;
    String* patronymic;
    String* birthday; // "dd:mm:yyyy"
    char gender; // 'M' or 'F'
    double salary;
} Liver;

typedef struct Node {
    Liver liver;
    struct Node* next;
} Node;

// Funcs for time 
String* stringify_time(const struct tm* timestruct){
    char buffer[32];
    int sprintf_status = sprintf(buffer, "%02d:%02d:%04d %02d:%02d:%02d",
     timestruct->tm_mday, (timestruct->tm_mon + 1), (timestruct->tm_year + 1900), 
     timestruct->tm_hour, timestruct->tm_min, timestruct->tm_sec); 
    
    if (sprintf_status < 0) return NULL; 

    String* s = create_string(buffer);

    return s;
}

errCodes parse_time(const char* datetime, struct tm** timestruct){
    int day, month, year;
    if (sscanf(datetime, "%d:%d:%d", &day, &month, &year) != 3) {
        return DATE_FORMAT_ERR;
    }

    struct tm* ts = malloc(sizeof(struct tm));
    if (!ts) return MALLOC_ERR;

    ts->tm_mday = day;
    ts->tm_mon = month - 1;
    ts->tm_year = year - 1900;

    ts->tm_isdst = -1;

    time_t timestamp = mktime(ts);

    if (timestamp == -1) {
        free(ts);
        return DATE_FORMAT_ERR;
    }

    // date must be in range of 1980 and 2050
    struct tm* validated_time = localtime(&timestamp);
    if (!validated_time){
        free(ts);
        return DATE_FORMAT_ERR;
    }
    if (validated_time->tm_year + 1900 < 1980 || validated_time->tm_year + 1900 > 2050) {
        free(ts);
        return DATE_FORMAT_ERR;
    }

    *timestruct = ts;

    return SUCCESS;
}

// Linked list functions
Node* create_node(Liver liver){
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) return NULL;

    new_node->liver.surname = copy_new_string(liver.surname);
    new_node->liver.name = copy_new_string(liver.name);
    new_node->liver.patronymic = copy_new_string(liver.patronymic);
    new_node->liver.birthday = copy_new_string(liver.birthday);
    new_node->liver.gender = liver.gender;
    new_node->liver.salary = liver.salary;
    new_node->next = NULL;

    if (!new_node->liver.surname || !new_node->liver.name || !new_node->liver.patronymic || !new_node->liver.birthday) {
        // Free allocated strings
        if (new_node->liver.surname) delete_string(new_node->liver.surname);
        if (new_node->liver.name) delete_string(new_node->liver.name);
        if (new_node->liver.patronymic) delete_string(new_node->liver.patronymic);
        if (new_node->liver.birthday) delete_string(new_node->liver.birthday);
        free(new_node);
        return NULL;
    }

    return new_node;
}

void delete_node(Node* node){
    if(node){
        delete_string(node->liver.surname);
        delete_string(node->liver.name);
        delete_string(node->liver.patronymic);
        delete_string(node->liver.birthday);
        free(node);
    }
}

void prepend(Node** head, Node* new_node) {
    if (new_node == NULL) return;

    new_node->next = *head;
    *head = new_node;
}

// Function to add a node to the end of the list
void append(Node** head, Node* new_node) {
    if (new_node == NULL) return;

    if (*head == NULL) {
        *head = new_node;
    } else {
        Node* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

// read from file
int compare_by_birthday(const void* a, const void* b) {
    Liver* liver_a = (Liver*)a;
    Liver* liver_b = (Liver*)b;
    return strcmp(liver_a->birthday->text, liver_b->birthday->text);
}

errCodes read_file_to_list(const char* filename, Node** head) {
    FILE* file = fopen(filename, "r");
    if (!file) return DATE_FORMAT_ERR;

    int capacity = 10;
    int cnt = 0;
    Liver *temp_array = malloc(sizeof(Liver) * (capacity));
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        int is_empty = 1;
        for (int i = 0; line[i]; i++) {
            if (!isspace(line[i])) {
                is_empty = 0;
                break;
            }
        }
        if (is_empty) continue;

        Liver liver;
        char surname[STR_FIELD_SIZE], name[STR_FIELD_SIZE], patronymic[STR_FIELD_SIZE];
        char birthday_str[STR_FIELD_SIZE];
        char gender;
        double salary;

        int parsed = sscanf(line, "%99s %99s %99s %99s %c %lf", surname, name, patronymic, birthday_str, &gender, &salary);
        if (parsed != 6) {
            continue;
        }

        liver.surname = create_string(surname);
        liver.name = create_string(name);
        liver.patronymic = create_string(patronymic);

        struct tm* t1 = NULL;
        if((parse_time(birthday_str, &t1) != SUCCESS)){
            if (liver.surname) delete_string(liver.surname);
            if (liver.name) delete_string(liver.name);
            if (liver.patronymic) delete_string(liver.patronymic);
            free(temp_array);
            fclose(file);
            return MALLOC_ERR;
        }

        liver.birthday = stringify_time(t1);
        if (!liver.surname || !liver.name || !liver.patronymic || !liver.birthday) {
            if (liver.surname) delete_string(liver.surname);
            if (liver.name) delete_string(liver.name);
            if (liver.patronymic) delete_string(liver.patronymic);
            if (liver.birthday) delete_string(liver.birthday);
            free(temp_array);
            fclose(file);
            return MALLOC_ERR;
        }

        liver.gender = gender;
        liver.salary = salary;

        if (cnt == capacity){
            capacity *= 2;
            Liver *temp = realloc(temp_array, sizeof(Liver) * capacity);
            if (!temp){
                delete_string(liver.surname);
                delete_string(liver.name);
                delete_string(liver.patronymic);
                delete_string(liver.birthday);
                fclose(file);
                free(temp_array);
            }
        }
    }
    fclose(file);

    qsort(temp_array, cnt, sizeof(Liver), compare_by_birthday);

    *head = NULL;
    for (int i = 0; i < cnt; i++) {
        Node* new_node = create_node(temp_array[i]);
        if (!new_node) {
            for (int j = i; j < cnt; j++) {
                delete_string(temp_array[j].surname);
                delete_string(temp_array[j].name);
                delete_string(temp_array[j].patronymic);
                delete_string(temp_array[j].birthday);
            }
            free(temp_array);
            return MALLOC_ERR;
        }

        if (*head == NULL) {
            *head = new_node;
        } else {
            Node* temp = *head;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = new_node;
        }
    }

    for (int i = 0; i < cnt; i++) {
        delete_string(temp_array[i].surname);
        delete_string(temp_array[i].name);
        delete_string(temp_array[i].patronymic);
        delete_string(temp_array[i].birthday);
    }

    // free(temp_array);
    return SUCCESS;
}


Node* search_livers(Node* head, const char* surname, const char* name, const char* patronymic){
    Node* current = head;
    while(current){
        if(strcmp(current->liver.surname->text, surname) == 0 &&
           strcmp(current->liver.name->text, name) == 0 &&
           strcmp(current->liver.patronymic->text, patronymic) == 0){
               return current;
           }
        current = current->next;
    }
    return NULL;
}

errCodes export_list_to_file(Node* head, const char* filename){
    FILE* file = fopen(filename, "w");
    if(!file) return DATE_FORMAT_ERR;

    Node* current = head;
    while(current){
        fprintf(file, "%s %s %s %s %c %.2lf\n",
            current->liver.surname->text,
            current->liver.name->text,
            current->liver.patronymic->text,
            current->liver.birthday->text,
            current->liver.gender,
            current->liver.salary);
        current = current->next;
    }

    fclose(file);
    return SUCCESS;
}

void free_list(Node* head){
    while(head){
        Node* temp = head;
        head = head->next;
        delete_node(temp);
    }
}

int main(){
    char input_filename[STR_FIELD_SIZE];
    printf("Введите имя входного файла: ");
    if(scanf("%99s", input_filename) != 1){
        printf("Ошибка ввода имени файла.\n");
        return 1;
    }
    while(getchar() != '\n');

    Node* head = NULL;
    errCodes status = read_file_to_list(input_filename, &head);
    if(status != SUCCESS){
        printf("Ошибка чтения файла.\n");
        free_list(head);
        return 1;
    }

    int command;
    while(1){
        printf("\n--- Меню ---\n");
        printf("1. Добавить жителя\n");
        printf("2. Удалить жителя\n");
        printf("3. Изменить жителя\n");
        printf("4. Показать всех жителей\n");
        printf("5. Поиск жителя\n");
        printf("6. Экспортировать данные в файл\n");
        printf("0. Выход\n");
        printf("Выберите опцию: ");

        if(scanf("%d", &command) != 1){
            printf("неверный ввод для выбора опции.\n");
            while(getchar() != '\n');
            continue;
        }
        while(getchar() != '\n');

        switch(command){
            case 1: { 
                Liver new_liver;
                char surname[STR_FIELD_SIZE], name[STR_FIELD_SIZE], patronymic[STR_FIELD_SIZE];
                char birthday_str[STR_FIELD_SIZE];
                char gender;
                double salary;

                printf("Введите фамилию: ");
                if(scanf("%99s", surname) != 1){
                    printf("Ошибка ввода фамилии.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите имя: ");
                if(scanf("%99s", name) != 1){
                    printf("Ошибка ввода имени.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите отчество: ");
                if(scanf("%99s", patronymic) != 1){
                    printf("Ошибка ввода отчества.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите дату рождения (dd:mm:yyyy): ");
                if(scanf("%99s", birthday_str) != 1){
                    printf("Ошибка ввода даты рождения.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите пол (M/F): ");
                if(scanf(" %c", &gender) != 1 || (gender != 'M' && gender != 'F')){
                    printf("Ошибка ввода пола.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите зарплату: ");
                if(scanf("%lf", &salary) != 1 || !isfinite(salary)){
                    printf("Ошибка ввода зарплаты.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                // Создание нового жителя
                new_liver.surname = create_string(surname);
                new_liver.name = create_string(name);
                new_liver.patronymic = create_string(patronymic);
                new_liver.birthday = create_string(birthday_str);
                new_liver.gender = gender;
                new_liver.salary = salary;

                if(!new_liver.surname || !new_liver.name || !new_liver.patronymic || !new_liver.birthday){
                    printf("Ошибка выделения памяти для жителя.\n");
                    if(new_liver.surname) delete_string(new_liver.surname);
                    if(new_liver.name) delete_string(new_liver.name);
                    if(new_liver.patronymic) delete_string(new_liver.patronymic);
                    if(new_liver.birthday) delete_string(new_liver.birthday);
                    break;
                }

                Node* new_node = create_node(new_liver);

                delete_string(new_liver.surname);
                delete_string(new_liver.name);
                delete_string(new_liver.patronymic);
                delete_string(new_liver.birthday);

                if(!new_node){
                    printf("Ошибка создания узла для жителя.\n");
                    break;
                }

                // Добавление в конец списка
                append(&head, new_node);

                printf("Житель успешно добавлен.\n");
                break;
            }
            case 2: { 
                char surname[STR_FIELD_SIZE], name[STR_FIELD_SIZE], patronymic[STR_FIELD_SIZE];
                printf("Введите фамилию жителя для удаления: ");
                if(scanf("%99s", surname) != 1){
                    printf("Ошибка ввода фамилии.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите имя жителя для удаления: ");
                if(scanf("%99s", name) != 1){
                    printf("Ошибка ввода имени.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите отчество жителя для удаления: ");
                if(scanf("%99s", patronymic) != 1){
                    printf("Ошибка ввода отчества.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                Node* target = search_livers(head, surname, name, patronymic);
                if(!target){
                    printf("Житель не найден.\n");
                    break;
                }

                // Удаление узла из списка
                if(head == target){
                    head = head->next;
                }
                else{
                    Node* prev = head;
                    while(prev->next != NULL && prev->next != target){
                        prev = prev->next;
                    }
                    if(prev->next == target){
                        prev->next = target->next;
                    }
                }
                delete_node(target);
                printf("Житель успешно удален.\n");
                break;
            }
            case 3: { 
                char surname[STR_FIELD_SIZE], name[STR_FIELD_SIZE], patronymic[STR_FIELD_SIZE];
                printf("Введите фамилию жителя для изменения: ");
                if(scanf("%99s", surname) != 1){
                    printf("Ошибка ввода фамилии.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите имя жителя для изменения: ");
                if(scanf("%99s", name) != 1){
                    printf("Ошибка ввода имени.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите отчество жителя для изменения: ");
                if(scanf("%99s", patronymic) != 1){
                    printf("Ошибка ввода отчества.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                Node* target = search_livers(head, surname, name, patronymic);
                if(!target){
                    printf("Житель не найден.\n");
                    break;
                }

                // Запрос новых данных
                char new_surname[STR_FIELD_SIZE], new_name[STR_FIELD_SIZE], new_patronymic[STR_FIELD_SIZE];
                char new_birthday_str[STR_FIELD_SIZE];
                char new_gender;
                double new_salary;

                printf("Введите новую фамилию: ");
                if(scanf("%99s", new_surname) != 1){
                    printf("Ошибка ввода фамилии.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите новое имя: ");
                if(scanf("%99s", new_name) != 1){
                    printf("Ошибка ввода имени.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите новое отчество: ");
                if(scanf("%99s", new_patronymic) != 1){
                    printf("Ошибка ввода отчества.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите новую дату рождения (dd:mm:yyyy): ");
                if(scanf("%99s", new_birthday_str) != 1){
                    printf("Ошибка ввода даты рождения.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите новый пол (M/F): ");
                if(scanf(" %c", &new_gender) != 1 || (new_gender != 'M' && new_gender != 'F')){
                    printf("Ошибка ввода пола.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите новую зарплату: ");
                if(scanf("%lf", &new_salary) != 1 || !isfinite(new_salary)){
                    printf("Ошибка ввода зарплаты.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                // Обновление данных жителя
                String* temp_surname = create_string(new_surname);
                String* temp_name = create_string(new_name);
                String* temp_patronymic = create_string(new_patronymic);
                String* temp_birthday = create_string(new_birthday_str);

                if(!temp_surname || !temp_name || !temp_patronymic || !temp_birthday){
                    printf("Ошибка выделения памяти для новых данных.\n");
                    if(temp_surname) delete_string(temp_surname);
                    if(temp_name) delete_string(temp_name);
                    if(temp_patronymic) delete_string(temp_patronymic);
                    if(temp_birthday) delete_string(temp_birthday);
                    break;
                }

                // Проверка корректности даты рождения
                struct tm* tm_birthday;
                status = parse_time(new_birthday_str, &tm_birthday);
                if(status != SUCCESS){
                    printf("Некорректная дата рождения.\n");
                    delete_string(temp_surname);
                    delete_string(temp_name);
                    delete_string(temp_patronymic);
                    delete_string(temp_birthday);
                    break;
                }

                // Обновление строк
                delete_string(target->liver.surname);
                delete_string(target->liver.name);
                delete_string(target->liver.patronymic);
                delete_string(target->liver.birthday);

                target->liver.surname = temp_surname;
                target->liver.name = temp_name;
                target->liver.patronymic = temp_patronymic;
                target->liver.birthday = temp_birthday;
                target->liver.gender = new_gender;
                target->liver.salary = new_salary;

                printf("Житель успешно изменен.\n");
                break;
            }
            case 4: { 
                Node* current = head;
                int count = 1;
                while(current){
                    printf("\nЖитель %d:\n", count++);
                    printf("Фамилия: %s\n", current->liver.surname->text);
                    printf("Имя: %s\n", current->liver.name->text);
                    printf("Отчество: %s\n", current->liver.patronymic->text);
                    printf("Дата рождения: %s\n", current->liver.birthday->text);
                    printf("Пол: %c\n", current->liver.gender);
                    printf("Зарплата: %.2lf\n", current->liver.salary);
                    current = current->next;
                }
                if(count == 1){
                    printf("Список жителей пуст.\n");
                }
                break;
            }
            case 5: { 
                char surname[STR_FIELD_SIZE], name[STR_FIELD_SIZE], patronymic[STR_FIELD_SIZE];
                printf("Введите фамилию жителя для поиска: ");
                if(scanf("%99s", surname) != 1){
                    printf("Ошибка ввода фамилии.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите имя жителя для поиска: ");
                if(scanf("%99s", name) != 1){
                    printf("Ошибка ввода имени.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                printf("Введите отчество жителя для поиска: ");
                if(scanf("%99s", patronymic) != 1){
                    printf("Ошибка ввода отчества.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                Node* found = search_livers(head, surname, name, patronymic);
                if(!found){
                    printf("Житель не найден.\n");
                }
                else{
                    printf("\nНайденный житель:\n");
                    printf("Фамилия: %s\n", found->liver.surname->text);
                    printf("Имя: %s\n", found->liver.name->text);
                    printf("Отчество: %s\n", found->liver.patronymic->text);
                    printf("Дата рождения: %s\n", found->liver.birthday->text);
                    printf("Пол: %c\n", found->liver.gender);
                    printf("Зарплата: %.2lf\n", found->liver.salary);
                }
                break;
            }
            case 6: { 
                char output_filename[STR_FIELD_SIZE];
                printf("Введите имя выходного файла: ");
                if(scanf("%99s", output_filename) != 1){
                    printf("Ошибка ввода имени файла.\n");
                    while(getchar() != '\n');
                    break;
                }
                while(getchar() != '\n');

                status = export_list_to_file(head, output_filename);
                if(status != SUCCESS){
                    printf("Ошибка экспорта данных в файл.\n");
                }
                else{
                    printf("Данные успешно экспортированы в файл %s.\n", output_filename);
                }
                break;
            }
            case 0: { 
                printf("Выход из программы.\n");
                break;
            }
            default: {
                printf("Ошибка: некорректный выбор, попробуйте снова.\n");
                break;
            }
        }
    }

    free_list(head);
    return 0;
}
