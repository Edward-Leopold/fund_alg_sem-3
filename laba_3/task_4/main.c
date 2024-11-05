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

typedef struct Time{
    int t_year;
    int t_mon;
    int t_day;
    int t_hour;
    int t_min;
    int t_sec;
}Time;

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
        // delete_address(&(mail->address));
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

    // mail->address = *create_adress(addr->town->text, addr->street->text, addr->house, addr->building->text, addr->apartment, addr->index->text);

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
        delete_mail(mail);  // Clean up in case of allocation failure
        return NULL;
    }

    return mail;
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

    Mail* mail = create_mail(adr, 12.5, "458501", time_s, time_s);
    if (!mail){
        printf("mail creating error\n");
        free(t1);
    
        delete_string(time_s);
        delete_address(adr);
        return 1;
    }
    delete_mail(mail);
    free(t1);
    
    delete_string(time_s);
    delete_address(adr);

    return 0;
}