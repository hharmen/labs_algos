#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct Node{
	int64_t key;
	struct Node* left;
	struct Node* right;
} Node;

Node* New_Node(int64_t value){
	Node* new_node = (Node*)malloc(sizeof(Node));
	new_node->key = value;
	new_node->right = NULL;
	new_node->left = NULL;
	return new_node;
}

Node* Add(Node* node, int64_t value){
	if (node == NULL){
		node = New_Node(value);
		return node;
	}
	if (value > node->key){
		if (node->right != NULL){
			node->right = Add(node->right, value);
			return node;
		}
		node->right = New_Node(value);
	}else if (value < node->key){
		if (node->left != NULL){
			node-> left = Add(node->left, value);
			return node;
		}
		node->left = New_Node(value);
	}
	return node;
}

void RemoveTree(Node* node){
	if (node == NULL){
		return;
	}
	RemoveTree(node->right);
	RemoveTree(node->left);
	free(node);
}

Node* AreNodeInTree(Node* node, int64_t value){
	if (node == NULL){ 
		return NULL;
	}
	if (value == node->key){
		return node;
	}
	if (value < node->key){
		return AreNodeInTree(node->left, value);
	}
	return AreNodeInTree(node->right, value);

}

Node* PopInTree(Node* node, int64_t value){
	if (!AreNodeInTree(node, value)){
		return node;
	}

	if (node->key == value){
		if (node->right == NULL && node->left == NULL){
			free(node);
			return NULL;
		}
		if (node->right == NULL){
			Node* new_node = node->left;
			free(node);
			return new_node;
		}
		if (node->left == NULL){
			Node* new_node = node->right;
			free(node);
			return new_node;
		}
		Node* new_node = node->left;
		Node* prev_node = node;
		if (new_node->right == NULL){
			new_node->right = node->right;
			free(node);
			return new_node;
		}
		while (new_node->right != NULL){
			prev_node = new_node;
			new_node = new_node->right;
		}

		prev_node->right = new_node->left;
		new_node->right = node->right;
		new_node->left = node->left;
		free(node);
		return new_node;
		
	}
	
	Node* need_to_remove_node = node;
	Node* prev_node;
	int64_t left_or_right;

	while (need_to_remove_node->key != value){
		prev_node = need_to_remove_node;
		if (need_to_remove_node->key > value){
			need_to_remove_node = need_to_remove_node->left;
			left_or_right = 0;
		}else{
			need_to_remove_node = need_to_remove_node->right;
			left_or_right = 1;
		}
	}

	
	if (need_to_remove_node->right == NULL && need_to_remove_node->left == NULL){
		if (left_or_right){
			prev_node->right = NULL;
		}else{
			prev_node->left = NULL;
		}
		free(need_to_remove_node);
		return node;
	}
	if (need_to_remove_node->right == NULL){
		if (left_or_right){
			prev_node->right = need_to_remove_node->left;
		}else{
			prev_node->left = need_to_remove_node->left;
		}
		free(need_to_remove_node);
		return node;
	}
	if (need_to_remove_node->left == NULL){
		if (left_or_right){
			prev_node->right = need_to_remove_node->right;
		}else{
			prev_node->left = need_to_remove_node->right;
		}
		free(need_to_remove_node);
		return node;
	}

	Node* new_node = need_to_remove_node->right;
	Node* prev_node_for_new = need_to_remove_node;
	if (new_node->left == NULL){
		new_node->left = need_to_remove_node->left;
		if (left_or_right){
			prev_node->right = new_node;
		}else{
			prev_node->left = new_node;
		}
		free(need_to_remove_node);
		return node;

	}
	while (new_node->left != NULL){
		prev_node_for_new = new_node;
		new_node = new_node->left;
	}

	if (left_or_right){
		prev_node->right = new_node;
	}else{
		prev_node->left = new_node;
	}
	prev_node_for_new->left = new_node->right;
	new_node->right = need_to_remove_node->right;
	new_node->left = need_to_remove_node->left;
	free(need_to_remove_node);
	return node;
	
}

int64_t DepthTree(Node* node){
	if (node == NULL){
		return 0;
	}
	int64_t depth_left = DepthTree(node->left);
	int64_t depth_right = DepthTree(node->right);
	return (depth_left > depth_right ? depth_left : depth_right)+1;
}

void PrintTree(Node* node, int64_t depth){
	if (node == NULL){
		return;
	}
	PrintTree(node->right, depth+1);
	for (int64_t i = 0; i<depth; i++){
		printf("  ");
	}
	printf("%ld\n", node->key);
	PrintTree(node->left, depth+1);
}

void SolveVar15(Node* node, int64_t* widths, int64_t depth){
	if (node == NULL){
		return;
	}
	widths[depth] += 1;
	SolveVar15(node->left, widths, depth+1);
	SolveVar15(node->right, widths, depth+1);
}

int main(){

	Node* node = NULL;
	PrintTree(node, 0);
	
	while (1) {
		int32_t exec_variant;
		printf("1. Добавить число\n");
		printf("2. Удалить число\n");
		printf("3. Вывести дерево\n");
		printf("4. Решить задачу (варант 15)\n");
		printf("5. Выйти из программы\n");
		printf("Выберите вариант (введите только цифру): ");

		if (scanf("%d", &exec_variant) != 1){
			printf("========================\n");
			printf("НАХРЕНА ТЫ ВЕЛ ЧТО-ТО ЕЩЕ КРОМЕ ЧИСЛА\n");
			printf("========================\n");
			while (getchar() != '\n');
			continue;
		}

		int64_t value;

		switch (exec_variant){
			case 1:
                printf("Введите число для добавления: ");
                if (!scanf("%lld", &value)) {
					printf("========================\n");
                    printf("ТЫ НЕ ЗНАЕШЬ КАК ВЫГЛЯДИТ ЧИСЛА?\n");
					printf("========================\n");
					while (getchar() != '\n');
                    continue;
                }
                node = Add(node, value);
                printf("Число %lld добавлено (если его уже не было конечно)\n", value);
                break;
			case 2:
                printf("Введите число для удаления: ");
                if (!scanf("%lld", &value)) {
					printf("========================\n");
                    printf("ТЫ НЕ ЗНАЕШЬ КАК ВЫГЛЯДИТ ЧИСЛА?\n");
					printf("========================\n");
					while (getchar() !=  '\n');
                    continue;
                }
                node = PopInTree(node, value);
                printf("Число %lld удалено (если он был конечно)\n", value);
                break;
			case 3:
				printf("========================\n");
				PrintTree(node, 0);
				printf("========================\n");
				break;
			case 4:
				int64_t depth = DepthTree(node);
				int64_t* widths = (int64_t*)calloc(depth, sizeof(int64_t));
				SolveVar15(node, widths, 0);
				int64_t res = 0;
				for (int64_t i = 0; i < depth; i++){
					if (res < widths[i]){
						res = widths[i];
					}
				}
				free(widths);
				printf("========================\n");
				printf("Ответ на задание: %lld\n", res);
				printf("========================\n");
				break;
			case 5:
				RemoveTree(node);
				return 0;
			default:
				printf("========================\n");
				printf("НЕТ ТАКОГО ДЕЙСТВИЯ\n");
				printf("========================\n");
		}
	}

}
