#include"syntax_tree.h"
int get_int(const char* str)
{
	int res = 0;
	if (str[0] == '0')
	{
		if (str[1] == 'x' || str[1] == 'X')
		{
			for (int i = 2; str[i] != '\0'; i++)
			{
				if (str[i] <= 'F' && str[i] >= 'A')
				{
					res *= 16;
					res += 10 + (int)(str[i] - 'A');
				}
				else if (str[i] <= 'f' && str[i] >= 'a')
				{
					res *= 16;
					res += 10 + (int)(str[i] - 'a');
				}
				else
				{
					res *= 16;
					res += (int)(str[i] - '0');
				}
			}
			return res;
		}
		else if (str[1] == '\0') return 0;
		else
		{
			for (int i = 1; str[i] != '\0'; i++)
			{
				res *= 8;
				res += (int)(str[i] - '0');
			}
			return res;
		}
	}
	else return atoi(str);
}
float get_float(const char* str)
{
	return atof(str);
}
Node* init_token(const char* name, const char* value)//for tokens
{
	Node* node = (Node*)malloc(sizeof(Node));
	node->name=(char*)malloc(50);
        node->val=(char*)malloc(50);
	node->name = strcpy(node->name, name);
	node->val = strcpy(node->val, value);
	node->child = NULL;
	node->next = NULL;
	node->token = true;
	node->value = !(strcmp(name, "ID") && strcmp(name, "INT") && strcmp(name, "FLOAT") && strcmp(name, "TYPE"));
	node->vacuum = false;
	node->line = 0;
	return node;
}
Node* init_node_2(const char* name, int line)
{
	Node* node = (Node*)malloc(sizeof(Node));
        node->name=(char*)malloc(50);
	node->name = strcpy(node->name, name);
	node->line = line;
	node->child = NULL;
	node->next = NULL;
	node->token = false;
	node->value = false;
	node->vacuum = false;
	return node;
}
void append_node(Node* root, Node* subnode)
{
	if (root->child == NULL)
	{
		root->child = subnode;
	}
	else
	{
		Node* p;
		for (p = root->child; p->next != NULL; p = p->next);
		p->next = subnode;
	}
}
void check_vacuum(Node* root)
{
	//if (root == NULL) return;
	bool test = !(root->token);
	for (Node* p = root->child; p != NULL; p = p->next)
	{
		check_vacuum(p);
		test=test&&(p->vacuum);
	}
	root->vacuum = test;
}
void set_vacuum(Node* root)
{
	root->vacuum = true;
}
void print_tree(Node* root, int depth)
{
	if (root == NULL) return;
	if (root->vacuum) return;
	for (int i = 0; i < depth; i++)
	{
		printf("  ");
	}
	if (root->token)
	{
		printf("%s", root->name);
		if (root->value)
		{
			if(!strcmp(root->name,"INT"))
			{
				printf(": %d", get_int(root->val));
			}
			else if(!strcmp(root->name,"FLOAT")) printf(": %f", get_float(root->val));
			else printf(": %s", root->val);
		}
		printf("\n");
	}
	else
	{
		printf("%s (%d)\n", root->name, root->line);//syntax with line
		for (Node* p = root->child; p != NULL; p = p->next)//subnodes
		{
			print_tree(p, depth + 1);
		}
	}
}
Node* init_syn(const char* name, int line, int num, ...)
{
	Node* temp;
	Node* p = init_node_2(name, line);
	va_list valist;
	va_start(valist, num);
	for (int i = 0; i < num; i++)
	{
		temp = va_arg(valist, Node*);
		append_node(p, temp);
	}
	va_end(valist);
	return p;
}
Node* init_vacum(const char* name)
{
	Node* p = init_node_2(name, 0);
	set_vacuum(p);
	return p;
}

