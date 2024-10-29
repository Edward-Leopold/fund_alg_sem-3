#pragma once

typedef enum errorCodes{
    NOT_ENOUGH_ARGUEMENTS,
    TOO_MANY_ARGUEMENTS,
    MALLOC_ERR, 
    REALLOC_ERR,
    WRITE_TO_FILE_ERR,
    INVALID_SYMBOL,
    UNABLE_TO_OPEN_FILE,
    SAME_FILE,
    UNKNOWN_FLAG,
    NOT_A_FLAG,

    SUCCESS
} errorCodes;