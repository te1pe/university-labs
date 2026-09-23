#include <stdio.h>
#include <stdlib.h>

typedef enum { RED, BLACK } nodeColor;

typedef struct Node {
    nodeColor color;
    int data;
    struct Node *left, *right, *parent;
} Node;

Node *Root = NULL;

void RightRotation(Node *x) {
    Node *y = x->left;
    x->left = y->right;
    if (y->right != NULL) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL) {
        Root = y;
    }
    else if (x == x->parent->right) {
        x->parent->right = y;
    }
    else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

void LeftRotation(Node *x) {
    Node *y = x->right;
    x->right = y->left;
    if (y->left != NULL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL) {
        Root = y;
    }
    else if (x == x->parent->left) {
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void FixViolation(Node *z) {
    while (z->parent && z->parent->color == RED) {
        Node *grandparent = z->parent->parent;
        if (grandparent == NULL) {
            break;
        }
        if (z->parent == grandparent->left) {
            Node *uncle = grandparent->right;
            if (uncle && uncle->color == RED) {
                z->parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    LeftRotation(z);
                }
                z->parent->color = BLACK;
                grandparent->color = RED;
                RightRotation(grandparent);
            }
        } else {
            Node *uncle = grandparent->left;
            if (uncle && uncle->color == RED) {
                z->parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    RightRotation(z);
                }
                z->parent->color = BLACK;
                grandparent->color = RED;
                LeftRotation(grandparent);
            }
        }
    }
    Root->color = BLACK;
}

int Insert(int data, Node *nodes, int *node_count) {
    Node *z = &nodes[(*node_count)++];
    z->color = RED;
    z->data = data;
    z->left = z->right = z->parent = NULL;
    Node *y = NULL;
    Node *x = Root;
    while (x != NULL) {
        y = x;
        if (z->data < x->data) {
            x = x->left;
        }
        else {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == NULL) {
        Root = z;
    }
    else if (z->data < y->data) {
        y->left = z;
    }
    else {
        y->right = z;
    }
    FixViolation(z);
    return 1;
}

int BlackHeightRBT(Node *node) {
    if (node == NULL) return 0;
    int height = BlackHeightRBT(node->right);
    return height + (node->color == BLACK ? 1 : 0);
}

int main() {
    int N;
    if (scanf("%d", &N) != 1) return 0;
    Node *nodes = NULL;
    nodes = (Node *)malloc(N * sizeof(Node));
    int node_count = 0;
    Root = NULL;
    for (int i = 0; i < N; i++) {
        int data;
        if (scanf("%d", &data) != 1) {
            free(nodes);
            return 0;
        }
        Insert(data, nodes, &node_count);
    }
    if (N == 0) {
        printf("0");
    } else {
        int blackHeight = BlackHeightRBT(Root);
        printf("%d", blackHeight + 1);
    }
    free(nodes);
    return 0;
}

