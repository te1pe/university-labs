#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int value;
    unsigned char height;
    struct Node *left;
    struct Node *right;
} Node;

unsigned char GetHeight(Node *current) {
    return current ? current->height : 0;
}

int CalculateBalanceFactor(Node *current) {
    return GetHeight(current->left) - GetHeight(current->right);
}

void FixHeight(Node *current) {
    unsigned char height_left = GetHeight(current->left);
    unsigned char height_right = GetHeight(current->right);
    current->height = (height_left > height_right ? height_left : height_right) + 1;
}

Node *LeftRotation(Node *current) {
    Node *right = current->right;
    current->right = right->left;
    right->left = current;
    FixHeight(current);
    FixHeight(right);
    return right;
}

Node *RightRotation(Node *current) {
    Node *left = current->left;
    current->left = left->right;
    left->right = current;
    FixHeight(current);
    FixHeight(left);
    return left;
}

Node *BalanceTree(Node *current, int N) {
    FixHeight(current);
    if (CalculateBalanceFactor(current) == -2) {
        if (CalculateBalanceFactor(current->right) > 0) {
            if (N < 100) printf("BL ");
            current->right = RightRotation(current->right);
            return LeftRotation(current);
        } else {
            if (N < 100) printf("SL ");
            return LeftRotation(current);
        }
    }

    if (CalculateBalanceFactor(current) == 2) {
        if (CalculateBalanceFactor(current->left) < 0) {
            if (N < 100) printf("BR ");
            current->left = LeftRotation(current->left);
            return RightRotation(current);
        } else {
            if (N < 100) printf("SR ");
            return RightRotation(current);
        }
    }

    return current;
}

Node *Insertion(Node *current, int value, int N, Node *avl_nodes, int *node_counter) {
    if (!current) {
        Node *new_node = &avl_nodes[(*node_counter)++];
        new_node->value = value;
        new_node->height = 1;
        new_node->left = new_node->right = NULL;
        return new_node;
    }
    if (value > current->value) {
        current->right = Insertion(current->right, value, N, avl_nodes, node_counter);
    } else {
        current->left = Insertion(current->left, value, N, avl_nodes, node_counter);
    }
    return BalanceTree(current, N);
}

int main() {
    int N = 0;
    if (scanf("%d", &N) != 1) {printf("Empty input");return 0;}
    if (N < 0 || N > 2000000) {printf("Bad input");return 0;}
    Node *root = NULL;
    Node *avl_nodes = (Node *)malloc(N * sizeof(Node));
    if (!avl_nodes) {return 0;}  
    int node_counter = 0;
    for (int i = 0; i < N; i++) {
        int value;
        if (scanf("%d", &value) != 1) {
            printf("Bad input");
            free(avl_nodes);
            return 0;
            }
            root = Insertion(root, value, N, avl_nodes, &node_counter);
        }
        
    printf("%d", GetHeight(root));
    free(avl_nodes);
    return 0;
}

