#include <stdio.h>
#include <time.h>
#include "strings.h"

typedef enum errCodes{
    MALLOC_ERR,
    REALLOC_ERR,
    DATE_FORMAT_ERR,

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


int main(){
    Address *adr = create_adress("Moscow", "Velikanova", 14, "1", 239, "345029");
    printf("%s\n", adr->street->text);
    // delete_address(adr);

    char* timestr = "1:1:2024                              01:1200:12";
    struct tm* t1;
    errCodes time_status = parse_time(timestr, &t1);
    if (time_status != SUCCESS) {
        switch (time_status){
        case MALLOC_ERR:
            printf("malloc error\n");
            break;
        case DATE_FORMAT_ERR:
            printf("something went wrong with date\n");
            break;
        default:
            break;
        }
        return 1;
    }
    
    
    String *time_s = stringify_time(t1);
    if (!time_s){
        printf("malloc error\n");
        return 1;
    }
    printf("your date is %s\n", time_s->text);

    Mail *mail1 = create_mail(adr, 12.5, "458501", time_s, time_s);
    Mail *mail2 = create_mail(adr, 22.5, "458201", time_s, time_s);
    Mail *mail3 = create_mail(adr, 13.5, "458661", time_s, time_s);
    Post *post = create_post(adr);
    post_add_mail(post, mail1);
    post_add_mail(post, mail2);
    post_add_mail(post, mail3);

    delete_post(post);
    delete_mail(mail1);
    delete_mail(mail2);
    delete_mail(mail3);
    free(t1);
    
    delete_string(time_s);
    // delete_address(adr);

    return 0;
}