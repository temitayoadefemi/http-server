#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst.h"

Node* root = NULL;

Node* createNode(const char* key, const char* value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = strdup(key);
    newNode->value = strdup(value);
    newNode->left = newNode->right = NULL;
    return newNode;
}

Node* insertNode(Node* node, const char* key, const char* value) {
    if (node == NULL) return createNode(key, value);

    if (strcmp(key, node->key) < 0) {
        node->left = insertNode(node->left, key, value);
    } else if (strcmp(key, node->key) > 0) {
        node->right = insertNode(node->right, key, value);
    }

    return node;
}

Node* search(Node* root, const char* key) {
    if (root == NULL || strcmp(root->key, key) == 0) {
        return root;
    }

    if (strcmp(key, root->key) < 0) {
        return search(root->left, key);
    } else {
        return search(root->right, key);
    }
}
