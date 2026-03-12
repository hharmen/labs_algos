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

void Add(Node* node, int64_t value){
	if (value > node->key){
		if (node->right != NULL){
			Add(node->right, value);
			return;
		}
		node->right = New_Node(value);
		return;
	}else if (value < node->key){
		if (node->left != NULL){
			Add(node->left, value);
			return;
		}
		node->left = New_Node(value);
		return;
	}
	return;
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

int64_t main(){
	Node* node = New_Node(10);
	Add(node, 5);
	Add(node, 12);
	Add(node, 11);
	Add(node, 19);
	Add(node, 2);
	Add(node, 4);
	Add(node, 7);
	Add(node, 6);
	PrintTree(node, 0);
	node = PopInTree(node, 12);
	PrintTree(node, 0);
}
