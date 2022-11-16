%{
#include "syntax.tab.h"
#include<stdio.h>
int yycolumn = 1;
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;

%}

%option yylineno
number [0-9]
hexnumbers [0-9a-fA-F]
letter [_a-zA-Z]
oct 0[0-7]*
dec 0|([1-9]{number}*)
hex 0[xX]{hexnumbers}+
decimal ({number}*\.{number}+)|({number}+\.{number}*)

INT {oct}|{dec}|{hex}
FLOAT {decimal}([eE][+-]?{number}+)?
ID {letter}({letter}|{number})*
RELOP >|<|>=|<=|==|!=

%%

int|float {yylval=new_leaf(yylineno,"TYPE");
    return TYPE;}
"struct" {yylval=new_leaf(yylineno,"STRUCT");
    return STRUCT;}
"return" {yylval=new_leaf(yylineno,"RETURN");
    return RETURN;}
"if" {yylval=new_leaf(yylineno,"IF");
    return IF;}
"else" {yylval=new_leaf(yylineno,"ELSE");
    return ELSE;}
"while" {yylval=new_leaf(yylineno,"WHILE");
    return WHILE;}

{INT} {yylval=new_leaf(yylineno,"INT");
    return INT;}
{FLOAT} {yylval=new_leaf(yylineno,"FLOAT");
    return FLOAT;}
{ID} {yylval=new_leaf(yylineno,"ID");
    return ID;}

";" {yylval=new_leaf(yylineno,"SEMI");
    return SEMI;}
"," {yylval=new_leaf(yylineno,"COMMA");
    return COMMA;}
"=" {yylval=new_leaf(yylineno,"ASSIGNOP");
    return ASSIGNOP;}
{RELOP} {yylval=new_leaf(yylineno,"RELOP");
    return RELOP;}

"+" {yylval=new_leaf(yylineno,"PLUS");
    return PLUS;}
"-" {yylval=new_leaf(yylineno,"MINUS");
    return MINUS;}
"*" {yylval=new_leaf(yylineno,"STAR");
    return STAR;}
"/" {yylval=new_leaf(yylineno,"DIV");
    return DIV;}
"&&" {yylval=new_leaf(yylineno,"AND");
    return AND;}
"||" {yylval=new_leaf(yylineno,"OR");
    return OR;}

"." {yylval=new_leaf(yylineno,"DOT");
    return DOT;}
"!" {yylval=new_leaf(yylineno,"NOT");
    return NOT;}
"(" {yylval=new_leaf(yylineno,"LP");
    return LP;}
")" {yylval=new_leaf(yylineno,"RP");
    return RP;}
"[" {yylval=new_leaf(yylineno,"LB");
    return LB;}
"]" {yylval=new_leaf(yylineno,"RB");
    return RB;}
"{" {yylval=new_leaf(yylineno,"LC");
    return LC;}
"}" {yylval=new_leaf(yylineno,"RC");
    return RC;}

\n {yycolumn = 1;}
[\t\r] {;}
" " {;}
"//" {
	char c = input();
	while (c != '\n' && c)
		c = input();
}
"/*" {
    char pre=input(),now=input();
    while(pre!='*'||now!='/'){
        pre=now;
        now=input();
        if(!now){
            printf("untermminated comment");
            if_over=0;
            break;
        }
    }
}

"*/" {
   printf("Error type B at Line %d: Syntax error\n",yylineno);
   if_over=0; 
}

. {printf("Error type A at Line %d: Mysterious characters \"%s\"\n",
yylineno, yytext);
if_over=0;}

%%

/*
int main(int argc, char** argv) {
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    }
    yylex();
    return 0;
}*/


