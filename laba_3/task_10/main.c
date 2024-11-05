#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <linux/limits.h>

typedef enum errCodes {
    NOT_ENOUGH_ARGUEMENTS,
    TOO_MANY_ARGUEMENTS,
    UNABLE_TO_OPEN_FILE,
    SAME_FILE,
    WRITE_TO_FILE_ERR,
    MALLOC_ERR,
    REALL0C_ERR,
    INCORRECT_STRING,

    SUCCESS
} errCodes;  

typedef struct TreeNode {
    char value;
    struct TreeNode **children;
    int num_children;
    int children_mem_size;
} TreeNode;

typedef struct ListNode {
    TreeNode *node;
    struct ListNode *next;
} ListNode;

typedef struct Stack {
    ListNode *top;
    int size;
} Stack;

Stack* create_stack() {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (!stack) return NULL;
    stack->top = NULL; 
    stack->size = 0;

    return stack;
}

TreeNode* push_stack(Stack* stack, TreeNode* treenode) {
    ListNode* new_node = (ListNode*)malloc(sizeof(ListNode));
    if (!new_node) return NULL; 

    new_node->node = treenode;  
    new_node->next = stack->top;
    stack->top = new_node;
    stack->size++;

    return treenode;
}

TreeNode* pop_stack(Stack* stack) {
    if (stack->top == NULL) return NULL;

    ListNode* temp = stack->top;
    TreeNode* node = temp->node;
    stack->top = stack->top->next;
    free(temp);
    stack->size--;

    return node;                
}

TreeNode* top_stack(Stack* stack){
    return stack->top ? stack->top->node : NULL;
}

void free_stack(Stack* stack) {
    while (stack->top != NULL) {
        pop_stack(stack);
    }
    free(stack);
}

errCodes is_same_file(char *a, char *b){
    errCodes res = SUCCESS;
    char input_path[PATH_MAX];
    char output_path[PATH_MAX];
    realpath(a, input_path);
    realpath(b, output_path);
    int is_not_same = 0;
    
    for (int i = 0; (input_path[i] && output_path[i]); i++) {       
        if (input_path[i] != output_path[i]) is_not_same = 1;
    }   
    if (!is_not_same) res = SAME_FILE;

    return res;
}

errCodes getArgs(int argc, char** argv, char **in_file, char **out_file){
    if (argc < 3) return NOT_ENOUGH_ARGUEMENTS;
    else if(argc > 3) return TOO_MANY_ARGUEMENTS;

    errCodes files_status = is_same_file(argv[1], argv[2]);
    if (files_status != SUCCESS) return files_status;

    *in_file = argv[1];
    *out_file = argv[2];

    return SUCCESS;
}

errCodes clean_string(const char* str, char** result) {
    const int len_original = strlen(str);
    char *cleaned = (char*)malloc(sizeof(char) * (len_original + 1));
    if (!cleaned) return MALLOC_ERR;

    int open_brackets = 0;
    int need_comma = 0;
    int cnt = 0;
    char prev_char = 0;

    for (int i = 0; str[i] != '\n' && str[i] != '\0'; i++) {
        char current = str[i];

        if (isspace(current)) continue;

        if (current == '(') {
            if(!isalpha(prev_char)){
                free(cleaned);
                return INCORRECT_STRING;
            }
            open_brackets++;
            need_comma = 0;
            cleaned[cnt++] = current;
        } else if (current == ')') {
            if(prev_char == '(' || prev_char == ','){
                free(cleaned);
                return INCORRECT_STRING;
            }
            open_brackets--;
            need_comma = 1;
            cleaned[cnt++] = current;
        } else if (current == ',') {
            if (!need_comma) {
                free(cleaned);
                return INCORRECT_STRING;
            }
            need_comma = 0;
            cleaned[cnt++] = current;
        } else if (isalpha(current)) {
            if (need_comma) { 
                free(cleaned);
                return INCORRECT_STRING;
            }
            if (isalpha(prev_char)) { 
                free(cleaned);
                return INCORRECT_STRING;
            }
            cleaned[cnt++] = current;
            need_comma = 1;
        } else {
            free(cleaned);
            return INCORRECT_STRING;
        }

        prev_char = current;
    }

    if (open_brackets != 0) {
        free(cleaned);
        return INCORRECT_STRING;
    }

    cleaned[cnt] = '\0';
    *result = cleaned;

    return SUCCESS;
}

errCodes read_strings(const char* in_file, char*** result){
    FILE* input = fopen(in_file, "r");
    if (!input) return UNABLE_TO_OPEN_FILE;
    
    int str_count = 0;
    int mem_size = 10;
    char **strings = malloc(sizeof(char*) * (mem_size + 1));
    if (!strings){
        fclose(input);
        return MALLOC_ERR;
    }

    const int len_str = 5001;
    char str[len_str];
    while ((fgets(str, len_str, input)) != NULL){
        int is_empty = 1;
        for (int i = 0; str[i]; i++) {
            if (!isspace(str[i])) {
                is_empty = 0;
                break;
            }
        }
        if (is_empty) continue;

        if(str_count == mem_size){
            mem_size *= 2;
            char **temp = realloc(strings, sizeof(char*) * (mem_size + 1));
            if(!temp){
                for (int i = 0; i < str_count; i++) free(strings[i]);
                fclose(input);
                free(strings);
                return REALL0C_ERR;
            }
            strings = temp;
        }

        char *string;
        errCodes clean_status = clean_string(str, &string);
        if (clean_status != SUCCESS){
            for (int i = 0; i < str_count; i++) free(strings[i]);
            fclose(input);
            free(strings);
            return clean_status;
        }

        strings[str_count] = string;
        str_count++;
    }
    
    strings[str_count] = NULL;


    *result = strings;
    fclose(input);
    return SUCCESS;
}


TreeNode* create_node(char value) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node) {
        node->value = value;
        node->num_children = 0;
        node->children_mem_size = 2;
        node->children = (TreeNode**)malloc(sizeof(TreeNode*) * node->children_mem_size);
        if (!node->children) {
            free(node);
            return NULL;
        }
    }
    return node;
}

void add_child(TreeNode *parent, TreeNode *child) {
    if (parent->num_children == parent->children_mem_size) {
        parent->children_mem_size *= 2;
        TreeNode **new_children = (TreeNode**)realloc(parent->children, sizeof(TreeNode*) * parent->children_mem_size);
        if (!new_children) {
            printf("Realloc failed for children array.\n");
            return;
        }
        parent->children = new_children;
    }
    parent->children[parent->num_children] = child;
    parent->num_children++;
}

TreeNode* build_tree(const char *str) {
    TreeNode *root = NULL;
    TreeNode *current = NULL;
    Stack *stack = create_stack();

    if (!stack) return NULL;

    for (int i = 0; str[i] != '\0'; i++) {
        char ch = str[i];

        if (isalpha(ch)) {
            TreeNode *node = create_node(ch);
            if (!node) {
                free_stack(stack);
                return NULL;
            }

            if (current == NULL) {
                root = node;
            } else {
                add_child(current, node);
            }
            current = node;
        } else if (ch == '(') {
            TreeNode* temp = push_stack(stack, current);
            if (!temp) {
                free_stack(stack);
                return NULL;
            }
        } else if (ch == ')') {
            current = pop_stack(stack);
        } else if (ch == ',') {
            current = top_stack(stack); 
        }
    }

    free_stack(stack);
    return root;
}

void print_tree(FILE *output, TreeNode *node, int level) {
    if (!node) return;

    for (int i = 0; i < level; i++) fprintf(output, "  ");
    fprintf(output, "%c\n", node->value);

    for (int i = 0; i < node->num_children; i++) print_tree(output, node->children[i], level + 1);
}

void free_tree(TreeNode *node) {
    if (!node) return;

    for (int i = 0; i < node->num_children; i++) {
        free_tree(node->children[i]);
    }
    free(node->children);
    free(node);
}

errCodes make_trees(const char* out_file, char** strings){
    
    FILE *output = fopen(out_file, "w");
    if (!output) return UNABLE_TO_OPEN_FILE;

    for (int i = 0; strings[i]; i++) {
        TreeNode *root = build_tree(strings[i]);
        if (root == NULL) {
            fclose(output);
            return MALLOC_ERR;
        }

        fprintf(output, "Tree for string %d:\n", i + 1);
        print_tree(output, root, 0);
        fprintf(output, "\n");

        free_tree(root);
    }

    fclose(output);

    return SUCCESS;
}

int main(int argc, char** argv){
    char *in_file;
    char *out_file;

    errCodes err_status = getArgs(argc, argv, &in_file, &out_file);
    if(err_status != SUCCESS){ // handling errors from cli input
        switch (err_status){
        case NOT_ENOUGH_ARGUEMENTS:
            printf("Not enough number of argumnets.\n");
            break;
        case TOO_MANY_ARGUEMENTS:
            printf("Too many argumnets.\n");
            break;
        case SAME_FILE:
            printf("Files are the same \n");
            break;
        default:
            break;
        }
        return 1;
    }

    char **strings;
    errCodes read_status = read_strings(in_file, &strings);
    if(read_status != SUCCESS){
        switch (read_status){
        case MALLOC_ERR:
            printf("Malloc error!.\n");
            break;
        case REALL0C_ERR:
            printf("Realloc error!.\n");
            break;
        case INCORRECT_STRING:
            printf("Incorrect char in sting in input file!.\n");
            break;
        default:
            break;
        }
        return 1;
    }

    errCodes make_status = make_trees(out_file, strings);
    if(make_status != SUCCESS){
        switch (make_status){
        case MALLOC_ERR:
            printf("Malloc error!.\n");
            break;
        case REALL0C_ERR:
            printf("Realloc error!.\n");
            break;
        case INCORRECT_STRING:
            printf("Incorrect char in sting in input file!.\n");
            break;
        default:
            break;
        }
        for (int i = 0; strings[i]; i++) {
            free(strings[i]);
        }
        free(strings);
        return 1;
    }

    for (int i = 0; strings[i]; i++) {
        free(strings[i]);
    }
    free(strings);
    return 0;
}

