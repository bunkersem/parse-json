#include <stdio.h>
#include <stdlib.h>
#include "json-parser.h"
#include "helpers.h"

int main() {

    printf("parse-json");

    printf("\n\nExample 1:\n\n");
    
    key_value_node *root_node = parse_json_file("example.json");
    print_key_value_tree(root_node, 1);
    free_key_value_node(root_node);

    printf("\n\nExample 2:\n\n");
    
    root_node = parse_json_file("example2.json");
    print_key_value_tree(root_node, 0);
    free_key_value_node(root_node);
    
    getchar();

    return 0;
}
