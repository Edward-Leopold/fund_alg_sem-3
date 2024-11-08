#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "strings.h"

#define STR_FIELD_SIZE 100 

typedef enum errCodes{
    MALLOC_ERR,
    REALLOC_ERR,
    DATE_FORMAT_ERR,
    MAIL_NOT_FOUND,
    ADDRESS_INPUT_ERR,
    MAIL_ID_ERR,

    SUCCESS
}errCodes;

typedef struct Address{
    String* town;
    String* street;
    int house;
    String* building;
	int apartment;
	String* index;
}Address;

typedef struct Mail{
    Address address;
    double weight;
    String* mail_id;
    String* mail_created_time;
    String* mail_received_time;
}Mail;

typedef struct Post{
    Address* address;
    Mail* mails;
    int mem_size;
    int mails_size;
}Post;


// funcs for time 
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
    int day, month, year, hours, minutes, seconds;
    if (sscanf(datetime, "%d:%d:%d %d:%d:%d", &day, &month, &year, &hours, &minutes, &seconds) != 6) {
        return DATE_FORMAT_ERR;
    }

    struct tm* ts = malloc(sizeof(struct tm));
    if (!ts) return MALLOC_ERR;

    ts->tm_mday = day;
    ts->tm_mon = month - 1;
    ts->tm_year = year - 1900;
    ts->tm_hour = hours;
    ts->tm_min = minutes;
    ts->tm_sec = seconds;

    ts->tm_isdst = -1;

    time_t timestamp = mktime(ts);

    if (timestamp == -1) {
        if(ts) free(ts);
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

errCodes compare_time(const struct tm* t1, const struct tm* t2){
    time_t time1 = mktime((struct tm *)t1);
    time_t time2 = mktime((struct tm *)t2);

    if (time1 >= time2) return DATE_FORMAT_ERR;

    return SUCCESS;
}

// func for address
void delete_address(Address *addr){
    if (addr){
        delete_string(addr->town);
        delete_string(addr->street);
        delete_string(addr->building);
        delete_string(addr->index);
        free(addr);
    }
}

Address* create_adress(char* town, char* street, int house, char* building, int apartment, char* index){
    Address* addr = malloc(sizeof(Address));
    if (!addr) return NULL;

    addr->town = create_string(town);
    addr->street = create_string(street);
    addr->building = create_string(building);
    addr->index = create_string(index);
    addr->house = house;
    addr->apartment = apartment;

    if (!addr->town || !addr->street || !addr->building || !addr->index) {
        delete_address(addr);
        return NULL;
    }

    return addr;
}

errCodes validate_address(char* town, char* street, int house, char* building, int apartment, char* index){
    if (strlen(town) == 0) return ADDRESS_INPUT_ERR;
    if (strlen(street) == 0) return ADDRESS_INPUT_ERR;
    if (house < 1) return ADDRESS_INPUT_ERR;
    if (strlen(building) == 0) return ADDRESS_INPUT_ERR;   
    if (apartment < 1) return ADDRESS_INPUT_ERR;
    if (strlen(index) != 6) return ADDRESS_INPUT_ERR;   
    for (int i = 0; i < 6; i++){
        if (!isdigit(index[i])) return ADDRESS_INPUT_ERR; 
    }

    return SUCCESS;
}
// func for mail
void delete_mail(Mail *mail){
    if (mail){
        delete_string(mail->address.town);
        delete_string(mail->address.street);
        delete_string(mail->address.building);
        delete_string(mail->address.index);

        delete_string(mail->mail_id);
        delete_string(mail->mail_created_time);
        delete_string(mail->mail_received_time);
        free(mail);
    }
}

Mail* create_mail(Address *addr, double weight, char* mail_id, String* mail_created_time, String* mail_received_time){
    Mail* mail = malloc(sizeof(Mail));
    if (!mail) return NULL;

    String *mail_id_s = create_string(mail_id);
    if (!mail_id_s) {
        delete_mail(mail);
        return NULL;
    }

    mail->address.town = copy_new_string(addr->town);
    mail->address.street = copy_new_string(addr->street);
    mail->address.building = copy_new_string(addr->building);
    mail->address.index = copy_new_string(addr->index);
    mail->address.house = addr->house;
    mail->address.apartment = addr->apartment;

    mail->weight = weight;
    mail->mail_id = mail_id_s;
    mail->mail_created_time = copy_new_string(mail_created_time);
    mail->mail_received_time = copy_new_string(mail_received_time);

    if (!mail->address.town || !mail->address.street || !mail->address.building || !mail->address.index ||
        !mail->mail_created_time || !mail->mail_received_time) {
        delete_mail(mail);
        return NULL;
    }

    return mail;
}

errCodes validte_mail_id(const Post* post, const char* id){
    if (strlen(id) != 14) return MAIL_ID_ERR;   
    for (int i = 0; i < 14; i++){
        if (!isdigit(id[i])) return MAIL_ID_ERR; 
    }
    for (int j = 0; j < post->mails_size; j++){
        if (strcmp(post->mails[j].mail_id->text, id) == 0) return MAIL_ID_ERR;
    }

    return SUCCESS;
}

// func for post
Post* create_post(Address *addr){
    Post* post = malloc(sizeof(Post));
    if (!post) return NULL;

    post->address = addr;
    post->mem_size = 10;
    post->mails_size = 0;

    post->mails = malloc(sizeof(Mail) * (post->mem_size + 1));
    if (!post->mails) {
        free(post);
        return NULL;
    }

    return post;
}

void delete_post(Post* post){
    if(post){
        for (int i = 0; i < post->mails_size; i++) {
            delete_string(post->mails[i].mail_id);
            delete_string(post->mails[i].mail_created_time);
            delete_string(post->mails[i].mail_received_time);

            delete_string(post->mails[i].address.town);
            delete_string(post->mails[i].address.street);
            delete_string(post->mails[i].address.building);
            delete_string(post->mails[i].address.index);
        }
        free(post->mails);
        delete_address(post->address);
        free(post);
    }
}

void delete_mail_content(Mail* mail) {
    if (mail) {
        delete_string(mail->mail_id);
        delete_string(mail->mail_created_time);
        delete_string(mail->mail_received_time);
        delete_string(mail->address.town);
        delete_string(mail->address.street);
        delete_string(mail->address.building);
        delete_string(mail->address.index);
    }
}

errCodes post_add_mail(Post* post, Mail* mail){
    Mail copied_mail;
    copied_mail.address.town = copy_new_string(mail->address.town);
    copied_mail.address.street = copy_new_string(mail->address.street);
    copied_mail.address.building = copy_new_string(mail->address.building);
    copied_mail.address.index = copy_new_string(mail->address.index);
    copied_mail.address.house = mail->address.house;
    copied_mail.address.apartment = mail->address.apartment;

    copied_mail.weight = mail->weight;
    copied_mail.mail_id = copy_new_string(mail->mail_id);
    copied_mail.mail_created_time = copy_new_string(mail->mail_created_time);
    copied_mail.mail_received_time = copy_new_string(mail->mail_received_time);

    if (!copied_mail.address.town || !copied_mail.address.street || !copied_mail.address.building ||
        !copied_mail.address.index || !copied_mail.mail_id || !copied_mail.mail_created_time || !copied_mail.mail_received_time) {
        
        delete_string(copied_mail.address.town);
        delete_string(copied_mail.address.street);
        delete_string(copied_mail.address.building);
        delete_string(copied_mail.address.index);
        delete_string(copied_mail.mail_id);
        delete_string(copied_mail.mail_created_time);
        delete_string(copied_mail.mail_received_time);

        return MALLOC_ERR;
    }

    if (post->mails_size == post->mem_size){
        post->mem_size *= 2;
        Mail* temp = realloc(post->mails, sizeof(Mail) * (post->mem_size + 1));
        if (!temp) {
            delete_string(copied_mail.mail_id);
            delete_string(copied_mail.mail_created_time);
            delete_string(copied_mail.mail_received_time);

            delete_string(copied_mail.address.town);
            delete_string(copied_mail.address.street);
            delete_string(copied_mail.address.building);
            delete_string(copied_mail.address.index);
            return REALLOC_ERR;
        }
        post->mails = temp;
    }
    post->mails[post->mails_size] = copied_mail;
    post->mails_size++;

    return SUCCESS;
}

errCodes post_delete_mail(Post* post, char* mail_id) {
    String* mail_id_str = create_string(mail_id);
    if (!mail_id_str) return MALLOC_ERR;

    int index = -1;
    for (int i = 0; i < post->mails_size; i++) {
        if (compare_strings(post->mails[i].mail_id, mail_id_str) == 0) {
            index = i;
            break;
        }
    }
    delete_string(mail_id_str);
    if (index == -1) return MAIL_NOT_FOUND;

    delete_mail_content(&post->mails[index]);
    for (int i = index; i < post->mails_size - 1; i++) {
        post->mails[i] = post->mails[i + 1];
    }
    post->mails_size--;
    
    return SUCCESS;
}

errCodes search_mail_by_id(Post* post, char* mail_id, Mail* found){
    String* mail_id_str = create_string(mail_id);
    if (!mail_id_str) return MALLOC_ERR;

    int index = -1;
    for (int i = 0; i < post->mails_size; i++) {
        if (compare_strings(post->mails[i].mail_id, mail_id_str) == 0) {
            index = i;
            break;
        }
    }
    delete_string(mail_id_str);
    if (index == -1) return MAIL_NOT_FOUND;

    *found = post->mails[index];
    return SUCCESS;
}

// sorting in order of indexes
int comparator_indexes(const void *a, const void *b){
    const Mail* mail_a = (const Mail*)a;
    const Mail* mail_b = (const Mail*)b;

    int res = compare_strings(mail_a->address.index, mail_b->address.index);
    if (res != 0) return res;

    return compare_strings(mail_a->mail_id, mail_b->mail_id);
}

void sort_mails_by_indexes(Post *post){
    qsort(post->mails, post->mails_size, sizeof(Mail), comparator_indexes);
    return;
}

// find delivered and expired mails
int compare_created_times(const void* a, const void* b) {
    Mail* mail_a = *(Mail**)a;
    Mail* mail_b = *(Mail**)b;

    return strcmp(mail_a->mail_created_time->text, mail_b->mail_created_time->text);
}

errCodes delivered_mails(Post* post, Mail*** result){
    int capacity = 10;
    int cnt = 0; 
    Mail** found = (Mail**)malloc(sizeof(Mail*) * (capacity + 1));

    time_t curtime = time(NULL);

    for (int i = 0; i < post->mails_size; i++){
        if (cnt == capacity){
            capacity *= 2;
            Mail** temp = realloc(found, sizeof(Mail*) * (capacity + 1));
            if (!temp){
                free(found);
                return REALLOC_ERR;
            }
            found = temp;
        }

        struct tm* received_time_tm = NULL;
        errCodes parse_status = parse_time(post->mails[i].mail_received_time->text, &received_time_tm);
        if (parse_status != SUCCESS) {
            free(found);
            return parse_status;
        }

        time_t received_time = mktime(received_time_tm);
        free(received_time_tm);
        if (received_time == -1){
            free(found);
            return MALLOC_ERR;
        }

        if (difftime(curtime, received_time) > 0) {
            found[cnt] = &post->mails[i];
            cnt++;
        }
    }
    found[cnt] = NULL;
    *result = found;

    if (cnt != 0) {
        qsort(*result, cnt, sizeof(Mail*), compare_created_times);
    }

    return SUCCESS;
}

errCodes expired_mails(Post* post, Mail*** result) {
    int capacity = 10;
    int cnt = 0;
    Mail** found = (Mail**)malloc(sizeof(Mail*) * (capacity + 1));
    if (!found) return MALLOC_ERR;

    time_t curtime = time(NULL);

    for (int i = 0; i < post->mails_size; i++) {
        if (cnt == capacity) {
            capacity *= 2;
            Mail** temp = realloc(found, sizeof(Mail*) * (capacity + 1));
            if (!temp) {
                free(found);
                return REALLOC_ERR;
            }
            found = temp;
        }

        struct tm* received_time_tm = NULL;
        errCodes parse_status = parse_time(post->mails[i].mail_received_time->text, &received_time_tm);
        if (parse_status != SUCCESS) {
            free(found);
            return parse_status;
        }

        time_t received_time = mktime(received_time_tm);
        free(received_time_tm);
        if (received_time == -1) {
            free(found);
            return MALLOC_ERR;
        }

        if (difftime(received_time, curtime) > 0) { 
            found[cnt] = &post->mails[i];
            cnt++;
        }
    }

    found[cnt] = NULL; 
    *result = found;

    if (cnt > 1) {
        qsort(*result, cnt, sizeof(Mail*), compare_created_times);
    }

    return SUCCESS;
}


int main(){
    Address *addr = create_adress("Moscow", "Lenina", 34, "1", 232, "370024");
    if (!addr) {
        printf("Error initial address for post\n");
        return 1;
    }
    Post *post = create_post(addr);
    if (!post) {
        printf("Error initial post\n");
        delete_address(addr);
        return 1;
    }

    // adding initial mails 
    struct tm* init_tm_t1 = NULL;
    struct tm* init_tm_t2 = NULL;
    errCodes parse_init_time_1 = parse_time("01:01:2024 09:00:00", &init_tm_t1);
    if (parse_init_time_1 != SUCCESS){
        printf("Error creating first timestamp\n");
        delete_post(post);
        return 1;
    }
    errCodes parse_init_time_2 = parse_time("02:01:2024 17:00:00", &init_tm_t2);
    if (parse_init_time_2 != SUCCESS){
        printf("Error creating second timestamp\n");
        free(init_tm_t1);
        delete_post(post);
        return 1;
    }
    String *init_created_time_str = stringify_time(init_tm_t1);
    if (!init_created_time_str){
        printf("Error creating first time string!\n");
        free(init_tm_t1);
        free(init_tm_t2);
        delete_post(post);
        return 1;
    }
    String *init_received_time_str = stringify_time(init_tm_t2);
    if (!init_received_time_str){
        printf("Error creating second time string!\n");
        delete_string(init_created_time_str);
        free(init_tm_t1);
        free(init_tm_t2);
        delete_post(post);
        return 1;
    }
    free(init_tm_t1);
    free(init_tm_t2);

    Mail *mail1 = create_mail(addr, 1.5, "23456789012345", init_created_time_str, init_received_time_str);
    if (!mail1) {
        printf("Error creating first mail\n");
        delete_string(init_created_time_str);
        delete_string(init_received_time_str);
        delete_post(post);
        return 1;
    }

    Mail *mail2 = create_mail(addr, 2.0, "12345888901234", init_created_time_str, init_received_time_str);
    if (!mail2) {
        printf("Error creating second mail\n");
        delete_string(init_created_time_str);
        delete_string(init_received_time_str);
        delete_mail(mail1);
        delete_post(post);
        return 1;
    }

    Mail *mail3 = create_mail(addr, 2.7, "34567890123456", init_created_time_str, init_received_time_str);
    if (!mail3) {
        printf("Error creating third mail\n");
        delete_string(init_created_time_str);
        delete_string(init_received_time_str);
        delete_mail(mail1);
        delete_mail(mail2);
        delete_post(post);
        return 1;
    }
    
    delete_string(init_created_time_str);
    delete_string(init_received_time_str);

    errCodes post_add_mail_status1 = post_add_mail(post, mail1);
    if (post_add_mail_status1 != SUCCESS) {
        printf("Error adding first initial mail to post\n");
        delete_mail(mail1);
        delete_mail(mail2);
        delete_mail(mail3);
        delete_post(post);
        return 1;
    }

    errCodes post_add_mail_status2 = post_add_mail(post, mail2);
    if (post_add_mail_status2 != SUCCESS) {
        printf("Error adding second initial mail to post\n");
        delete_mail(mail1);
        delete_mail(mail2);
        delete_mail(mail3);
        delete_post(post);
        return 1;
    }

    errCodes post_add_mail_status3 = post_add_mail(post, mail3);
    if (post_add_mail_status3 != SUCCESS) {
        printf("Error adding third initial mail to post\n");
        delete_mail(mail1);
        delete_mail(mail2);
        delete_mail(mail3);
        delete_post(post);
        return 1;
    }

    delete_mail(mail1);
    delete_mail(mail2);
    delete_mail(mail3);
    // end of adding initial mails 

    sort_mails_by_indexes(post);

    int command;
    do {
        printf("\n--- Почтовое меню ---\n");
        printf("1. Добавить посылку\n");
        printf("2. Удалить посылку\n");
        printf("3. Показать все посылки\n");
        printf("4. Поиск посылки по id\n");
        printf("5. Показать все доставленные посылки\n");
        printf("6. Показать все недоставленные посылки\n");
        printf("0. Выход\n");
        printf("Выберите опцию: ");
        if (scanf("%d", &command) != 1) {
            printf("Ошибка: неверный ввод для выбора опции.\n");
            while (getchar() != '\n'); // очистка буфера
            continue;
        }

        switch (command) {
            case 1: { // create mail
                int buf_size = STR_FIELD_SIZE; // buf_size = 100
                char town[buf_size], street[buf_size], building[buf_size], index[buf_size], mail_id[buf_size], created_time[buf_size], received_time[buf_size];
                int house, apartment;
                double weight;
                
                // receiving data
                while (1) {
                    printf("Введите город: ");
                    if (scanf("%99s", town) == 1 && strlen(town) > 0) break;
                    printf("Неверный ввод для города.\n");
                    while (getchar() != '\n');
                }
                while (getchar() != '\n');


                while (1) {
                    printf("Введите улицу: ");
                    if (scanf("%99s", street) == 1 && strlen(street) > 0) break;
                    printf("Неверный ввод для улицы.\n");
                    while (getchar() != '\n');
                }
                while (getchar() != '\n');

                while (1) {
                    printf("Введите номер дома: ");
                    if (scanf("%d", &house) == 1 && house > 0) break;
                    printf("Неверный ввод для номера дома.\n");
                    while (getchar() != '\n');
                }
                while (getchar() != '\n');


                while (1) {
                    printf("Введите корпус: ");
                    if (scanf("%99s", building) == 1 && strlen(building) > 0) break;
                    printf("Неверный ввод для корпуса.\n");
                    while (getchar() != '\n');
                }
                while (getchar() != '\n');

                while (1) {
                    printf("Введите номер квартиры: ");
                    if (scanf("%d", &apartment) == 1 && apartment > 0) break;
                    printf("Неверный ввод для номера квартиры.\n");
                    while (getchar() != '\n');
                }
                while (getchar() != '\n');

                while (1) {
                    printf("Введите индекс получателя: ");
                    if (scanf("%99s", index) == 1 && strlen(index) == 6) {
                        int valid = 1;
                        for (int i = 0; i < 6; i++) {
                            if (!isdigit(index[i])) {
                                valid = 0;
                                break;
                            }
                        }
                        if (valid) break;
                    }
                    printf("Неверный ввод для индекса (должен содержать 6 цифр).\n");
                    while (getchar() != '\n');
                }
                while (getchar() != '\n');

                while (1) {
                    printf("Введите вес послыки: ");
                    if (scanf("%lf", &weight) == 1 && isfinite(weight)) break;
                    printf("Некорректный вес посылки.\n");
                    while (getchar() != '\n');
                }
                while (getchar() != '\n');

                while (1) {
                    printf("Введите ID письма: ");
                    if (scanf("%99s", mail_id) == 1 && validte_mail_id(post, mail_id) == SUCCESS) break;
                    printf("Неверный идентификатор посылки (должно быть 14 цифр).\n");
                    while (getchar() != '\n');
                }
                while (getchar() != '\n');

                struct tm* tm_t1 = NULL;
                struct tm* tm_t2 = NULL;
                while (1) {
                    printf("Введите время создания (dd:MM:yyyy hh:mm:ss): ");
                    
                    if (fgets(created_time, buf_size, stdin) == NULL || parse_time(created_time, &tm_t1) != SUCCESS) {
                        printf("Ошибка задания времени создания послыки.\n");
                        continue;
                    }

                    printf("Введите время вручения (dd:MM:yyyy hh:mm:ss): ");
                    if (fgets(received_time, buf_size, stdin) == NULL || parse_time(received_time, &tm_t2) != SUCCESS) {
                        printf("Ошибка задания времени получения послыки.\n");
                        free(tm_t1);
                        tm_t1 = NULL;
                        continue;
                    }

                    if (compare_time(tm_t1, tm_t2) == SUCCESS) break; // compare time (must be t1 < t2)
                    printf("Время отправки должно быть меньше времени получения!\n");
                    free(tm_t1);
                    free(tm_t2);
                    tm_t1 = NULL;
                    tm_t2 = NULL;
                }

                // creating structures 
                // create address for mail
                Address *mail_addr = create_adress(town, street, house, building, apartment, index);
                if (!mail_addr) {
                    printf("Не удалось выделить память для данных адреса!\n");
                    free(tm_t1);
                    free(tm_t2);
                    continue; 
                }

                // create time strings for mail
                String *created_time_str = stringify_time(tm_t1);
                if (!created_time_str){
                    printf("Не удалось выделить память для данных времени создания посылки!\n");
                    free(tm_t1);
                    free(tm_t2);
                }
                String *received_time_str = stringify_time(tm_t2);
                if (!received_time_str){
                    printf("Не удалось выделить память для данных времени получения посылки!\n");
                    delete_string(created_time_str);
                    free(tm_t1);
                    free(tm_t2);
                }
                free(tm_t1);
                free(tm_t2);

                // create mail with address for post
                Mail *mail = create_mail(mail_addr, weight, mail_id, created_time_str, received_time_str);
                if (!mail){
                    printf("Не удалось выделить память для данных посылки!\n");
                    delete_address(mail_addr);
                    delete_string(created_time_str);
                    delete_string(received_time_str);
                    continue; 
                }
                delete_address(mail_addr);
                delete_string(created_time_str);
                delete_string(received_time_str);

                errCodes post_add_mail_status = post_add_mail(post, mail);
                if (post_add_mail_status != SUCCESS){
                    printf("Не удалось выделить память для записи послыки в почтовое отделение!\n");
                    delete_mail(mail);
                    continue;
                }
                delete_mail(mail);

                printf("Посылка успешно добавлена!\n");
                sort_mails_by_indexes(post);
                break;
            }
            case 2: { // delete mail
                char mail_id[STR_FIELD_SIZE];
                printf("Введите id посылки для удаления: ");
                if (scanf("%99s", mail_id) != 1) {
                    printf("Неверный формат id.\n");
                    while (getchar() != '\n');
                    continue; 
                }
                while (getchar() != '\n');

                errCodes delete_mail_status = post_delete_mail(post, mail_id);
                if(delete_mail_status != SUCCESS){
                    switch (delete_mail_status){
                    case MALLOC_ERR:
                        printf("Не удалось выделить память для строки id\n");
                        break;
                    case MAIL_NOT_FOUND:
                        printf("Посылка с id %s не найдено\n", mail_id);
                        break;
                    default:
                        break;
                    }
                } else{
                    printf("Посылка с id %s успешно удалено \n", mail_id);
                }

                sort_mails_by_indexes(post);
                break;
            }
            case 3: { // show all mails
                printf("\n--- Список посылок ---\n");
                for (int i = 0; i < post->mails_size; i++) {
                    printf("Послыка %d:\n", i + 1);
                    printf("  Город: %s\n", post->mails[i].address.town->text);
                    printf("  Улица: %s\n", post->mails[i].address.street->text);
                    printf("  Дом: %d\n", post->mails[i].address.house);
                    printf("  Корпус: %s\n", post->mails[i].address.building->text);
                    printf("  Квартира: %d\n", post->mails[i].address.apartment);
                    printf("  Индекс получателя: %s\n", post->mails[i].address.index->text);
                    printf("  Вес послыки: %.2f\n", post->mails[i].weight);
                    printf("  ID: %s\n", post->mails[i].mail_id->text);
                    printf("  Время создания: %s\n", post->mails[i].mail_created_time->text);
                    printf("  Время вручения: %s\n", post->mails[i].mail_received_time->text);
                }
                break;
            }
            case 4: { // search mail by id
                char mail_search_id[STR_FIELD_SIZE];
                printf("Введите id посылки: ");
                if (scanf("%99s", mail_search_id) != 1) {
                    printf("Неверный формат id.\n");
                    while (getchar() != '\n');
                    continue; 
                }
                while (getchar() != '\n');
                
                Mail found_mail;
                errCodes search_mail_by_id_status = search_mail_by_id(post, mail_search_id, &found_mail);
                if(search_mail_by_id_status != SUCCESS){
                    switch (search_mail_by_id_status){
                    case MALLOC_ERR:
                        printf("Не удалось выделить память для строки id\n");
                        break;
                    case MAIL_NOT_FOUND:
                        printf("Посылка с id %s не найдено\n", mail_search_id);
                        break;
                    default:
                        break;
                    }
                } else{
                    printf("Посылка с id %s найдено \n", mail_search_id);
                    printf("  Город: %s\n", found_mail.address.town->text);
                    printf("  Улица: %s\n", found_mail.address.street->text);
                    printf("  Дом: %d\n", found_mail.address.house);
                    printf("  Корпус: %s\n", found_mail.address.building->text);
                    printf("  Квартира: %d\n", found_mail.address.apartment);
                    printf("  Индекс получателя: %s\n", found_mail.address.index->text);
                    printf("  Вес послыки: %.2f\n", found_mail.weight);
                    printf("  ID: %s\n", found_mail.mail_id->text);
                    printf("  Время создания: %s\n", found_mail.mail_created_time->text);
                    printf("  Время вручения: %s\n", found_mail.mail_received_time->text);
                }

                break;
            }
            case 5: { // print delivered mails in order of creation
                Mail **delivered = NULL;
                errCodes delivered_status = delivered_mails(post, &delivered);
                if (delivered_status != SUCCESS){
                    printf("Не удалось выделить память для вывода и сортировки доставленных писем\n");
                    break;
                }

                printf("Посылки доставленные на текущий момент\n");
                for(int i = 0; delivered[i]; i++){
                    printf("Послыка %d:\n", i + 1);
                    printf("  Город: %s\n", delivered[i]->address.town->text);
                    printf("  Улица: %s\n", delivered[i]->address.street->text);
                    printf("  Дом: %d\n", delivered[i]->address.house);
                    printf("  Корпус: %s\n", delivered[i]->address.building->text);
                    printf("  Квартира: %d\n", delivered[i]->address.apartment);
                    printf("  Индекс получателя: %s\n", delivered[i]->address.index->text);
                    printf("  Вес послыки: %.2f\n", delivered[i]->weight);
                    printf("  ID: %s\n", delivered[i]->mail_id->text);
                    printf("  Время создания: %s\n", delivered[i]->mail_created_time->text);
                    printf("  Время вручения: %s\n", delivered[i]->mail_received_time->text);
                }
                free(delivered);

                break;
            }
            case 6: { // print exired mails in order of creation
                Mail **expired = NULL;
                errCodes expired_status = expired_mails(post, &expired);
                if (expired_status != SUCCESS){
                    printf("Не удалось выделить память для вывода и сортировки недоставленных посылок\n");
                    break;
                }

                printf("Посылки недоставленные на текущий момент\n");
                for(int i = 0; expired[i]; i++){
                    printf("Посылка %d:\n", i + 1);
                    printf("  Город: %s\n", expired[i]->address.town->text);
                    printf("  Улица: %s\n", expired[i]->address.street->text);
                    printf("  Дом: %d\n", expired[i]->address.house);
                    printf("  Корпус: %s\n", expired[i]->address.building->text);
                    printf("  Квартира: %d\n", expired[i]->address.apartment);
                    printf("  Индекс получателя: %s\n", expired[i]->address.index->text);
                    printf("  Вес послыки: %.2f\n", expired[i]->weight);
                    printf("  ID: %s\n", expired[i]->mail_id->text);
                    printf("  Время создания: %s\n", expired[i]->mail_created_time->text);
                    printf("  Время вручения: %s\n", expired[i]->mail_received_time->text);
                }
                free(expired);

                break;
            } 
            case 0:
                printf("Выход из программы.\n");
                break;
            default:
                printf("Ошибка: некорректный выбор. Пожалуйста, попробуйте снова.\n");
        }
    } while (command != 0);

    delete_post(post);
    return 0;
}