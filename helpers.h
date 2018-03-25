
#ifndef __JSON_PARSER_HELPERS_H__
#define __JSON_PARSER_HELPERS_H__

#include "json-parser.h"

#define INDENTATION_SPACES 4

key_value_node *get_prop_node(key_value_node *kvn, char *key_name);

key_value_node *parse_json_file(char *filename);

int print_key_value_tree(key_value_node *root, unsigned short iscompact);

int free_key_value_node(key_value_node *node);

#endif



