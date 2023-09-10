#ifndef __MML_AST_VARIABLE_DECLARATION_H__
#define __MML_AST_VARIABLE_DECLARATION_H__

#include <string>
#include <cdk/ast/typed_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/types/basic_type.h>

namespace mml {

    /**
     * Class for describing variable declarations.
     *  - qualifier type identifier = initializer 
     */
    class variable_declaration_node: public cdk::typed_node {
        int _qualifier; // qualifier is private by default.
        std::string _identifier;
        cdk::expression_node *_initializer;

    public:
        inline variable_declaration_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> varType, const std::string &identifier,
                                    cdk::expression_node *initializer) :
            cdk::typed_node(lineno), _qualifier(qualifier), _identifier(identifier), _initializer(initializer) {
            type(varType);
        }

    public:
        inline int qualifier() const {
            return _qualifier;
        }

        inline const std::string& identifier() const {
            return _identifier;
        }

        inline cdk::expression_node* initializer() {
            return _initializer;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_variable_declaration_node(this, level);
        }

    };

} // mml

#endif
