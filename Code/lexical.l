digit [0-9]
letter [_a-zA-Z]
character [0-9_a-zA-Z]
%{
#include<stdio.h>
int line = 1;
int yycolumn = 1;
bool ERROR_FLAG = false;
void lexErr(char* msg, char* text);

#define YY_USER_ACTION \
yylloc.first_line=yylloc.last_line=yylineno; \
yylloc.first_column=yycolumn; \
yylloc.last_column=yycolumn+yyleng-1; \
yycolumn+=yyleng;
%}

%%
("int"|"float")/[^0-9_a-zA-Z]   {yylval = init_token("TYPE",yytext);return TYPE;}
"if"/[^0-9_a-zA-Z]              {yylval = init_token("IF",yytext);return IF;}
"else"/[^0-9_a-zA-Z]            {yylval = init_token("ELSE",yytext);return ELSE;}
"while"/[^0-9_a-zA-Z]           {yylval = init_token("WHILE",yytext);return WHILE;}
"return"/[^0-9_a-zA-Z]          {yylval = init_token("RETURN",yytext);return RETURN;}
"struct"/[^0-9_a-zA-Z]          {yylval = init_token("STRUCT",yytext);return STRUCT;}
(([1-9][0-9]*)|0)\.{digit}+     {yylval = init_token("FLOAT",yytext);return FLOAT;}
0[0-7]+                         {yylval = init_token("INT",yytext);return INT;} 
0[xX][0-9A-Fa-f]+               {yylval = init_token("INT",yytext);return INT;} 
0[0-7]*[8-9]+[0-7]*             {lexErr("Illegal octal number",yytext); yylval = init_token("INT",yytext);return INT;}
0[xX][0-9A-Fa-f]*[G-Zg-z]+[0-9A-Fa-f]*  {lexErr("Illegal hexadecimal number",yytext); yylval = init_token("INT",yytext);return INT;}
([1-9][0-9]*)|0                 {yylval = init_token("INT",yytext);return INT;}
{letter}{character}*            {yylval = init_token("ID",yytext);return ID;}
";"                             {yylval = init_token("SEMI",yytext);return SEMI;}
","                             {yylval = init_token("COMMA",yytext);return COMMA;}
"=="|">="|"<="|"!="             {yylval = init_token("RELOP",yytext);return RELOP;}
(">"|"<")/[^=]                  {yylval = init_token("RELOP",yytext);return RELOP;}
"="/[^=]                        {yylval = init_token("ASSIGNOP",yytext);return ASSIGNOP;}
"*"                             {yylval = init_token("STAR",yytext);return STAR;}
"+"                             {yylval = init_token("PLUS",yytext);return PLUS;}
"-"                             {yylval = init_token("MINUS",yytext);return MINUS;}
"."                             {yylval = init_token("DOT",yytext);return DOT;}
"!"/[^=]                        {yylval = init_token("NOT",yytext);return NOT;}
"/"                             {yylval = init_token("DIV",yytext);return DIV;}
"&&"                            {yylval = init_token("AND",yytext);return AND;}
"||"                            {yylval = init_token("OR",yytext);return OR;}
"("                             {yylval = init_token("LP",yytext);return LP;}
")"                             {yylval = init_token("RP",yytext);return RP;}
"["                             {yylval = init_token("LB",yytext);return LB;}
"]"                             {yylval = init_token("RB",yytext);return RB;}
"{"                             {yylval = init_token("LC",yytext);return LC;}
"}"                             {yylval = init_token("RC",yytext);return RC;}
\n                              {yycolumn = 1;line++;yylineno++;}
[ \t\0\r]+                      

.                               {lexErr("Undefined character",yytext);}

%%
void lexErr(char* msg, char* text){
    ERROR_FLAG = true;
    printf("ERROR TYPE A at line %d: %s: %s\n",line, msg, text);
}