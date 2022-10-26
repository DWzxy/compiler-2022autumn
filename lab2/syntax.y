%{
//#define DEBUG
void debug(char*str);

#include"node.h"
int if_over=1;
#include "lex.yy.c"
extern Node* semantic_tree;

void yyerror(char* msg, ...);
%}

/* declared tokens */
%token INT
%token FLOAT
%token ID
%token SEMI COMMA ASSIGNOP RELOP
%token PLUS MINUS STAR DIV
%token AND OR DOT NOT
%token TYPE LP RP LB RB LC RC
%token STRUCT RETURN IF ELSE WHILE

/* declared non-terminals */
%type ExtDefList ExtDef ExtDecList
%type Specifier StructSpecifier OptTag Tag
%type VarDec FunDec VarList ParamDec
%type CompSt StmtList Stmt
%type DefList Def DecList Dec
%type Args
%type Exp

%right ASSIGNOP
%left OR
%left AND 
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP 
%left LB RB 
%left DOT
%nonassoc ELSE
//后面的优先级更大

//%start Exp

%%

Program : ExtDefList {$$=semantic_tree=new_node(@1.first_line,"Program",1,$1);
if(if_over) print_tree($$,0);}
;
ExtDefList : ExtDef ExtDefList {$$=new_node(@1.first_line,
"ExtDefList",2,$1,$2);}
| /* empty */ {$$=NULL;}
;
ExtDef : Specifier ExtDecList SEMI {$$=new_node(@1.first_line,
"ExtDef",3,$1,$2,$3);}
| Specifier SEMI {$$=new_node(@1.first_line,"ExtDef",2,$1,$2);}
| Specifier FunDec CompSt {$$=new_node(@1.first_line,"ExtDef",3,$1,$2,$3);}
| error SEMI {yyerror("wrong expression 1",@1.first_line);}
;
ExtDecList : VarDec {$$=new_node(@1.first_line,"ExtDecList",1,$1);}
| VarDec COMMA ExtDecList {$$=new_node(@1.first_line,"ExtDecList",3,$1,$2,$3);}
| VarDec error SEMI {yyerror("missing ',' 2",@2.first_line);}
;

Specifier : TYPE {$$=new_node(@1.first_line,"Specifier",1,$1);}
| StructSpecifier {$$=new_node(@1.first_line,"Specifier",1,$1);}
;
StructSpecifier : STRUCT OptTag LC DefList RC {$$=new_node(@1.first_line,
"StructSpecifier",5,$1,$2,$3,$4,$5);}
| STRUCT Tag {$$=new_node(@1.first_line,"StructSpecifier",2,$1,$2);}
;
OptTag : ID {$$=new_node(@1.first_line,"OptTag",1,$1);}
| /* empty */ {$$=NULL;}
;
Tag : ID {$$=new_node(@1.first_line,"Tag",1,$1);}
;

VarDec : ID {$$=new_node(@1.first_line,"VarDec",1,$1);}
| VarDec LB INT RB {$$=new_node(@1.first_line,"VarDec",4,$1,$2,$3,$4);}
| VarDec LB error RB {yyerror("wrong expression 3",@3.first_line);}
| VarDec LB INT error {yyerror("missing ']'",@4.first_line);}
;
FunDec : ID LP VarList RP {$$=new_node(@1.first_line,"FunDec",4,$1,$2,$3,$4);}
| ID LP RP {$$=new_node(@1.first_line,"FunDec",3,$1,$2,$3);}
| ID LP error RP {yyerror("wrong expression 4",@3.first_line);}
| error LP VarList RP {yyerror("wrong expression 5",@1.first_line);}
;
VarList : ParamDec COMMA VarList {$$=new_node(@1.first_line,"VarList",3,$1,$2,$3);}
| ParamDec {$$=new_node(@1.first_line,"VarList",1,$1);}
;
ParamDec : Specifier VarDec {$$=new_node(@1.first_line,"ParamDec",2,$1,$2);}
;

CompSt : LC DefList StmtList RC {$$=new_node(@1.first_line,"CompSt",4,
$1,$2,$3,$4);}
;
StmtList : Stmt StmtList {$$=new_node(@1.first_line,"StmtList",2,$1,$2);}
| /* empty */ {$$=NULL;}
Stmt : Exp SEMI {$$=new_node(@1.first_line,"Stmt",2,$1,$2);}
| CompSt {$$=new_node(@1.first_line,"Stmt",1,$1);}
| RETURN Exp SEMI {$$=new_node(@1.first_line,"Stmt",3,$1,$2,$3);}
| IF LP Exp RP Stmt {$$=new_node(@1.first_line,"Stmt",5,$1,$2,$3,$4,$5);}
| IF LP Exp RP Stmt ELSE Stmt {$$=new_node(@1.first_line,"Stmt",7,
$1,$2,$3,$4,$5,$6,$7);}
| WHILE LP Exp RP Stmt {$$=new_node(@1.first_line,"Stmt",5,$1,$2,$3,$4,$5);}
| RETURN Exp error {yyerror("missing ';' 3",@3.first_line);}
| RETURN error SEMI {yyerror("wrong expression 7",@2.first_line);}
| Exp error{yyerror("missing ';' 4",@2.first_line);}
| WHILE LP Exp RP error SEMI{yyerror("wrong expression 7.5",@5.first_line);}
;

DefList : Def DefList {$$=new_node(@1.first_line,"DefList",2,$1,$2);}
| /* empty */ {$$=NULL;}
;
Def : Specifier DecList SEMI {$$=new_node(@1.first_line,"Def",3,$1,$2,$3);}
| error SEMI {yyerror("wrong expression 7.8",@2.first_line);}
;
DecList : Dec {$$=new_node(@1.first_line,"DecList",1,$1);}
| Dec COMMA DecList {$$=new_node(@1.first_line,"DecList",3,$1,$2,$3);}
;
Dec : VarDec {$$=new_node(@1.first_line,"Dec",1,$1);}
| VarDec ASSIGNOP Exp {$$=new_node(@1.first_line,"Dec",3,$1,$2,$3);}
;

Exp : Exp ASSIGNOP Exp {$$=new_node(@1.first_line,"Exp",3,$1,$2,$3);}
| Exp AND Exp {$$=new_node(@1.first_line,"Exp",3,$1,$2,$3);}
| Exp OR Exp {$$=new_node(@1.first_line,"Exp",3,$1,$2,$3);}
| Exp RELOP Exp {$$=new_node(@1.first_line,"Exp",3,$1,$2,$3);}
| Exp PLUS Exp {$$=new_node(@1.first_line,"Exp",3,$1,$2,$3);}
| Exp MINUS Exp {$$=new_node(@1.first_line,"Exp",3,$1,$2,$3);}
| Exp STAR Exp {$$=new_node(@1.first_line,"Exp",3,$1,$2,$3);}
| Exp DIV Exp {$$=new_node(@1.first_line,"Exp",3,$1,$2,$3);}
| LP Exp RP {$$=new_node(@1.first_line,"Exp",3,$1,$2,$3);}
| MINUS Exp {$$=new_node(@1.first_line,"Exp",2,$1,$2);}
| NOT Exp {$$=new_node(@1.first_line,"Exp",2,$1,$2);}
| ID LP Args RP {$$=new_node(@1.first_line,"Exp",4,$1,$2,$3,$4);}
| ID LP RP {$$=new_node(@1.first_line,"Exp",3,$1,$2,$3);}
| Exp LB Exp RB {$$=new_node(@1.first_line,"Exp",4,$1,$2,$3,$4);}
| Exp DOT ID {$$=new_node(@1.first_line,"Exp",3,$1,$2,$3);}
| ID {$$=new_node(@1.first_line,"Exp",1,$1);}
| INT {$$=new_node(@1.first_line,"Exp",1,$1);}
| FLOAT {$$=new_node(@1.first_line,"Exp",1,$1);}
| Exp ASSIGNOP error {yyerror("wrong expression 9",@3.first_line);}
| Exp AND error {yyerror("wrong expression 10",@3.first_line);}
| Exp OR error {yyerror("wrong expression 11",@3.first_line);}
| Exp RELOP error {yyerror("wrong expression 12",@3.first_line);}
| Exp PLUS error {yyerror("wrong expression 13",@3.first_line);}
| Exp MINUS error {yyerror("wrong expression 14",@3.first_line);}
| Exp STAR error {yyerror("wrong expression 15",@3.first_line);}
| Exp DIV error {yyerror("wrong expression 16",@3.first_line);}
| LP error RP {yyerror("wrong expression 17",@2.first_line);}
| MINUS error {yyerror("wrong expression 18",@2.first_line);}
| NOT error {yyerror("wrong expression 19",@2.first_line);}
| ID LP error RP {yyerror("wrong expression 20",@3.first_line);}
| Exp LB error RB {yyerror("wrong expression 21",@3.first_line);}
;
Args : Exp COMMA Args {$$=new_node(@1.first_line,"Args",3,$1,$2,$3);}
| Exp {$$=new_node(@1.first_line,"Args",1,$1);}
;

%%

void debug(char*str){
    #ifdef DEBUG
    printf("at line %d, %s\n",yylineno,str);
    #endif
}

void yyerror(char* msg, ...){
    if(strcmp("syntax error",msg)==0) return;
    va_list args;
    va_start(args,msg);
    int tmp=va_arg(args,int);
    printf("Error type B at line %d: %s\n",tmp,msg);
    if_over=0;
    exit(0);
}

/*
Program = ExtDefList
ExtDefList = ExtDef ExtDefList | empty
ExtDef = Specifier ExtDecList SEMI | Specifier SEMI | Specifier FunDec CompSt
//程序=变量，结构体，函数结合
ExtDecList = VarDec | VarDec COMMA ExtDecList | empty //简单变量具体定义
Specifier = TYPE | StructSpecifier //简单变量类型|结构体定义
StructSpecifier = STRUCT OptTag LC DefList RC | STRUCT Tag
OptTag = ID | empty
Tag = ID
VarDec = ID | VarDec LB INT RB
FunDec = ID LP VarList RP| ID LP RP //函数及参数列表
VarList = ParamDec COMMA VarList| ParamDec
ParamDec = Specifier VarDec
CompSt = LC DefList StmtList RC //函数具体定义，先声明后使用
StmtList = Stmt StmtList | empty
Stmt = Exp SEMI| CompSt| RETURN Exp SEMI | IF LP Exp RP Stmt
|IF LP Exp RP Stmt ELSE Stmt | WHILE LP Exp RP Stmt //使用
DefList = Def DefList | empty
Def = Specifier DecList SEMI//变量声明
DecList = Dec | Dec COMMA DecList
Dec = VarDec | VarDec ASSIGNOP Exp
Exp = Exp ASSIGNOP Exp| Exp AND Exp | Exp OR Exp
| Exp RELOP Exp | Exp PLUS Exp | Exp MINUS Exp
| Exp STAR Exp | Exp DIV Exp | LP Exp RP | MINUS Exp
| NOT Exp | ID LP Args RP | ID LP RP | Exp LB Exp RB
| Exp DOT ID | ID | INT | FLOAT
Args = Exp COMMA Args | Exp

*/