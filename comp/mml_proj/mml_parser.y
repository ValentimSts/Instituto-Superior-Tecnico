%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <vector>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include ".auto/all_nodes.h"
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
//-- don't change *any* of these --- END!
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  //-- don't change *any* of these --- END!

  int                   i;	/* integer value */
  double                d;  /* double value */
  std::string          *s;	/* symbol name or string literal */

  cdk::basic_node      *node;	      /* node pointer */
  cdk::sequence_node   *sequence;   /* sequence nodes */
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;     /* lvalue nodes */

  mml::block_node                *block;       /* block nodes */
  mml::function_definition_node  *fundef;      /* function definition nodes */
  mml::variable_declaration_node *decl;        /* variable declaration nodes */

  std::vector<std::shared_ptr<cdk::basic_type>> *types; /* type vector */
};

%token tARROW tGE tLE tEQ tNE tAND tOR 
%token tPUBLIC tPRIVATE tFORWARD tFOREIGN
%token tNULL
%token tTYPE_INT tTYPE_DOUBLE tTYPE_STRING tTYPE_VOID tTYPE_AUTO
%token tINPUT tSIZEOF tPRINT tPRINTLN tBEGIN tEND
%token tIF tELIF tELSE
%token tWHILE
%token tSTOP tRETURN tNEXT

%token<i> tINTEGER 
%token<d> tDOUBLE
%token<s> tSTRING tIDENTIFIER

%type<block> block main_block
%type<fundef> program fundef
%type<decl> file_declaration declaration vardec variable
%type<node> instruction elif stop next return with_update_node
%type<sequence> opt_file_decl opt_program file file_declarations opt_declarations declarations variables opt_instructions instructions opt_expressions expressions
%type<expression> expression initializer integer double
%type<lvalue> lvalue
%type<s> string
%type<type> data_type func_type
%type<types> data_types 


%nonassoc tIFX
%nonassoc tELIF tELSE 

%right '=' 
%left tOR
%left tAND

%nonassoc '~'

%left tEQ tNE
%left tGE tLE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%left '?'

%right tUNARY 

%nonassoc '(' '['
%nonassoc ')' ']'
%nonassoc ','
%nonassoc ';'

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file             : opt_file_decl opt_program { compiler->ast($$ = new cdk::sequence_node(LINE, $2, $1)); }
                 ;

opt_program      : /* empty */  { $$ = new cdk::sequence_node(LINE); }
                 | program      { $$ = new cdk::sequence_node(LINE, $1); }
                 ;

program          : tBEGIN main_block tEND { $$ = new mml::function_definition_node(LINE, nullptr, cdk::primitive_type::create(4, cdk::TYPE_INT), $2, true); }
                 ;

main_block       : opt_declarations opt_instructions { $$ = new mml::block_node(LINE, $1, $2); }
                 ;

opt_file_decl    : /* empty */        { $$ = new cdk::sequence_node(LINE); }
                 | file_declarations  { $$ = $1; }
                 ;

file_declarations:                   file_declaration { $$ = new cdk::sequence_node(LINE, $1); }
                 | file_declarations file_declaration { $$ = new cdk::sequence_node(LINE, $2, $1); }
                 ;

file_declaration : tPUBLIC  data_type  tIDENTIFIER initializer ';'    { $$ = new mml::variable_declaration_node(LINE, tPUBLIC, $2, *$3, $4); }
                 /* Auto declarations */
                 | tPUBLIC  tTYPE_AUTO tIDENTIFIER '=' expression ';' { $$ = new mml::variable_declaration_node(LINE, tPUBLIC, nullptr, *$3, $5); }
                 | tPUBLIC             tIDENTIFIER '=' expression ';' { $$ = new mml::variable_declaration_node(LINE, tPUBLIC, nullptr, *$2, $4); }
                 |                     tIDENTIFIER '=' expression ';' { $$ = new mml::variable_declaration_node(LINE, tPUBLIC, nullptr, *$1, $3); }
                 /* External declarations */
                 | tFORWARD data_type  tIDENTIFIER ';'                { $$ = new mml::variable_declaration_node(LINE, tFORWARD, $2, *$3, nullptr); }
                 | tFOREIGN data_type  tIDENTIFIER ';'                { $$ = new mml::variable_declaration_node(LINE, tFOREIGN, $2, *$3, nullptr); }
                 /* Variable declarations */
                 | declaration                                        { $$ = $1; }
                 ;

opt_declarations : /* empty */  { $$ = new cdk::sequence_node(LINE); }
                 | declarations { $$ = $1; }
                 ;

declarations     :              declaration { $$ = new cdk::sequence_node(LINE, $1); }
                 | declarations declaration { $$ = new cdk::sequence_node(LINE, $2, $1); }
                 ;
      
declaration      : vardec ';' { $$ = $1; }
                 ;


 /* ====== Variable declarations ====== */

vardec           : data_type  tIDENTIFIER initializer    { $$ = new mml::variable_declaration_node(LINE, tPRIVATE, $1, *$2, $3); delete $2; }
                 /* Auto declarations */
                 | tTYPE_AUTO tIDENTIFIER '=' expression { $$ = new mml::variable_declaration_node(LINE, tPUBLIC, nullptr, *$2, $4); delete $2; }
                 ;

data_types       : data_type                { $$ = new std::vector<std::shared_ptr<cdk::basic_type>>(); $$->push_back($1); }
                 | data_types ',' data_type { $$ = $1; $$->push_back($3); }
                 ;

data_type        : tTYPE_INT                    { $$ = cdk::primitive_type::create(4, cdk::TYPE_INT); }
                 | tTYPE_STRING                 { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
                 | tTYPE_DOUBLE                 { $$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }
                 | '[' tTYPE_VOID ']'           { $$ = cdk::reference_type::create(4, cdk::primitive_type::create(4, cdk::TYPE_VOID)); }  
                 | '[' data_type ']'            { 
                                                  /* Any number of void pointers is equivalent to a single void pointer. */
                                                  if ($2->name() == cdk::TYPE_POINTER && cdk::reference_type::cast($2)->referenced()->name() == cdk::TYPE_VOID) {
                                                    $$ = cdk::reference_type::create(4, cdk::primitive_type::create(4, cdk::TYPE_VOID)); 
                                                  } 
                                                  else {
                                                    $$ = cdk::reference_type::create(4, $2);     
                                                  }
                                                }
                 | func_type                    { $$ = $1; }
                 ;

func_type        : data_type  '<' data_types '>' { $$ = cdk::functional_type::create(*$3, $1); delete $3; }
                 | data_type  '<'            '>' { $$ = cdk::functional_type::create($1); }
                 | tTYPE_VOID '<'            '>' { $$ = cdk::functional_type::create(cdk::primitive_type::create(4, cdk::TYPE_VOID)); }
                 | tTYPE_VOID '<' data_types '>' { $$ = cdk::functional_type::create(*$3, cdk::primitive_type::create(4, cdk::TYPE_VOID)); delete $3; }
                 ;
       
initializer      : /* empty */    { $$ = nullptr; /* must be nullptr, not NIL */ }
                 | '=' expression { $$ = $2; }
                 ;


 /* ====== Function definitions ====== */

fundef           : '(' variables ')' tARROW data_type  block { $$ = new mml::function_definition_node(LINE, $2, $5, $6, false); }
                 | '(' variables ')' tARROW tTYPE_VOID block { $$ = new mml::function_definition_node(LINE, $2, cdk::primitive_type::create(4, cdk::TYPE_VOID), $6, false); }
                 ;

variables        : /* empty */             { $$ = new cdk::sequence_node(LINE); }
                 |               variable  { $$ = new cdk::sequence_node(LINE, $1); }
                 | variables ',' variable  { $$ = new cdk::sequence_node(LINE, $3, $1); }
                 ;

variable         : data_type tIDENTIFIER { $$ = new mml::variable_declaration_node(LINE, tPRIVATE, $1, *$2, nullptr); delete $2; }
                 ;

block            : '{' opt_declarations opt_instructions '}' { $$ = new mml::block_node(LINE, $2, $3); }
                 ;

opt_instructions : /* empty */  { $$ = new cdk::sequence_node(LINE); }
                 | instructions { $$ = $1; }
                 ;

instructions     :              instruction { $$ = new cdk::sequence_node(LINE, $1); }
                 | instructions instruction { $$ = new cdk::sequence_node(LINE, $2, $1); }
                 ;

instruction      : expression ';'                                { $$ = new mml::evaluation_node(LINE, $1); }
                 | expressions tPRINT                            { $$ = new mml::print_node(LINE, $1, false); }
                 | expressions tPRINTLN                          { $$ = new mml::print_node(LINE, $1, true); }
                 | with_update                                   { $$ = $1; }
                 | stop                                          { $$ = $1; }
                 | next                                          { $$ = $1; }
                 | return                                        { $$ = $1; }
                 | tIF '(' expression ')' instruction %prec tIFX { $$ = new mml::if_node(LINE, $3, $5); }
                 | tIF '(' expression ')' instruction elif       { $$ = new mml::if_else_node(LINE, $3, $5, $6); }
                 | tWHILE '(' expression ')' instruction         { $$ = new mml::while_node(LINE, $3, $5); }
                 | block                                         { $$ = $1; }
                 ;

with_update      : tWITH expression tUPDATE expression ':' expression tDOTDOT expression { $$ = new mml::with_update_node(LINE, $2, $4, $6, $8); }
                 ;
 
stop             : tSTOP          ';' { $$ = new mml::stop_node(LINE); }
                 | tSTOP tINTEGER ';' { $$ = new mml::stop_node(LINE, $2); }
                 ;

next             : tNEXT          ';' { $$ = new mml::next_node(LINE); }
                 | tNEXT tINTEGER ';' { $$ = new mml::next_node(LINE, $2); }
                 ;

return           : tRETURN             ';' { $$ = new mml::return_node(LINE); }
                 | tRETURN expression  ';' { $$ = new mml::return_node(LINE, $2); }
                 ;

elif             : tELSE                    instruction            { $$ = $2; }
                 | tELIF '(' expression ')' instruction %prec tIFX { $$ = new mml::if_node(LINE, $3, $5); }
                 | tELIF '(' expression ')' instruction elif       { $$ = new mml::if_else_node(LINE, $3, $5, $6); }
                 ;


 /* ====== Expressions ====== */

opt_expressions  : /* empty */ { $$ = new cdk::sequence_node(LINE); }
                 | expressions { $$ = $1; }
                 ;

expressions      :                 expression { $$ = new cdk::sequence_node(LINE, $1); }
                 | expressions ',' expression { $$ = new cdk::sequence_node(LINE, $3, $1); }
                 ;

expression       : integer                             { $$ = $1; }
                 | double                              { $$ = $1; }
                 | string                              { $$ = new cdk::string_node(LINE, $1); }
                 | tNULL                               { $$ = new mml::nullptr_node(LINE); }
                 /* LEFT VALUES */
                 | lvalue                              { $$ = new cdk::rvalue_node(LINE, $1); }
                 /* ASSIGNMENTS */
                 | lvalue '=' expression               { $$ = new cdk::assignment_node(LINE, $1, $3); }
                 /* ARITHMETIC EXPRESSIONS */
                 | expression '+' expression           { $$ = new cdk::add_node(LINE, $1, $3); }
                 | expression '-' expression           { $$ = new cdk::sub_node(LINE, $1, $3); }
                 | expression '*' expression           { $$ = new cdk::mul_node(LINE, $1, $3); }
                 | expression '/' expression           { $$ = new cdk::div_node(LINE, $1, $3); }
                 | expression '%' expression           { $$ = new cdk::mod_node(LINE, $1, $3); }
                 /* LOGICAL EXPRESSIONS */
                 | expression  '<' expression          { $$ = new cdk::lt_node(LINE, $1, $3); }
                 | expression tLE  expression          { $$ = new cdk::le_node(LINE, $1, $3); }
                 | expression tEQ  expression          { $$ = new cdk::eq_node(LINE, $1, $3); }
                 | expression tGE  expression          { $$ = new cdk::ge_node(LINE, $1, $3); }
                 | expression  '>' expression          { $$ = new cdk::gt_node(LINE, $1, $3); }
                 | expression tNE  expression          { $$ = new cdk::ne_node(LINE, $1, $3); }
                 /* LOGICAL EXPRESSIONS */
                 | expression tAND  expression         { $$ = new cdk::and_node(LINE, $1, $3); }
                 | expression tOR   expression         { $$ = new cdk::or_node (LINE, $1, $3); }
                 /* UNARY EXPRESSION */
                 | '-' expression %prec tUNARY         { $$ = new cdk::neg_node(LINE, $2); }
                 | '+' expression %prec tUNARY         { $$ = new mml::identity_node(LINE, $2); }
                 | '~' expression                      { $$ = new cdk::not_node(LINE, $2); }
                 /* OTHER EXPRESSION */
                 | tINPUT                              { $$ = new mml::input_node(LINE); }
                 /* FUNC DEFINITIONS */
                 | fundef                              { $$ = $1; }
                 /* FUNC CALLS*/
                 | '@' '(' opt_expressions ')'         { $$ = new mml::function_call_node(LINE, $3, nullptr); }
                 | expression '(' opt_expressions ')'  { $$ = new mml::function_call_node(LINE, $3, $1); }
                 | tSIZEOF '(' expression ')'          { $$ = new mml::sizeof_node(LINE, $3); }
                 /* OTHER EXPRESSION */
                 | '(' expression ')'                  { $$ = $2; }
                 | '[' expression ']'                  { $$ = new mml::stack_alloc_node(LINE, $2); }
                 | lvalue '?'                          { $$ = new mml::address_of_node(LINE, $1); }
                 ;

lvalue           : tIDENTIFIER                   { $$ = new cdk::variable_node(LINE, *$1); delete $1; }
                 | expression '[' expression ']' { $$ = new mml::index_node(LINE, $1, $3); }
                 ;

integer          : tINTEGER { $$ = new cdk::integer_node(LINE, $1); }
                 ;

double           : tDOUBLE { $$ = new cdk::double_node(LINE, $1); }
                 ;

string           : tSTRING        { $$ = $1; }
                 | string tSTRING { $$ = $1; $$->append(*$2); delete $2; }
                 ;

%%
