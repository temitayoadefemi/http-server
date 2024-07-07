#ifndef BST_H
#define BST_H

typedef struct Node {
    char *key;
    char *value;
    struct Node *left, *right;
} Node;

Node* insertNode(Node* node, const char* key, const char* value);
Node* search(Node* root, const char* key);

#endif // BST_H