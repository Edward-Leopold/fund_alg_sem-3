#include <stdio.h>
#include <time.h>
#include "strings.h"

typedef enum errCodes{
    MALLOC_ERR,
    REALLOC_ERR,

    
    SUCCESS
}errCodes;

typedef struct Adress{
    String town;
    String street;
    int house;
    String building;
	int apartment;
	String index;
}Adress;

typedef struct Mail{
    Adress adress;
    double weight;
    String mail_id;
    String mail_created_time;
    String mail_received_time;
}Mail;

typedef struct Post{
    Adress* address;
    Mail* mails;
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


int main(){
    String *s = create_string("Hello, world!");
    printf("%s\n", s->text);
    delete_string(s);

    return 0;
}