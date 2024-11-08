#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "strings.h"

typedef enum errCodes{
    MALLOC_ERR,
    REALLOC_ERR,
    DATE_FORMAT_ERR,
    MAIL_NOT_FOUND,
    ADDRESS_INPUT_ERR,
    MAIL_ID_ERR,

    SUCCESS
}errCodes;


typedef struct Liver{
    String surname;
    String name;
    String patronymic;
    String birthday;
    char gender;
    double salary;
}Liver;

typedef struct Node {
	Liver liver;
	struct Node* next;
} Node;

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







int main(){
    


    return 0;
}