#ifndef __MML_AST_PRINT_NODE_H__
#define __MML_AST_PRINT_NODE_H__

#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>

namespace mml {

  /**
   * Class for describing print nodes.
   *  - expr, expr, ...! | !!
   */
  class print_node: public cdk::basic_node {
    cdk::sequence_node *_arguments;
    bool _newline; // distinguishes !! (print with \n) from ! (print without \n).

  public:
    inline print_node(int lineno, cdk::sequence_node *arguments, bool newline) :
        cdk::basic_node(lineno), _arguments(arguments), _newline(newline) {
    }

  public:
    inline cdk::expression_node *argument(size_t i) {
      return dynamic_cast<cdk::expression_node*>(_arguments->node(i));
    }

    inline cdk::sequence_node *arguments() {
      return _arguments;
    }

    inline bool newline() const {
      return _newline;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_print_node(this, level);
    }

  };

} // mml

#endif
