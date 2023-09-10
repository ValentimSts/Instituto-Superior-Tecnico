#ifndef __MMLSCANNER_H__
#define __MMLSCANNER_H__

#undef yyFlexLexer
#define yyFlexLexer mml_scanner_FlexLexer
#include <FlexLexer.h>

typedef mml_scanner_FlexLexer mml_scanner;

#endif
