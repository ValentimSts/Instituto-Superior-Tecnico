#ifndef __MML_AST_FUNCTION_CALL_NODE_H__
#define __MML_AST_FUNCTION_CALL_NODE_H__

#include <string>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>

namespace mml {

    /**
     * Class for describing function call nodes.
     *  - func(arg1, arg2, ...)
     */
    class function_call_node: public cdk::expression_node {
        cdk::sequence_node *_arguments;
        cdk::expression_node *_func;

    public:
        /**
         * Constructor for a function call without arguments.
         */
        inline function_call_node(int lineno, cdk::expression_node *func) :
                cdk::expression_node(lineno), _arguments(new cdk::sequence_node(lineno)), _func(func) {
        }

        /**
         * Constructor for a function call with arguments.
         */
        inline function_call_node(int lineno, cdk::sequence_node *arguments, cdk::expression_node *func) :
                cdk::expression_node(lineno), _arguments(arguments), _func(func) {
        }
    
    public:
        inline cdk::expression_node *argument(size_t i) {
            return dynamic_cast<cdk::expression_node*>(_arguments->node(i));
        }
        
        inline cdk::sequence_node *arguments() {
            return _arguments;
        }

        inline cdk::expression_node *func() {
            return _func;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_function_call_node(this, level);
        }
        
    };

} // mml


#endif