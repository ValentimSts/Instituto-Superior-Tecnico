#ifndef __MML_AST_FUNCTION_DEFINITION_NODE_H__
#define __MML_AST_FUNCTION_DEFINITION_NODE_H__

#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/typed_node.h>
#include <cdk/types/basic_type.h>
#include "block_node.h"

namespace mml {

	/**
	 * Class for describing function definitions.
	 *  - (arg1, arg2, ...) -> return_type { block }
	 */
	class function_definition_node: public cdk::expression_node {
		cdk::sequence_node *_arguments;
		mml::block_node *_block;
		bool _main; // Tells us if the function being defined is the main function.

	public:
		inline function_definition_node(int lineno, cdk::sequence_node *arguments, std::shared_ptr<cdk::basic_type> return_type, mml::block_node *block, bool main) :
				cdk::expression_node(lineno), _arguments(arguments), _block(block), _main(main) {
			cdk::expression_node::type(return_type);
		}

	public:
		cdk::typed_node* argument(size_t i) const {
			return dynamic_cast<cdk::typed_node*>(_arguments->node(i));
		}

		inline cdk::sequence_node *arguments() {
			return _arguments;
		}

		inline std::shared_ptr<cdk::basic_type> return_type() {
			return cdk::expression_node::type();
		}

		inline mml::block_node *block() {
			return _block;
		}

		inline bool main() const {
			return _main;
		}

		void accept(basic_ast_visitor *sp, int level) {
			sp->do_function_definition_node(this, level);
		}

	};

} // mml

#endif