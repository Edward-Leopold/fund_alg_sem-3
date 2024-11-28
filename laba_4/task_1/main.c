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
    HASH_INSERT_ERR,

    SUCCESS
} errCodes;

typedef struct HashItem{
    char* key;
    char* value;
    struct HashItem* next;
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

    item->key = (char*)malloc(sizeof(char) * (strlen(key) + 1));
    if(item->key == NULL){
        free(item);
        return NULL;
    }
    item->value = (char*)malloc(sizeof(char) * (strlen(value) + 1));
    if(item->value == NULL){
        free(item);
        free(key);
        return NULL;
    }

    strcpy(item->key, key);
    strcpy(item->value, value);
    item->next = NULL;

    return item;
}

void push_item(HashItem** head, HashItem* item){
    if (!(*head)) {
        *head = item;
        return;
    }

    HashItem* current = *head;
    while(current->next){
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

void delete_item(HashItem* item){
    if(item) {
        free(item->key);
        free(item->value);
        free(item);
    }
}

int list_len(HashItem* head){
    int cnt = 0;

    if (head == NULL){
        return cnt;
    }
    HashItem* cur = head;
    cnt++;
    while (cur->next){
        cnt++;
        cur = cur->next;
    }

    return cnt;
}

void delete_items(HashItem* head){
    HashItem* lastitem;
    while(head){
        lastitem = pop_item(&head);
        delete_item(lastitem);
    }
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
        free(table->items);
    }
}

void print_table(HashTable* table) {
    if (!table || !table->items) {
        printf("Hash table is empty or uninitialized.\n");
        return;
    }

    for (int i = 0; i < table->size; i++) {
        printf("Bucket %d:\n", i);
        HashItem* current = table->items[i];
        while (current) {
            printf("  Key: %s, Value: %s\n", current->key, current->value);
            current = current->next;
        }
    }
}


size_t hash_function(char* key){
    size_t num = 0;
    int len = strlen(key);
    for (int i = 0; i < len; i++) {
        if (key[i] >= '0' && key[i] <= '9'){
            num = num * 62 + key[i] - '0';
        }
        else if (key[i] >= 'A' && key[i] <= 'Z'){
            num = num * 62 + key[i] - 'A' + 10;
        }
        else if (key[i] >= 'a' && key[i] <= 'z'){
            num = num * 62 + key[i] - 'a' + 36;
        }
    }
    return num;
}

errCodes insert_table(HashTable* table, char* key, char* value, int hash){
    int index = hash % table->size;
    HashItem* item = create_item(key, value);
    if (!item){
        return MALLOC_ERR;
    }
    push_item(&(table->items[index]), item);
    return SUCCESS;
}

HashTable* refactor_table(HashTable* oldtable, int new_hashsize){
    HashTable* table = create_table(new_hashsize);
    if (!table) return NULL;

    for (int i = 0; i < oldtable->size; i++){
        while(oldtable->items[i]){
            HashItem* item = pop_item(&(oldtable->items[i]));
            char* key = item->key;
            char* value = item->value;
            delete_item(item);
            if (insert_table(table, key, value, hash_function(key)) != SUCCESS){
                delete_table(table);
                return NULL;
            }
        }
    }

    return table;
}

int is_need_rebuild(HashTable *table){
    int max = -1;
    int min = table->size + 1;

    for (int i = 0; i < table->size; i++){
        if (table->items[i]){
            int cur_len = list_len(table->items[i]); 
            max = (cur_len > max) ? cur_len : max;
            min = (cur_len < min) ? cur_len : min;  
        }
    }

    if (max / min >= 2){
        return 1;
    }
    return 0;
}

void next_lexem(int* c, FILE* f){
    while((*c = fgetc(f)) != EOF){
        if(!(*c == ' ' || *c == '\t' || *c == '\n')) break;
    }
    fseek(f, -1, SEEK_CUR);
}

errCodes read_str(FILE* input, char** buffer, int* c){
    int capacity = 20;
    int idx = 0;
    char* str = (char*)malloc(capacity * sizeof(char));
    if (!str){ 
        return MALLOC_ERR;
    }
    while((*c = fgetc(input)) != EOF){
        if (isspace(*c)) {
            break;
        }
        str[idx++] = *c;
        if(capacity == idx){
            capacity *= 2;
            char* temp = (char*)realloc(str, capacity * sizeof(char));
            if(temp == NULL){
                free(str);
                return REALLOC_ERR;
            }
            str = temp;
        }
    }
    if (idx == 0 && *c == EOF) { 
        free(str);
        return MALLOC_ERR;
    }
    if (*c != EOF) fseek(input, -1, SEEK_CUR);
    
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
        if (read_str(file, &str, c) != SUCCESS) { // smth went wrong
            free(key);
            free(value);
            return MALLOC_ERR;
        }
        next_lexem(c, file);

        if (strcmp(str, "#define") == 0) {
            free(str);
            
            if (read_str(file, &key, c) != SUCCESS) {
                free(key);
                free(value);
                return MALLOC_ERR;
            }
            next_lexem(c, file);
            // strcpy(key, str); 
            // key = str;
            // free(str);

            if (read_str(file, &value, c) != SUCCESS) {
                free(key);
                free(value);
                return MALLOC_ERR;
            }
            // next_lexem(c, file);
            // strcpy(value, str);
            // value = str;
            // free(str);

            *c = fgetc(file);
            while (*c == ' ' || *c == '\t') {
                *c = fgetc(file);
            }
            if (*c != '\n' && *c != EOF) {
                fseek(file, start_pos, SEEK_SET); 
                break;
            }

            // adding key value to hashtable
            printf("%s %s\n", key, value);
            if (insert_table(*table, key, value, hash_function(key)) != SUCCESS) {
                free(key);
                free(value);
                return HASH_INSERT_ERR;
            }
            free(key);
            free(value);
        } else {
            fseek(file, start_pos, SEEK_SET);
            break;
        }
    }

    // free(key);
    // free(value);
    return SUCCESS;
}

char* find_value(HashTable* table, char* key) {
    if (!table || !table->items || !key) {
        return NULL;
    }

    int index = hash_function(key) % table->size;
    HashItem* current = table->items[index];
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return NULL; // value not found
}

errCodes process_text(FILE* file, HashTable* table, int *c, FILE* output) {
    char* buffer = NULL;

    while ((*c = fgetc(file)) != EOF) {
        if (!isspace(*c)) { 
            fseek(file, -1, SEEK_CUR); 
            if (read_str(file, &buffer, c) != SUCCESS) {
                if (buffer) free(buffer);
                return MALLOC_ERR; 
            }
            char* replacement = find_value(table, buffer);
            if (replacement) {
                fprintf(output, "%s", replacement); 
            } else {
                fprintf(output, "%s", buffer); 
            }
            free(buffer);
            buffer = NULL;
        } else {
            fputc(*c, output); 
        }
    }
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

    HashTable* table = create_table(HASHSIZE);  
    if (!table) {
        fclose(file);
        printf("Unable to create hash table.\n");
        return 1;
    }

    int c = 0;
    errCodes define_status = read_define(file, &c, &table);
    if (define_status != SUCCESS) {
        printf("Error reading #define directives. Error code: %d\n", define_status);
        delete_table(table);
        fclose(file);
        return 1;
    }

    FILE* output = fopen("output.txt", "w");
    if (!output) {
        printf("Unable to create output file.\n");
        delete_table(table);
        fclose(file);
        return 1;
    }

    print_table(table);

    errCodes process_status = process_text(file, table, &c, output);
    if (process_status != SUCCESS) {
        printf("Error processing text. Error code: %d\n", process_status);
        delete_table(table);
        fclose(file);
        fclose(output);
        return 1;
    }

    printf("Processing completed successfully.\n");
    delete_table(table);
    fclose(file);
    fclose(output);
    return 0;
}