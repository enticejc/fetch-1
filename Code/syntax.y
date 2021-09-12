%{
#include "syntax_tree.h"
#include "sym_table.h"
#define YYDEBUG 1

#define YYSTYPE Node*
Node* root;
void yyerror(char* msg);
void myerr(char* msg);
int yylex();
%}
%nonassoc LOWER_THAN_ANYTHING

%token INT
%token FLOAT
%token ID
%token COMMA
%token TYPE 

%nonassoc LC RC

%token STRUCT RETURN IF WHILE
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT

%nonassoc LOWER_THAN_RP
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_SEMI
%token SEMI

%%

//high-level definitions

Program: ExtDefList					{$$ = init_syn("Program", @$.first_line, 1, $1);
										root = $$;}
	;
ExtDefList: ExtDef ExtDefList 		{$$ = init_syn("ExtDefList", @$.first_line, 2, $1, $2);}
	| 								{$$ = init_vacum("ExtDefList");}
	; 
ExtDef: Specifier ExtDecList SEMI	{$$ = init_syn("ExtDef", @$.first_line, 3, $1, $2, $3);}		
	| Specifier SEMI				{$$ = init_syn("ExtDef", @$.first_line, 2, $1, $2);}			
	| Specifier FunDec CompSt		{$$ = init_syn("ExtDef", @$.first_line, 3, $1, $2, $3);}	
	| Specifier FunDec SEMI			{$$ = init_syn("ExtDef", @$.first_line, 3, $1, $2, $3);}
	//errors	
	| error SEMI 					{myerr("Expected Specifier.");}
	| Specifier error SEMI 			{myerr("Cannot combine with previous specifier.");}
	| Specifier error CompSt		{myerr("Incorrect FunDec.");}
	| Specifier ExtDecList error SEMI	{myerr("Unexpected token following VarDec.");}
	| Specifier ExtDecList	 %prec LOWER_THAN_ANYTHING	 {myerr("Expected \";\" after Def.");}
	;

ExtDecList: VarDec					{$$ = init_syn("ExtDecList", @$.first_line, 1, $1);}
	| VarDec COMMA ExtDecList		{$$ = init_syn("ExtDecList", @$.first_line, 3, $1, $2, $3);}		
	;

//Specifiers

Specifier: TYPE						{$$ = init_syn("Specifier", @$.first_line, 1, $1);}	
	| StructSpecifier				{$$ = init_syn("Specifier", @$.first_line, 1, $1);}
	; 
StructSpecifier: STRUCT OptTag LC DefList RC	{$$ = init_syn("StructSpecifier", @$.first_line, 5, $1, $2, $3, $4, $5);}
	| STRUCT Tag								{$$ = init_syn("StructSpecifier", @$.first_line, 2, $1, $2);}
	;
OptTag: ID							{$$ = init_syn("OptTag", @$.first_line, 1, $1);}
	|				 				{$$ = init_vacum("OptTag");}
	;
Tag: ID								{$$ = init_syn("Tag", @$.first_line, 1, $1);}
	;

//Declarators

VarDec: ID							{$$ = init_syn("VarDec", @$.first_line, 1, $1);}
	| VarDec LB INT RB				{$$ = init_syn("VarDec", @$.first_line, 4, $1, $2, $3, $4);}
	//errors	
	| VarDec LB error RB			{myerr("Expected INT between \"[]\".");}
	;
FunDec: ID LP VarList RP			{$$ = init_syn("FunDec", @$.first_line, 4, $1, $2, $3, $4);}
	| ID LP RP						{$$ = init_syn("FunDec", @$.first_line, 3, $1, $2, $3);}
	//errors	
	| error LP RP					{myerr("Expected FunID.");}
	| ID LP error RP				{myerr("Expected VarList.");}
//	| ID error RP					{myerr("Expected \"(\".");}
	;
VarList: ParamDec COMMA VarList		{$$ = init_syn("VarList", @$.first_line, 3, $1, $2, $3);}
	| ParamDec						{$$ = init_syn("VarList", @$.first_line, 1, $1);}
	//errors	
	| error COMMA VarList			{myerr("Expected ParamDec.");}
	| ParamDec COMMA error			{myerr("Expected VarList.");}
	;
ParamDec: Specifier VarDec			{$$ = init_syn("ParamDec", @$.first_line, 2, $1, $2);}
	//errors	
	| error VarDec					{myerr("Expected Specifier.");}
	| Specifier error 				{myerr("Expected VarDec.");}
	;

//Statements

CompSt: LC DefList StmtList RC		{$$ = init_syn("CompSt", @$.first_line, 4, $1, $2, $3, $4);} 
	//errors
	| error RC	%prec LOWER_THAN_ANYTHING {myerr("Expected \"{\".");}
	| LC DefList StmtList %prec LOWER_THAN_ANYTHING	{myerr("Expected \"}\".");} 
	;
StmtList: Stmt StmtList				{$$ = init_syn("StmtList", @$.first_line, 2, $1, $2);}
	|								{$$ = init_vacum("StmtList");}
	; 
Stmt: Exp SEMI						{$$ = init_syn("Stmt", @$.first_line, 2, $1, $2);}
	| CompSt						{$$ = init_syn("Stmt", @$.first_line, 1, $1);}
	| RETURN Exp SEMI				{$$ = init_syn("Stmt", @$.first_line, 3, $1, $2, $3);}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE	{$$ = init_syn("Stmt", @$.first_line, 5, $1, $2, $3, $4, $5);}
	| IF LP Exp RP Stmt ELSE Stmt				{$$ = init_syn("Stmt", @$.first_line, 7, $1, $2, $3, $4, $5, $6, $7);}
	| WHILE LP Exp RP Stmt			{$$ = init_syn("Stmt", @$.first_line, 5, $1, $2, $3, $4, $5);}
	//errors
	| IF LP error RP Stmt %prec LOWER_THAN_ELSE	{myerr("Expected Exp.");}
	| IF LP error RP Stmt ELSE Stmt				{myerr("Expected Exp in \"()\".");}
	| WHILE LP error RP Stmt		{myerr("Expected Exp in \"()\".");}	
	| IF error						{myerr("Expected \"(\" after IF.");}
	| WHILE error					{myerr("Expected \"(\" after WHILE.");}
	| RETURN error SEMI				{myerr("Expected Exp to return.");}		
	| Exp error	SEMI				{myerr("Incorrect Stmt.");}	
	| Exp error	%prec LOWER_THAN_SEMI {myerr("Expected \";\" after Stmt.");}	
	| RETURN Exp error				{myerr("Expected \";\" after Stmt.");}
	| error	SEMI					{myerr("Incorrect Stmt.");}	
	| error ELSE Stmt				{myerr("Expected \"if\" before  \"else\".");}	
	;

//Local definitions

DefList: Def DefList				{$$ = init_syn("DefList", @$.first_line, 2, $1, $2);}
	|								{$$ = init_vacum("DefList");}
	; 
Def: Specifier DecList SEMI			{$$ = init_syn("Def", @$.first_line, 3, $1, $2, $3);}
	//errors	
	| Specifier error SEMI			{myerr("Incorrect DecList.");}
	| Specifier DecList	error		{myerr("Expected \";\" after Def.");}
	;
DecList: Dec						{$$ = init_syn("DecList", @$.first_line, 1, $1);}
	| Dec COMMA DecList				{$$ = init_syn("DecList", @$.first_line, 3, $1, $2, $3);}
	//errors
	| error COMMA DecList			{myerr("Expected Dec.");}
	| Dec COMMA error				{myerr("Expected DecList.");}
	;
Dec: VarDec							{$$ = init_syn("Dec", @$.first_line, 1, $1);}
	| VarDec ASSIGNOP Exp			{$$ = init_syn("Dec", @$.first_line, 3, $1, $2, $3);}
	//errors	
	| VarDec ASSIGNOP error 		{myerr("Expected Exp.");}
	;

//Expressions

Exp: Exp ASSIGNOP Exp				{$$ = init_syn("Exp", @$.first_line, 3, $1, $2, $3);} 
	| Exp AND Exp 					{$$ = init_syn("Exp", @$.first_line, 3, $1, $2, $3);}
	| Exp OR Exp 					{$$ = init_syn("Exp", @$.first_line, 3, $1, $2, $3);}
	| Exp RELOP Exp 				{$$ = init_syn("Exp", @$.first_line, 3, $1, $2, $3);}
	| Exp PLUS Exp 					{$$ = init_syn("Exp", @$.first_line, 3, $1, $2, $3);}
	| Exp MINUS Exp 				{$$ = init_syn("Exp", @$.first_line, 3, $1, $2, $3);}
	| Exp STAR Exp 					{$$ = init_syn("Exp", @$.first_line, 3, $1, $2, $3);}
	| Exp DIV Exp 					{$$ = init_syn("Exp", @$.first_line, 3, $1, $2, $3);}
	| LP Exp RP 					{$$ = init_syn("Exp", @$.first_line, 3, $1, $2, $3);}
	| MINUS Exp 					{$$ = init_syn("Exp", @$.first_line, 2, $1, $2);}
	| NOT Exp 						{$$ = init_syn("Exp", @$.first_line, 2, $1, $2);}
	| ID LP Args RP					{$$ = init_syn("Exp", @$.first_line, 4, $1, $2, $3, $4);}
	| ID LP RP						{$$ = init_syn("Exp", @$.first_line, 3, $1, $2, $3);}
	| Exp LB Exp RB					{$$ = init_syn("Exp", @$.first_line, 4, $1, $2, $3, $4);}
	| Exp DOT ID					{$$ = init_syn("Exp", @$.first_line, 3, $1, $2, $3);}
	| ID 							{$$ = init_syn("Exp", @$.first_line, 1, $1);}
	| INT 							{$$ = init_syn("Exp", @$.first_line, 1, $1);}
	| FLOAT 						{$$ = init_syn("Exp", @$.first_line, 1, $1);}
	//errors
	| Exp ASSIGNOP error			{myerr("Expected Exp.");}
	| ID LP error RP				{myerr("Expected Args between \"()\".");}
	| ID LP error %prec LOWER_THAN_RP	{myerr("Expected Args or \")\".");}
	| Exp LB error RB 				{myerr("Expected INT between \"[]\".");}
	| Exp LB Exp error				{myerr("Expected  \"]\".");}
	;
Args : Exp COMMA Args 				{$$ = init_syn("Args", @$.first_line, 3, $1, $2, $3);}
	| Exp 							{$$ = init_syn("Args", @$.first_line, 1, $1);}
//	| error COMMA Args				{myerr("Expected  Exp.");}
//	| Exp COMMA error				{myerr("Expected  Args.");}
	;

%%
#include "lex.yy.c"
int main(int argc, char** argv)
{

	if (argc <= 1) return 1;
	FILE* f = fopen(argv[1], "r");
	if (!f)
	{
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);

	//only enabled in debuging
	//yydebug = 1; 

	yyparse();
	if(!ERROR_FLAG)
	{
		check_vacuum(root);
		//print_tree(root,0);
		Program(root);
		check_func_def();
	}else{
		printf("Lexical or syntax error.\n");
	}
	return 0;
}

void myerr(char* msg){
	ERROR_FLAG = true;
	printf("ERROR TYPE B at line %d: %s\n", line, msg);
}

void yyerror(char* msg){
	ERROR_FLAG = true;
	//do nothings
}
