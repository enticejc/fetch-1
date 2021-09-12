#ifndef SYNTAX_TREE_H
#define SYNTAX_TREE_H
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<stdbool.h>
#include<stdarg.h>
#include<string.h>
typedef struct syn_tree
{
	bool token;
	bool vacuum;
	bool value;
	int line;
	char* name;
	char* val;
	struct syn_tree* child;
	struct syn_tree* next;
}Node;
extern Node* init_token(const char* name, const char* value);//for tokens
extern Node* init_node_2(const char* name, int line);//for syntaxs
extern void append_node(Node* root, Node* subnode);
extern void print_tree(Node* root, int depth);
extern void check_vacuum(Node* root);
extern void set_vacuum(Node* root);
extern Node* init_syn(const char* name, int line, int num, ...);
extern Node* init_vacum(const char* name);
#endif
