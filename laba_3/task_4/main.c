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
        // *timestruct = NULL;
        return DATE_FORMAT_ERR;
    }

    *timestruct = ts;

    return SUCCESS;
}


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



int main(){
    // Address *adr = create_adress("Moscow", "Velikanova", 14, "1", 239, "345029");
    // printf("%s\n", adr->street->text);
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
    
    
    String *s = stringify_time(t1);
    if (!s){
        printf("malloc error\n");
        return 1;
    }
    printf("your date is %s\n", s->text);
    free(t1);
    delete_string(s);

    return 0;
}