
#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#define MAX_VALUES_SIZE 1000
#define MAX_KEY_SIZE 1024

typedef enum { NUMBER, NUL, STRING, BOOLEAN, ARRAY, OBJECT } json_type;

union key_value;

typedef struct key_value_node {
    json_type type;
    union key_value* value;
    unsigned long val_len;
    char* key;
    struct key_value_node *parent;
} key_value_node;

typedef union key_value {
    unsigned short bool_val;
    char* string_val;
    double num_val;
    key_value_node *node_val;
} key_value;

key_value_node *parse_json(char *json_buf);

#endif

