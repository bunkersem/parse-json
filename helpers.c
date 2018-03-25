
#include "helpers.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

key_value_node *get_prop_node(key_value_node *branch_root, char *key_name) {
    if (branch_root->type != OBJECT) return NULL;
    for (unsigned long i = 0; i < branch_root->val_len; i++)
        if (branch_root->value[i].node_val != NULL && strcmp(branch_root->value[i].node_val->key, key_name) == 0) 
            return branch_root->value[i].node_val;
    return NULL;
}

key_value_node *parse_json_file(char *filename) {
    char *str = NULL;
    long length;
    FILE *f = fopen(filename, "r");
    if (f == NULL) return NULL;
    else {
        if (fseek(f, 0, SEEK_END)) return NULL;
        length = ftell(f);
        if (fseek(f, 0, SEEK_SET)) return NULL;
        str = malloc(length + sizeof(char));
        if (str) fread(str, 1, length, f);
        else return NULL;
        str[length] = '\0';
        if (fclose(f)) return NULL;
    }
    key_value_node *root = parse_json(str);
    free(str);
    return root;
}

int print_key_value_tree_rec(key_value_node *elem, unsigned short scope) {
    static const char *strings[] = {"number", "null", "string", "boolean", "array", "object"};
    if (elem->key != NULL && strlen(elem->key) > 0)
    {
        if (scope > 0) printf("%*c", scope * INDENTATION_SPACES, ' ');
        printf("key: %s\n", elem->key);
    }
    if (scope > 0)
        printf("%*c", scope * INDENTATION_SPACES, ' ');
    printf("type: %s\n", strings[elem->type]);
    if (elem->type == NUL)
    {
        if (scope > 0) printf("%*c", scope * INDENTATION_SPACES, ' ');
        printf("value: null\n");
    }
    else if (elem->type == BOOLEAN)
    {
        if (scope > 0) printf("%*c", scope * INDENTATION_SPACES, ' ');
        printf("value: %s\n", elem->value[0].bool_val ? "true" : "false");
    }
    else if (elem->type == STRING)
    {
        if (scope > 0) printf("%*c", scope * INDENTATION_SPACES, ' ');
        printf("value: %s\n", elem->value[0].string_val);
    }
    else if (elem->type == NUMBER)
    {
        if (scope > 0) printf("%*c", scope * INDENTATION_SPACES, ' ');
        printf("value: %.6f\n", elem->value[0].num_val);
    }
    else if (elem->type == OBJECT || elem->type == ARRAY)
    {
        if (scope > 0) printf("%*c", scope * INDENTATION_SPACES, ' ');
        printf("value:\n");
        for (unsigned int i = 0; i < elem->val_len; i++)
            print_key_value_tree_rec(elem->value[i].node_val, scope + 1);
    }
    return 0;
}

int print_compact_key_value_tree_rec(key_value_node *elem, unsigned short scope) {
    if (elem->key != NULL && strlen(elem->key) > 0)
    {
        if (scope > 0) printf("%*c", scope * INDENTATION_SPACES, ' ');
        printf("%s: ", elem->key);
    }
    if (elem->type == NUL)
        printf("null\n");
    else if (elem->type == BOOLEAN)
        printf("%s\n", elem->value[0].bool_val ? "true" : "false");
    else if (elem->type == STRING)
        printf("%s\n", elem->value[0].string_val);
    else if (elem->type == NUMBER)
        printf("%.6f\n", elem->value[0].num_val);
    else if (elem->type == OBJECT || elem->type == ARRAY)
    {
        puts("");
        for (unsigned int i = 0; i < elem->val_len; i++)
            if (elem->value[i].node_val != NULL) 
                print_compact_key_value_tree_rec(elem->value[i].node_val, scope + 1);
    }
    return 0;
}

int print_key_value_tree(key_value_node *root, unsigned short iscompact) {
    iscompact ? print_compact_key_value_tree_rec(root, 0) : print_key_value_tree_rec(root, 0);
    return 0;
}

int free_key_value_node(key_value_node *node) {
    if (node->type == OBJECT || node->type == ARRAY) {
        for (unsigned long i = 0; i < node->val_len; i++) {
            if (node->value[i].node_val != NULL) {
                free_key_value_node(node->value[i].node_val);
                free(node->value[i].node_val);
            }
        }
    } else if (node->type == STRING)
        free(node->value->string_val);
    free(node->value);
    free(node->key);
    if (node->parent != NULL) {
        // if (node->parent->type == OBJECT || node->parent->type == ARRAY) {
        //     for (unsigned long i = 0; i < node->parent->val_len; i++)
        //     if (node->parent->value[i].node_val != NULL && strcmp(node->parent->value[i].node_val->key, node->key) == 0) 
        //         node->parent->value[i].node_val = NULL; // set pointer of parent to null
        // }
    }
    if (node->parent == NULL)
        free(node);
    return 0;
}


