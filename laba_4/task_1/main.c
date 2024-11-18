#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <linux/limits.h>

#define HASHSIZE 128

typedef enum errCodes{
    NOT_ENOUGH_ARGUEMENTS,
    TOO_MANY_ARGUEMENTS,
    MALLOC_ERR,
    REALLOC_ERR,
    FILE_POS_ERR,
    SAME_FILE_ERR,
    UNABLE_TO_OPEN_FILE,

    SUCCESS
} errCodes;

typedef struct HashItem{
    char *key;
    char *value;
    HashItem *next;
}HashItem;


typedef struct HashTable{
    int size;   
    HashItem** items;
}HashTable;

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

errCodes getArgs(int argc, char** argv, char** filename){
    if (argc < 2) return NOT_ENOUGH_ARGUEMENTS;
    else if(argc > 2) return TOO_MANY_ARGUEMENTS;

    *filename = argv[1];

    return SUCCESS;
}

HashItem* create_item(char* key, char* value){
    HashItem *item = malloc(sizeof(HashItem));
    if (!item) return NULL;

    item->key = key;
    item->value = value;
    item->next = NULL;

    return item;
}

void delete_item(HashItem* item){
    if(item) free(item);
}

void delete_items(HashItem* head){
    HashItem* lastitem;
    while(head){
        lastitem = pop_item(&head);
        delete_item(lastitem);
    }
}

void push_item(HashItem** head, HashItem* item){
    if (!(*head)) {
        *head = item;
    }

    HashItem* current = head;
    while(current){
        current = current->next;
    }

    current->next = item;
}

HashItem* pop_item(HashItem** head){
    if(*head == NULL){
        return NULL;
    }
    HashItem *item = *head;
    *head = (*head)->next;  
    return item;
}

HashTable* create_table(int size){
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    if (!table) return NULL;

    table->items = (HashItem**)malloc(size * sizeof(HashItem*));
    if (!table->items){
        free(table);
        return NULL;
    }

    table->size = size;
    for (int i = 0; i < table->size; i++) table->items[i] = NULL;

    return table;
}

void delete_table(HashTable* table){
    if (table){
        for (int i = 0; i < table->size; i++){
            if (table->items[i]){
                delete_items(table->items[i]);
            }
        }
        free(table->size);
    }
}

errCodes insert_table(HashTable* table, char* key, char* value){
    
}

errCodes read_str(FILE* input, char** buffer, int* c){
    int capacity = 20;
    int idx = 0;
    char* str = (char*)malloc(capacity * sizeof(char));
    if (!str){ 
        return MALLOC_ERR;
    }
    while((*c = fgetc(input) != EOF)){
        str[idx++] = *c;
        if(capacity == idx){
            capacity *= 2;
            char* temp = (char*)realloc(str, capacity * sizeof(char));
            if(temp == NULL){
                return REALLOC_ERR;
            }
            str = temp;
        }
    }
    str[idx] = '\0';
    *buffer = str;
    return SUCCESS;
}

errCodes read_define(FILE * file, int *c, HashTable** table){
   int capacity_key = 20, capacity_value = 20;
    char* key = (char*)malloc(capacity_key * sizeof(char));
    if (key == NULL) {
        return MALLOC_ERR;
    }
    char* value = (char*)malloc(capacity_value * sizeof(char));
    if (value == NULL) {
        free(key);
        return MALLOC_ERR;
    }

    while (!feof(file)) {
        long start_pos = ftell(file);
        if (start_pos == -1L) {
            free(key);
            free(value);
            return FILE_POS_ERR;
        }

        char* str;
        if (read_str(file, &str, c) != SUCCESS) {
            free(key);
            free(value);
            return MALLOC_ERR;
        }

        if (strcmp(str, "#define") == 0) {
            free(str);
            
            if (read_str(file, &str, c) != SUCCESS) {
                free(key);
                free(value);
                return MALLOC_ERR;
            }
            strcpy(key, str); 
            free(str);

            if (read_str(file, &str, c) != SUCCESS) {
                free(key);
                free(value);
                return MALLOC_ERR;
            }
            strcpy(value, str);
            free(str);

            *c = fgetc(file);
            while (*c == ' ' || *c == '\t') {
                *c = fgetc(file);
            }
            if (*c != '\n' && *c != EOF) {
                fseek(file, start_pos, SEEK_SET); 
                break;
            }

            // Добавляем пару key-value в хэш-таблицу
            // if (insert(table, key, value, hash_function(key)) != SUCCESS) {
            //     free(key);
            //     free(value);
            //     return HASH_INSERT_ERR;
            // }
        } else {
            fseek(file, start_pos, SEEK_SET);
            break;
        }
    }

    free(key);
    free(value);
    return SUCCESS;
}



int main(int argc, char** argv){
    char* filename = NULL;
    errCodes args_status = getArgs(argc, argv, &filename);
    if (args_status != SUCCESS){ // handling errors from cli input
        switch (args_status){
        case NOT_ENOUGH_ARGUEMENTS:
            printf("Not enough number of argumnets.\n");
            break;
        case TOO_MANY_ARGUEMENTS:
            printf("Too many argumnets.\n");
            break;
        default:
            break;
        }
        return 1;
    }

    FILE* file = fopen(filename, "r");
    if (!file){
        printf("Unable to open file\n");
        return 1;
    }




    fclose(file);
    return 0;
}