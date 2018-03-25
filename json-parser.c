#include "json-parser.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

typedef enum { WAITFORKEY, BEFOREVALUEWRITE, AFTERVALUEWRITE } writing_state;
typedef enum { WM_INITIAL, WM_OBJECT, WM_ARRAY } writing_mode;

void *parse_err(const char *fmt, char s, unsigned short i) {
    s != '\0' ? fprintf(stderr, fmt, s, i) : fprintf(stderr, fmt);
    return NULL;
}

char *substring(const char *str, size_t begin, size_t len) {
    char *subbuff = malloc(len + 1);
    memcpy(subbuff, &str[begin], len);
    subbuff[len] = '\0';
    return subbuff;
}

unsigned short str_slice_eq(const char *str, size_t begin, size_t len, const char *target) {
    char* sub = substring(str, begin, len);
    int r = strcmp(sub, target);
    free(sub);
    return r == 0;
}

key_value_node *create_key_value_node(json_type type, writing_mode mode, char *key, key_value_node *curr) {
    key_value_node *new_node = malloc(sizeof(key_value_node));
    new_node->type = type;
    if (mode == WM_ARRAY) sprintf(key, "%lu", curr->val_len);
    new_node->key = malloc(strlen(key) + 1);
    strcpy(new_node->key, key);
    new_node->parent = curr;
    new_node->val_len = type == OBJECT || type == ARRAY ? 0 : 1;
    new_node->value = malloc(sizeof(key_value) * (type == OBJECT || type == ARRAY ? MAX_VALUES_SIZE : 1));
    return new_node;
}

key_value_node *parse_json(char *json_buf) {
    unsigned long len = strlen(json_buf);
    key_value_node *curr = NULL;
    writing_state ws = BEFOREVALUEWRITE;
    writing_mode mode = WM_INITIAL;
    char s;
    unsigned short key_write_index;
    char key[MAX_KEY_SIZE] = "";

    for (unsigned int i = 0; i < len; i++) {
        s = json_buf[i];
        switch (ws) {
        case AFTERVALUEWRITE:
            if (s == ',') ws = mode == WM_ARRAY ? BEFOREVALUEWRITE : WAITFORKEY;
            else if (s == '}' || s == ']') {
                if (mode == WM_ARRAY && s == '}') return parse_err("syntax error character '%c'(#%i): expected ']'", s, i);
                else if (mode == WM_OBJECT && s == ']') return parse_err("syntax error character '%c'(#%i): expected '}'", s, i);
                else if (curr->parent != NULL) { // close child
                    ws = AFTERVALUEWRITE;
                    curr = curr->parent;
                    mode = curr->type == ARRAY ? WM_ARRAY : WM_OBJECT;
                } else return curr; // we are done, and back at the root node
            }
            break;
        case BEFOREVALUEWRITE:
            if (isspace(s)) continue;
            if (s == '"') {
                s = json_buf[++i];
                unsigned int start = i;
                while (json_buf[i] != '"' || (i && json_buf[i - 1] == '\\'))
                    if (++i >= len) return parse_err("syntax error: unexpected end of string, expected a character in a string value", '\0', 0);
                s = json_buf[i];
                char *val = substring(json_buf, start, i - start);
                key_value_node *str_node = create_key_value_node(STRING, mode, key, curr);
                str_node->value[0].string_val = val;
                if(curr != NULL) curr->value[curr->val_len++].node_val = str_node;
                else curr = str_node;
                ws = AFTERVALUEWRITE;
            }
            else if (isdigit(s) || s == '.' || s == '-') {
                unsigned int start = i;
                while (isdigit(s) || s == '.') { // TODO error check while loops
                    if (++i >= len) return parse_err("syntax error: unexpected end of string, expected a character inside a number value", '\0', 0);
                    else s = json_buf[i];
                }
                char *strval = substring(json_buf, start, --i - start + 1);
                key_value_node *num_node = create_key_value_node(NUMBER, mode, key, curr);
                num_node->value[0].num_val = strtod(strval, NULL);
                free(strval);
                if (curr != NULL) curr->value[curr->val_len++].node_val = num_node;
                else curr = num_node;
                ws = AFTERVALUEWRITE;
            }
            else if ((i + 4) <= len && str_slice_eq(json_buf, i, 4, "true")) {
                key_value_node *bool_node = create_key_value_node(BOOLEAN, mode, key, curr);
                bool_node->value[0].bool_val = 1;
                i = i + 3;
                if (curr != NULL) curr->value[curr->val_len++].node_val = bool_node;
                else curr = bool_node;
                ws = AFTERVALUEWRITE;
            }
            else if ((i + 5) <= len && str_slice_eq(json_buf, i, 5, "false")) {
                key_value_node *bool_node = create_key_value_node(BOOLEAN, mode, key, curr);
                bool_node->value[0].bool_val = 0;
                i = i + 4;
                if (curr != NULL) curr->value[curr->val_len++].node_val = bool_node;
                else curr = bool_node;
                ws = AFTERVALUEWRITE;
            } else if ((i + 4) <= len && str_slice_eq(json_buf, i, 4, "null")) { // value is null
                key_value_node *null_node = create_key_value_node(NUL, mode, key, curr);
                null_node->value = NULL;
                i = i + 3;
                if (curr != NULL) curr->value[curr->val_len++].node_val = null_node;
                else curr = null_node;
                ws = AFTERVALUEWRITE;
            } else if (s == '{') {
                key_value_node *new_obj_node = create_key_value_node(OBJECT, mode, key, curr);
                if (curr != NULL) curr->value[curr->val_len++].node_val = new_obj_node;
                curr = new_obj_node;
                mode = WM_OBJECT;
                ws = WAITFORKEY;
            } else if (s == '[') {
                key_value_node *new_arr_node = create_key_value_node(ARRAY, mode, key, curr);
                if (curr != NULL) curr->value[curr->val_len++].node_val = new_arr_node;
                curr = new_arr_node;
                mode = WM_ARRAY;
                ws = BEFOREVALUEWRITE;
            } else return parse_err("syntax error: unexpected character '%c'(#%i). expected a value", s, i);
            break;
        case WAITFORKEY:
            if (isspace(s)) continue;
            if (s != '"') return parse_err("syntax error: unexpected character '%c'(#%i). expected a '\"' character", s, i);
            key_write_index = 0;
            i++;
            while (1) {
                if (i >= len) return parse_err("syntax error: unexpected end of string, expected a key character", '\0', 0);
                if (json_buf[i] == '"' && json_buf[i - 1] != '\\')  break;  // done writing title string
                else key[key_write_index++] = json_buf[i++];
            }
            key[key_write_index] = '\0';
            while (1) {
                if (i >= len) return parse_err("syntax error: unexpected end of string, expected a ':' character", '\0', 0);
                if (isspace(json_buf[++i])) continue;
                if (json_buf[i] == ':') break;
                else return parse_err("syntax error: unexpected character '%c'(#%i). expected a ':' character", s, i);
            }
            ws = BEFOREVALUEWRITE;
            break;
        }
    }
    if (curr != NULL && curr->parent == NULL) return curr; // this json string holds a single value
    return parse_err("syntax error", '\0', 0);
}
