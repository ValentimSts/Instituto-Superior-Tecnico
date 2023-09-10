#ifndef __MML_AST_SIZEOF_H__
#define __MML_AST_SIZEOF_H__

#include <cdk/ast/expression_node.h>

namespace mml {

  /**
   * Class for describing sizeof nodes.
   */
  class sizeof_node: public cdk::expression_node {
    cdk::expression_node *_expression;

  public:
    inline sizeof_node(int lineno, cdk::expression_node *expression = nullptr) :
        cdk::expression_node(lineno), _expression(expression) {
    }

  public:
    inline cdk::expression_node* expression() {
      return _expression;
    }
    
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_sizeof_node(this, level);
    }

  };

} // mml

#endif
