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

    item->key = key;
    item->value = value;
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
    if(item) free(item);
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

int hash_function(char* key){
    int num = 0;
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

errCodes read_str(FILE* input, char** buffer, int* c){
    int capacity = 20;
    int idx = 0;
    char* str = (char*)malloc(capacity * sizeof(char));
    if (!str){ 
        return MALLOC_ERR;
    }
    while((*c = fgetc(input)) != EOF){
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

            // adding key value to hashtable
            if (insert_table(*table, key, value, hash_function(key)) != SUCCESS) {
                free(key);
                free(value);
                return HASH_INSERT_ERR;
            }
        } else {
            fseek(file, start_pos, SEEK_SET);
            break;
        }
    }

    free(key);
    free(value);
    return SUCCESS;
}

char* find_value(HashTable* table, char* key) {
    int index = hash_function(key) % table->size;
    HashItem* current = table->items[index];
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value; // Возвращаем найденное значение
        }
        current = current->next;
    }
    return NULL; // Значение по ключу не найдено
}

errCodes process_text(FILE* file, HashTable* table, int* c, FILE* output) {
    int capacity = 1024; // Начальный размер буфера строки
    char* buffer = malloc(capacity * sizeof(char));
    if (!buffer) return MALLOC_ERR;

    int idx = 0;

    while ((*c = fgetc(file)) != EOF) {
        if (isalnum(*c)) {
            // Начинаем считывать слово
            int word_capacity = 128, word_idx = 0;
            char* word = malloc(word_capacity * sizeof(char));
            if (!word) {
                free(buffer);
                return MALLOC_ERR;
            }

            do {
                word[word_idx++] = *c;
                if (word_idx == word_capacity) {
                    word_capacity *= 2;
                    char* temp = realloc(word, word_capacity);
                    if (!temp) {
                        free(word);
                        free(buffer);
                        return REALLOC_ERR;
                    }
                    word = temp;
                }
                *c = fgetc(file);
            } while (*c != EOF && isalnum(*c));

            word[word_idx] = '\0';

            // Проверяем, есть ли замена в хэш-таблице
            char* replacement = find_value(table, word);
            const char* to_write = replacement ? replacement : word;

            // Пишем слово в буфер
            for (int i = 0; to_write[i] != '\0'; i++) {
                buffer[idx++] = to_write[i];
                if (idx == capacity) {
                    capacity *= 2;
                    char* temp = realloc(buffer, capacity);
                    if (!temp) {
                        free(word);
                        free(buffer);
                        return REALLOC_ERR;
                    }
                    buffer = temp;
                }
            }

            free(word);

            // Возвращаем неалфавитный символ обратно в обработку
            if (*c != EOF) {
                buffer[idx++] = *c;
                if (idx == capacity) {
                    capacity *= 2;
                    char* temp = realloc(buffer, capacity);
                    if (!temp) {
                        free(buffer);
                        free(word);
                        return REALLOC_ERR;
                    }
                    buffer = temp;
                }
            }
        } else {
            // Добавляем любые неалфавитные символы, включая пробелы, как есть
            buffer[idx++] = *c;
            if (idx == capacity) {
                capacity *= 2;
                char* temp = realloc(buffer, capacity);
                if (!temp) {
                    free(buffer);
                    return REALLOC_ERR;
                }
                buffer = temp;
            }
        }
    }

    buffer[idx] = '\0'; // Завершаем строку

    // Пишем весь буфер в выходной файл
    fprintf(output, "%s", buffer);
    free(buffer);

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