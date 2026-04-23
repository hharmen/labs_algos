#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char key[33];       // Ключ (до 32 символов + \0)
    double value;
    int height;
    struct Node *left;
    struct Node *right;
} Node;

int max(int a, int b) { return (a > b) ? a : b; }

int getHeight(Node *n) { return n ? n->height : 0; }

int getBalance(Node *n) { return n ? getHeight(n->left) - getHeight(n->right) : 0; }

Node* createNode(char* key, double value) {
    Node* node = (Node*)malloc(sizeof(Node));
    strncpy(node->key, key, 32);
    node->key[32] = '\0';
    node->value = value;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

Node* rightRotate(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    return x;
}

Node* leftRotate(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    return y;
}

Node* addNode(Node* node, char* key, double value, int *added) {
    if (!node) { 
        *added = 1;
        return createNode(key, value); 
    }

    int cmp = strcmp(key, node->key);
    if (cmp < 0) {
        node->left = addNode(node->left, key, value, added);
    } else if (cmp > 0) {
        node->right = addNode(node->right, key, value, added);
    } else {
        node->value = value;
        *added = 2;
        return node; 
    }

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);

    if (balance > 1 && strcmp(key, node->left->key) < 0) return rightRotate(node);
    if (balance < -1 && strcmp(key, node->right->key) > 0) return leftRotate(node);
    if (balance > 1 && strcmp(key, node->left->key) > 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && strcmp(key, node->right->key) < 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

Node* minValueNode(Node* node) {
    Node* current = node;
    while (current->left) current = current->left;
    return current;
}

Node* deleteNode(Node* root, char* key, int *deleted) {
    if (!root) return root;

    int cmp = strcmp(key, root->key);
    if (cmp < 0) root->left = deleteNode(root->left, key, deleted);
    else if (cmp > 0) root->right = deleteNode(root->right, key, deleted);
    else {
        *deleted = 1;
        if (!root->left || !root->right) {
            Node *temp = root->left ? root->left : root->right;
            free(root);
            return temp;
        }
        Node* temp = minValueNode(root->right);
        strcpy(root->key, temp->key);
        root->value = temp->value;
        root->right = deleteNode(root->right, temp->key, deleted);
    }

    if (!root) return root;
    root->height = 1 + max(getHeight(root->left), getHeight(root->right));
    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0) return rightRotate(root);
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && getBalance(root->right) <= 0) return leftRotate(root);
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    return root;
}

void printTree(Node* root, int space, FILE* out) {
    if (!root) return;
    space += 5;
    printTree(root->right, space, out);
    for (int i = 5; i < space; i++) fprintf(out, " ");
    fprintf(out, "%s(%.2f)\n", root->key, root->value);
    printTree(root->left, space, out);
}

Node* search(Node* root, char* key) {
    if (!root || strcmp(root->key, key) == 0) return root;
    if (strcmp(root->key, key) < 0) return search(root->right, key);
    return search(root->left, key);
}

void freeTree(Node* root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

int main() {
    FILE *in = fopen("input.txt", "r");
    FILE *out = fopen("output.txt", "w");
    if (!in || !out) return 1;

    Node *root = NULL;
    int op;
    char k[128];
    double v;

    while (fscanf(in, "%d", &op) != EOF) {
        switch (op) {
            case 1:
                fscanf(in, "%s %lf", k, &v);
                int added = 0;
                root = addNode(root, k, v, &added);
                fprintf(out, "1 %s %.2f\n%s\n", k, v, added==1 ? "Добавлен" : "Уже есть ключ, обновил значение");
                break;
            case 2:
                fscanf(in, "%s", k);
                int deleted = 0;
                root = deleteNode(root, k, &deleted);
                fprintf(out, "2 %s\n%s\n", k, deleted ? "Удален" : "Не найдено");
                break;
            case 3:
                fprintf(out, "3\n-----   Дерево   -----\n");
                printTree(root, 0, out);
                fprintf(out, "----------------------\n");
                break;
            case 4:
                fscanf(in, "%s", k);
                Node *res = search(root, k);
                fprintf(out, "4 %s\n", k);
                if (res) fprintf(out, "Value: %.2f\n", res->value);
                else fprintf(out, "Not found\n");
                break;
        }
    }

    fclose(in);
    fclose(out);
    freeTree(root);
    return 0;
}
