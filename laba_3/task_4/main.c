#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "strings.h"

typedef enum errCodes{
    MALLOC_ERR,
    REALLOC_ERR,
    DATE_FORMAT_ERR,
    DELETING_MAIL_ERR,
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


// func for time 
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

errCodes validte_mail_id(const char* id){
    if (strlen(id) != 14) return MAIL_ID_ERR;   
    for (int i = 0; i < 14; i++){
        if (!isdigit(id[i])) return MAIL_ID_ERR; 
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

errCodes post_delete_mail(Post* post, String* mail_id) {
    if (!post || !mail_id) return DELETING_MAIL_ERR;

    int index = -1;

    for (int i = 0; i < post->mails_size; i++) {
        if (compare_strings(post->mails[i].mail_id, mail_id) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) return DELETING_MAIL_ERR;

    delete_mail_content(&post->mails[index]);

    for (int i = index; i < post->mails_size - 1; i++) {
        post->mails[i] = post->mails[i + 1];
    }

    post->mails_size--;

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

    int choice;
    do {
        printf("\n--- Почтовое меню ---\n");
        printf("1. Добавить письмо\n");
        printf("2. Удалить письмо\n");
        printf("3. Показать все письма\n");
        printf("4. Выход\n");
        printf("Выберите опцию: ");
        if (scanf("%d", &choice) != 1) {
            printf("Ошибка: неверный ввод для выбора опции.\n");
            while (getchar() != '\n'); // очистка буфера
            continue;
        }

        switch (choice) {
            case 1: {
                int buf_size = 1000;
                char town[1000], street[1000], building[1000], index[1000], mail_id[1000], created_time[1000], received_time[1000];
                int house, apartment;
                double weight;
                
                // receiving data
                printf("Введите город: ");
                if (scanf("%49s", town) != 1) {
                    printf("Неверный ввод для города.\n");
                    continue; 
                } 

                printf("Введите улицу: ");
                if (scanf("%49s", street) != 1) {
                    printf("Неверный ввод для улицы.\n");
                    continue; 
                } 

                printf("Введите номер дома: ");
                if (scanf("%d", &house) != 1) {
                    printf("Неверный ввод для номера дома.\n");
                    continue;  
                }  

                printf("Введите корпус: ");
                if (scanf("%9s", building) != 1) {
                    printf("Неверный ввод для корпуса.\n");
                    continue;  
                }  

                printf("Введите номер квартиры: ");
                if (scanf("%d", &apartment) != 1) {
                    printf("Неверный ввод для номера квартиры.\n");
                    continue;  
                }  

                printf("Введите индекс: ");
                if (scanf("%9s", index) != 1) {
                    printf("Неверный ввод для индекса.\n");
                    continue;  
                }  

                printf("Введите ID письма: ");
                if (scanf("%19s", mail_id) != 1) {
                    printf("Неверный ввод для ID письма.\n");
                    continue;  
                }  

                printf("Введите время создания (dd:MM:yyyy hh:mm:ss): ");
                if (scanf("%19s", created_time) != 1) {
                    printf("Неверный ввод для времени создания.\n");
                    continue;  
                }  

                printf("Введите время вручения (dd:MM:yyyy hh:mm:ss): ");
                if (scanf("%19s", received_time) != 1) {
                    printf("Неверный ввод для времени вручения.\n");
                    continue;  
                }


                // validating data
                errCodes date_status = validate_address(town, street, house, building, apartment, index); //valiidate address
                if (date_status != SUCCESS) {
                    printf("Неверный данные для даты.\n");
                    continue; 
                }

                if (!isfinite(weight)){ // validate weight
                    printf("Некорректный вес посылки.\n");
                    continue; 
                }

                errCodes mail_id_status = validte_mail_id(mail_id); //validate mail_id
                if (mail_id_status != SUCCESS) {
                    printf("Неверный идентификатор посылки (должно быть 14 цифр).\n");
                    continue; 
                }

                struct tm* tm_t1; // validate time
                struct tm* tm_t2;
                errCodes tm_t1_status = parse_time(created_time, tm_t1);
                if (tm_t1_status != SUCCESS) {
                    printf("Ошибка задания времени создания послыки\n");
                    continue; 
                }
                errCodes tm_t2_status = parse_time(created_time, tm_t2);
                if (tm_t2_status != SUCCESS) {
                    printf("Ошибка задания времени отправки послыки\n");
                    free(tm_t1);
                    continue; 
                }

                errCodes time_compare_status = compare_time(tm_t1, tm_t2); // compare time (must be t1 < t2)
                if (time_compare_status != SUCCESS) {
                    printf("Время отправки должно быть меньше времени получения!\n");
                    free(tm_t1);
                    free(tm_t2);
                    continue; 
                }

                // creating structures 
                Address *mail_addr = create_adress(town, street, house, building, apartment, index);
                if (mail_addr != SUCCESS) {
                    printf("Ошибка создания адреса!\n");
                    free(tm_t1);
                    free(tm_t2);
                    continue; 
                }

                

                // String  *town, *street, *buildiing, *index, *mail_id, *created_time, *received_time;
                


                // Создание адреса и временных меток
                Address* address = create_adress(town, street, house, building, apartment, index);
                String* created_time_s = create_string(created_time);
                String* received_time_s = create_string(received_time);

                if (!address || !created_time_s || !received_time_s) {
                    printf("Ошибка: не удалось выделить память для данных письма.\n");
                    delete_address(address);
                    delete_string(created_time_s);
                    delete_string(received_time_s);
                    break;
                }

                // Создание письма и добавление в массив
                Mail* new_mail = create_mail(address, weight, mail_id, created_time_s, received_time_s);
                if (post_add_mail(post, new_mail) != SUCCESS) {
                    printf("Ошибка: не удалось добавить письмо.\n");
                    delete_mail(new_mail);
                } else {
                    printf("Письмо успешно добавлено.\n");
                }

                // Очистка временных данных
                delete_mail(new_mail);
                delete_address(address);
                delete_string(created_time_s);
                delete_string(received_time_s);

                break;
            }
            case 2: {
                // Удаление письма
                char mail_id[20];
                if (safe_scan_string("Введите ID письма для удаления: ", mail_id, sizeof(mail_id)) != SUCCESS || validate_mail_id(mail_id) != SUCCESS) continue;

                String* mail_id_str = create_string(mail_id);
                if (post_delete_mail(post, mail_id_str) == SUCCESS) {
                    printf("Письмо успешно удалено.\n");
                } else {
                    printf("Ошибка: письмо с таким ID не найдено.\n");
                }
                delete_string(mail_id_str);

                break;
            }
            case 3: {
                // Показать все письма
                printf("\n--- Список писем ---\n");
                for (int i = 0; i < post->mails_size; i++) {
                    printf("Письмо %d:\n", i + 1);
                    printf("  Город: %s\n", post->mails[i].address.town->text);
                    printf("  Улица: %s\n", post->mails[i].address.street->text);
                    printf("  Дом: %d\n", post->mails[i].address.house);
                    printf("  Корпус: %s\n", post->mails[i].address.building->text);
                    printf("  Квартира: %d\n", post->mails[i].address.apartment);
                    printf("  Индекс: %s\n", post->mails[i].address.index->text);
                    printf("  Вес: %.2f\n", post->mails[i].weight);
                    printf("  ID: %s\n", post->mails[i].mail_id->text);
                    printf("  Время создания: %s\n", post->mails[i].mail_created_time->text);
                    printf("  Время вручения: %s\n", post->mails[i].mail_received_time->text);
                }
                break;
            }
            case 4:
                printf("Выход из программы.\n");
                break;
            default:
                printf("Ошибка: некорректный выбор. Пожалуйста, попробуйте снова.\n");
        }
    } while (choice != 4);


    return 0;
}