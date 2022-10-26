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

int|float {yylval=new_leaf(yylineno,"TYPE",yytext);
    return TYPE;}
"struct" {yylval=new_leaf(yylineno,"STRUCT",yytext);
    return STRUCT;}
"return" {yylval=new_leaf(yylineno,"RETURN",yytext);
    return RETURN;}
"if" {yylval=new_leaf(yylineno,"IF",yytext);
    return IF;}
"else" {yylval=new_leaf(yylineno,"ELSE",yytext);
    return ELSE;}
"while" {yylval=new_leaf(yylineno,"WHILE",yytext);
    return WHILE;}

{INT} {yylval=new_leaf(yylineno,"INT",yytext);
    return INT;}
{FLOAT} {yylval=new_leaf(yylineno,"FLOAT",yytext);
    return FLOAT;}
{ID} {yylval=new_leaf(yylineno,"ID",yytext);
    return ID;}

";" {yylval=new_leaf(yylineno,"SEMI",yytext);
    return SEMI;}
"," {yylval=new_leaf(yylineno,"COMMA",yytext);
    return COMMA;}
"=" {yylval=new_leaf(yylineno,"ASSIGNOP",yytext);
    return ASSIGNOP;}
{RELOP} {yylval=new_leaf(yylineno,"RELOP",yytext);
    return RELOP;}

"+" {yylval=new_leaf(yylineno,"PLUS",yytext);
    return PLUS;}
"-" {yylval=new_leaf(yylineno,"MINUS",yytext);
    return MINUS;}
"*" {yylval=new_leaf(yylineno,"STAR",yytext);
    return STAR;}
"/" {yylval=new_leaf(yylineno,"DIV",yytext);
    return DIV;}
"&&" {yylval=new_leaf(yylineno,"AND",yytext);
    return AND;}
"||" {yylval=new_leaf(yylineno,"OR",yytext);
    return OR;}

"." {yylval=new_leaf(yylineno,"DOT",yytext);
    return DOT;}
"!" {yylval=new_leaf(yylineno,"NOT",yytext);
    return NOT;}
"(" {yylval=new_leaf(yylineno,"LP",yytext);
    return LP;}
")" {yylval=new_leaf(yylineno,"RP",yytext);
    return RP;}
"[" {yylval=new_leaf(yylineno,"LB",yytext);
    return LB;}
"]" {yylval=new_leaf(yylineno,"RB",yytext);
    return RB;}
"{" {yylval=new_leaf(yylineno,"LC",yytext);
    return LC;}
"}" {yylval=new_leaf(yylineno,"RC",yytext);
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


