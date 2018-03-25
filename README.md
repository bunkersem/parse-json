# parse-json

#### Extremely simple json-parsing library

MIT licensed with no dependencies (i.e. just drop the C file into your project)

API
---
```c
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
```

Instructions
------------



Sample program:
```c
int main() {
    key_value_node *root_node = parse_json("{\"name\": \"John\"}");
    
    printf("name: %s", root_node->value[0].node_val->value->string_val);

    free_key_value_node(root_node);
}
```

Get value:

```c
node->value->{bool_val|string_val|num_val+node_val}
```

Get type of value:
```c
switch(node->type) {
    case STRING:
    // string (.string_val)
    break;
    case NUMBER:
    // double (.num_val)
    break;
    case BOOLEAN:
    // bool(unsigned short) (.bool_val)
    break;
    case NUL:
    // no value
    break;
    case OBJECT:
    // array of values (key named by prop) (.node_val[i])
    break;
    case ARRAY:
    // array of values  (key named by index) (.node_val[i])
    break;
}
```

Use the `get_prop_node(key_value_node *kvn, char *key_name)` in helpers.c to get a node by its key name
```c
int main() {
    key_value_node *root_node = parse_json("{\"name\": \"John\"}");
    
    printf("name: %s", get_prop_node(root_node, "name")->value->string_val);

    free_key_value_node(root_node);
}
```

helper functions (helper.c):

`get_prop_node` gets the child node of a branch node by name.

`parse_json_file` parses a json file

`print_key_value_tree` prints the node tree to stdout

`free_key_value_node` frees a node (does not have to be the root node)