#ifndef __MML_AST_WITH_UPDATE_NODE_H__
#define __MML_AST_WITH_UPDATE_NODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/expression_node.h>

namespace mml {

    class with_update_node: public cdk::basic_node {
        cdk::expression_node *_function;
        cdk::expression_node *_vector;
        cdk::expression_node *_low;
        cdk::expression_node *_high;

    public:
        inline with_update_node(int lineno, cdk::expression_node *function, cdk::expression_node *vector,
                                            cdk::expression_node *low, cdk::expression_node *high) :
            cdk::basic_node(lineno), _function(function), _vector(vector), _low(low), _high(high) {
        }

    public:
        inline cdk::expression_node *function() {
            return _function;
        }

        inline cdk::expression_node *vector() {
            return _vector;
        }

        inline cdk::expression_node *low() {
            return _low;
        }

        inline cdk::expression_node *high() {
            return _high;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_with_update_node(this, level);
        }

    };

} // mml

#endif
