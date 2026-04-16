#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef enum { NODE_NUM, NODE_VAR, NODE_OP } NodeType;

typedef struct Node {
    NodeType type;
    union {
        int num;          // число
        char var;         // переменная (один символ)
        char op;          // оператор '+', '-', '*', '/'
    } data;
    struct Node *left;
    struct Node *right;
} Node;

Node* parse_expression(const char **s);
Node* parse_term(const char **s);
Node* parse_factor(const char **s);
void print_tree(Node *root, int level);
void print_infix(Node *root);
int precedence(char op);
void print_infix_sub(Node *node, int parent_prec, int right_assoc);
Node* remove_ones(Node *node);
void free_tree(Node *root);
void skip_spaces(const char **s);


void skip_spaces(const char **s) {
    while (**s == ' ') (*s)++;
}

Node* make_num(int val) {
    Node *n = (Node*)malloc(sizeof(Node));
    n->type = NODE_NUM;
    n->data.num = val;
    n->left = n->right = NULL;
    return n;
}

Node* make_var(char v) {
    Node *n = (Node*)malloc(sizeof(Node));
    n->type = NODE_VAR;
    n->data.var = v;
    n->left = n->right = NULL;
    return n;
}

Node* make_op(char op, Node *left, Node *right) {
    Node *n = (Node*)malloc(sizeof(Node));
    n->type = NODE_OP;
    n->data.op = op;
    n->left = left;
    n->right = right;
    return n;
}

Node* parse_number(const char **s) {
    int val = 0;
    while (isdigit(**s)) {
        val = val * 10 + (**s - '0');
        (*s)++;
    }
    return make_num(val);
}

Node* parse_variable(const char **s) {
    char v = **s;
    (*s)++;
    return make_var(v);
}

Node* parse_factor(const char **s) {
    skip_spaces(s);
    if (**s == '(') {
        (*s)++;
        Node *expr = parse_expression(s);
        skip_spaces(s);
        if (**s == ')') {
            (*s)++;
        } else {
            fprintf(stderr, "Ожидалась ')'\n");
            free_tree(expr);
            return NULL;
        }
        return expr;
    } else if (isdigit(**s)) {
        return parse_number(s);
    } else if (isalpha(**s)) {
        return parse_variable(s);
    } else {
        fprintf(stderr, "Неожиданный символ '%c'\n", **s);
        return NULL;
    }
}

Node* parse_term(const char **s) {
    Node *left = parse_factor(s);
    if (!left) return NULL;
    skip_spaces(s);
    while (**s == '*' || **s == '/') {
        char op = **s;
        (*s)++;
        Node *right = parse_factor(s);
        if (!right) {
            free_tree(left);
            return NULL;
        }
        left = make_op(op, left, right);
        skip_spaces(s);
    }
    return left;
}

Node* parse_expression(const char **s) {
    Node *left = parse_term(s);
    if (!left) return NULL;
    skip_spaces(s);
    while (**s == '+' || **s == '-') {
        char op = **s;
        (*s)++;
        Node *right = parse_term(s);
        if (!right) {
            free_tree(left);
            return NULL;
        }
        left = make_op(op, left, right);
        skip_spaces(s);
    }
    return left;
}

void print_tree(Node *root, int space) {
    if (root == NULL) return;
    int indent = space + 4;
    print_tree(root->right, indent);
    for (int i = 0; i < space; i++) printf(" ");
    switch (root->type) {
        case NODE_NUM: printf("%d\n", root->data.num); break;
        case NODE_VAR: printf("%c\n", root->data.var); break;
        case NODE_OP:  printf("%c\n", root->data.op); break;
    }
    print_tree(root->left, indent);
}

int precedence(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        default: return 0;
    }
}

void print_infix_sub(Node *node, int parent_prec, int right_assoc) {
    if (!node) return;
    if (node->type == NODE_NUM) {
        printf("%d", node->data.num);
    } else if (node->type == NODE_VAR) {
        printf("%c", node->data.var);
    } else if (node->type == NODE_OP) {
        int prec = precedence(node->data.op);
        int need_paren = 0;
        if (prec < parent_prec) need_paren = 1;
        if (prec == parent_prec && right_assoc) need_paren = 1;

        if (need_paren) printf("(");
        print_infix_sub(node->left, prec, 0);
        printf(" %c ", node->data.op);
        int right_assoc_flag = (node->data.op == '-' || node->data.op == '/') ? 1 : 0;
        print_infix_sub(node->right, prec, right_assoc_flag);
        if (need_paren) printf(")");
    }
}

void print_infix(Node *root) {
    print_infix_sub(root, 0, 0);
}


Node* remove_ones(Node *node) {
    if (!node) return NULL;
    node->left = remove_ones(node->left);
    node->right = remove_ones(node->right);

    if (node->type == NODE_OP && node->data.op == '*') {
        Node *left = node->left;
        Node *right = node->right;


        if (left->type == NODE_NUM && left->data.num == 1) {
            free(left);
            Node *result = right;
            free(node);
            return result;
        }

        if (right->type == NODE_NUM && right->data.num == 1) {
            free(right);
            Node *result = left;
            free(node);
            return result;
        }

        if (left->type == NODE_NUM && left->data.num == 1 &&
            right->type == NODE_NUM && right->data.num == 1) {
            free(left);
            free(right);
            Node *result = make_num(1);
            free(node);
            return result;
        }
    }
    return node;
}

void free_tree(Node *root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

int main() {
    char line[1024];
    printf("При вводе выражения не надо писать пробелы и обязательно нужно ставить все умножения, а не как в математике их пропускать\nВведите выражения (Ctrl+D для завершения):\n");
    while (fgets(line, sizeof(line), stdin)) {
        // Удаляем символ перевода строки
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';
        if (strlen(line) == 0) continue;

        const char *p = line;
        Node *expr = parse_expression(&p);
        if (!expr) {
            printf("Ошибка разбора выражения.\n");
            continue;
        }

        printf("\nИсходное выражение: ");
        print_infix(expr);
        printf("\nДерево исходного выражения:\n");
        print_tree(expr, 0);

        Node *transformed = remove_ones(expr);
        printf("\nВыражение (без сомножителей 1): ");
        print_infix(transformed);
        printf("\nДерево преобразованного выражения:\n");
        print_tree(transformed, 0);
        printf("----------------------------------------\n");

        free_tree(transformed);
    }
    return 0;
}