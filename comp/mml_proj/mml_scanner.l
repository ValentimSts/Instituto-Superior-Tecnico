%option c++ prefix="mml_scanner_" outfile="mml_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "mml_parser.tab.h"

// output stream for building string literals
static std::ostringstream strlit;

// don't change this
#define yyerror LexerError
%}

SPACE      ([ \t\r]|\n)
DIG        [[:digit:]]
LETTER     [A-Za-z]

INT          {DIG}+
IDENTIFIER   {LETTER}({DIG}|{LETTER})*

%x X_COMMENT
%x X_OCTAL
%x X_STRING X_BACKSLASH X_STRING_TERMINATOR
%%
  yydebug=1; set_debug(1);

 /*
  * =============================
  *           Comments           
  * =============================
  */

"//".*$                /* match the whole line (it is ignored) */

"/*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"/*"        yy_push_state(X_COMMENT);
<X_COMMENT>"*/"        yy_pop_state();
<X_COMMENT>.|"\n"      ;  /* ignore inside comments */


 /*
  * ==========================================
  *               Data Types 
  *  (pointers are processed in the parser.)      
  * ==========================================
  */

"int"                    return tTYPE_INT;
"double"                 return tTYPE_DOUBLE;
"string"                 return tTYPE_STRING;
"void"                   return tTYPE_VOID;

"auto"                   return tTYPE_AUTO;

"null"                   return tNULL;


 /*
  * =============================
  *        Global Symbols         
  * =============================
  */
  
"public"                 return tPUBLIC;
"forward"                return tFORWARD;
"foreign"                return tFOREIGN;


 /*
  * =============================
  *         Instructions 
  * =============================
  */

"with"                return tWITH;
"update"               return tUPDATE;

"input"                return tINPUT;
"sizeof"               return tSIZEOF;
"!"                    return tPRINT;
"!!"                   return tPRINTLN; 
"begin"                return tBEGIN; 
"end"                  return tEND; 

 /* Conditional */
"if"                   return tIF;
"elif"                 return tELIF;
"else"                 return tELSE;

 /* Iteration */
"while"                return tWHILE;

 /* Break */
"stop"                 return tSTOP;

 /* Return */
"return"               return tRETURN;

 /* Continue */
"next"                 return tNEXT;


 /* 
  * ============================= 
  *     Expression Operators        
  * ============================= 
  */

[-+*/%~<>@?!=:]          return *yytext;

".."                   return tDOTDOT;

"->"                   return tARROW;
">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE;
"&&"                   return tAND;
"||"                   return tOR;


 /*
  * =============================
  *   Delimiters and Separators
  * =============================
  */

[,;(){}\[\]]                  return *yytext;


 /*
  * ===============================
  *        Integers
  *           - Octals
  *           - Decimals
  * ===============================
  */

0[0-7]+                       {
                                try {
                                  yylval.i = std::stoi(yytext, NULL, 8);
                                  return tINTEGER;
                                }
                                catch (const std::out_of_range& e) {
                                  yyerror("integer out of range.");
                                }
                              }
0[0-9]+                       yyerror("invalid octal.");

0|[1-9][0-9]*                 {
                                try {
                                  yylval.i = std::stoi(yytext, NULL, 10);
                                  return tINTEGER;
                                }
                                catch (const std::out_of_range& e) {
                                  yyerror("integer out of range.");
                                }
                              }


 /*
  * =============================
  *         Real numbers 
  * =============================
  */

([0-9]+\.[0-9]*)([eE][-+]?[0-9]+)?  {
                                      try {
                                        yylval.d = std::stod(yytext);
                                        return tDOUBLE;
                                      }
                                      catch (const std::out_of_range& e) {
                                        yyerror("double out of range.");
                                      }
                                    }

([0-9]*\.[0-9]+)([eE][-+]?[0-9]+)?  {
                                      try {
                                        yylval.d = std::stod(yytext);
                                        return tDOUBLE;
                                      }
                                      catch (const std::out_of_range& e) {
                                        yyerror("double out of range.");
                                      }
                                    }
                                    
[0-9]+([Ee][-+]?[0-9]+)             {
                                      try {
                                        yylval.d = std::stod(yytext);
                                        return tDOUBLE;
                                      }
                                      catch (const std::out_of_range& e) {
                                        yyerror("double out of range.");
                                      }
                                    }


 /*
  * =============================
  *         Identifiers
  * =============================
  */

{IDENTIFIER}  yylval.s = new std::string(yytext); return tIDENTIFIER;


 /*
  * =============================
  *     Strings / Octals        
  * =============================
  */

"\""                           yy_push_state(X_STRING);
<X_STRING>\\                   yy_push_state(X_BACKSLASH);
<X_STRING>"\""                 {
                                 yylval.s = new std::string(strlit.str());
                                 strlit.str("");
                                 yy_pop_state();
                                 return tSTRING;
                               }


<X_STRING>\0                   yyerror("null character in string.");
<X_STRING>\n                   yyerror("newline in string.");
<X_STRING>.                    strlit << *yytext;

<X_BACKSLASH>"\""              strlit << '\"'; yy_pop_state();
<X_BACKSLASH>t                 strlit << '\t'; yy_pop_state();
<X_BACKSLASH>n                 strlit << '\n'; yy_pop_state();
<X_BACKSLASH>r                 strlit << '\r'; yy_pop_state();
<X_BACKSLASH>\\                strlit << '\\'; yy_pop_state();

 /* ======= Octals (BEGIN) ======= */

<X_BACKSLASH>[0-7]{1,3}           {
                                   unsigned long octal = std::stoul(yytext, NULL, 8);
                                   
                                   if (octal > 255) {
                                     yyerror("invalid octal.");
                                   }
                                   strlit << (char)(unsigned char)octal;

                                   yy_pop_state();
                                  }

 /* ======== Octals (END) ======== */

<X_BACKSLASH>0.                {
                                 yylval.s = new std::string(strlit.str());
                                 strlit.str("");
                                 yy_pop_state();
                                 yy_pop_state();
                                 yy_push_state(X_STRING_TERMINATOR);
                               }


 /* Ignores every character (inside the string) that comes after the '\0'. */      
<X_STRING_TERMINATOR>"\""      yy_pop_state(); return tSTRING;
<X_STRING_TERMINATOR>.         ;

<X_BACKSLASH>.                 strlit << *yytext; yy_pop_state();


 /*
  * =============================
  *       Other characters      
  * =============================
  */

[ \t\n]+               ; /* ignore whitespace */
.                      yyerror("unknown character");

%%
