#include <string>
#include <vector>
#include "targets/type_checker.h"
#include ".auto/all_nodes.h"  // automatically generated
#include <cdk/types/primitive_type.h>

#include <mml_parser.tab.h>

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

/**
 * Checks if the pointer type 't2' can assigned to a variable of
 * the pointer type 't1'.
 */
static bool acceptable_pointer_type(std::shared_ptr<cdk::basic_type> t1, std::shared_ptr<cdk::basic_type> t2) {
  auto type1 = t1;
  auto type2 = t2;

  while (true) {
    // Any pointer can be assigned to nullptr.
    if (type2 == nullptr) {
      return true;
    }

    if (type1->name() == cdk::TYPE_POINTER && type2->name() == cdk::TYPE_POINTER) {
      type1 = cdk::reference_type::cast(type1)->referenced();
      type2 = cdk::reference_type::cast(type2)->referenced();
    }
    else {
      break;
    }
  }

  return type1->name() == type2->name();
}


/**
 * Checks if the functional type 't2' can assigned to a variable of
 * the functional type 't1'.
 */
static bool acceptable_functional_type(std::shared_ptr<cdk::functional_type> t1, std::shared_ptr<cdk::functional_type> t2) {
  /* Check return type */
  auto t1_ret_type = t1->output(0);
  auto t2_ret_type = t2->output(0);

  auto  t1_ret_name = t1_ret_type->name();
  auto  t2_ret_name = t2_ret_type->name();

  // Pointer type.
  if (t1_ret_name == cdk::TYPE_POINTER) {
    if (!(t2_ret_name == cdk::TYPE_POINTER && acceptable_pointer_type(t1_ret_type, t2_ret_type))) {
      return false;
    }
  }
  // Functional type.
  else if (t1_ret_name == cdk::TYPE_FUNCTIONAL) {
    if (!(t2_ret_name == cdk::TYPE_FUNCTIONAL &&
           acceptable_functional_type(cdk::functional_type::cast(t1_ret_type), cdk::functional_type::cast(t2_ret_type)))) {
      return false;
    }
  }
  // Doubles (ints can can be used in place of doubles).
  else if (t1_ret_name == cdk::TYPE_DOUBLE) {
    if (!(t2_ret_name == cdk::TYPE_DOUBLE || t2_ret_name == cdk::TYPE_INT)) {
      return false;
    }
  }
  // Other primitive types.
  else {
    if (t1_ret_name != t2_ret_name) {
      return false;
    }
  }

  /* Check input types. */
  if (t1->input_length() != t2->input_length()) {
    return false;
  }

  for (size_t i = 0; i < t2->input_length(); i++) {
    // Pointer type.
    if (t1->input(i)->name() == cdk::TYPE_POINTER) {
      if (!(t2->input(i)->name() == cdk::TYPE_POINTER && acceptable_pointer_type(t1->input(i), t2->input(i)))) {
        return false;
      }
    }
    // Functional type.
    else if (t1->input(i)->name() == cdk::TYPE_FUNCTIONAL) {
      if (!(t2->input(i)->name() == cdk::TYPE_FUNCTIONAL &&
             acceptable_functional_type(cdk::functional_type::cast(t1->input(i)), cdk::functional_type::cast(t2->input(i))))) {
        return false;
      }
    }
    // Doubles (ints can can be used in place of doubles).
    else if (t1->input(i)->name() == cdk::TYPE_DOUBLE) {
      if (!(t2->input(i)->name() == cdk::TYPE_DOUBLE || t2->input(i)->name() == cdk::TYPE_INT)) {
        return false;
      }
    }
    // Other primitive types.
    else {
      if (t1->input(i)->name() != t2->input(i)->name()) {
        return false;
      }
    }
  }

  return true;
}

//---------------------------------------------------------------------------

void mml::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
  // EMPTY
}
void mml::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------
//-------------------> LITERALS / ARITHMETIC EXPRESSIONS <-------------------
//---------------------------------------------------------------------------

void mml::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void mml::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

void mml::type_checker::do_nullptr_node(mml::nullptr_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::reference_type::create(4, nullptr));
}

// New implementation:
void mml::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
}

//---------------------------------------------------------------------------

void mml::type_checker::processUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in argument of unary expression");

  // in MML, expressions are always int
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void mml::type_checker::do_neg_node(cdk::neg_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

// New implementations:
void mml::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

void mml::type_checker::do_identity_node(mml::identity_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void mml::type_checker::processBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->left()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER)) { 
    node->type(node->right()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && !node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->type(node->right()->type());
    node->left()->type(node->right()->type());
  }
  else if (!node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->type(node->left()->type());
    node->right()->type(node->left()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));

    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else {
    throw std::string("wrong types in binary expression");
  }
}

void mml::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void mml::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void mml::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void mml::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void mml::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void mml::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void mml::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void mml::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void mml::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void mml::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void mml::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}

// New implementations:
void mml::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void mml::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<mml::symbol> symbol = _symtab.find(id);

  if (symbol != nullptr) {
    node->type(symbol->type());
  } else {
    throw id;
  }
}

void mml::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC;
  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  }
  catch (const std::string &id) {
    throw "undeclared variable '" + id + "'";
  }
}

void mml::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->lvalue()->accept(this, lvl + 4);
  node->rvalue()->accept(this, lvl + 4);

  // Integers
  if (node->lvalue()->is_typed(cdk::TYPE_INT)) {

    if (node->rvalue()->is_typed(cdk::TYPE_INT)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    }
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {    
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    }
    else {
      throw std::string("wrong type in assignment to integer");
    }

  }
  // Doubles
  else if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE)) {

    // Ints can be used in place of doubles
    if (node->rvalue()->is_typed(cdk::TYPE_DOUBLE) ||
        node->rvalue()->is_typed(cdk::TYPE_INT)) {
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    }
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {            
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
      node->rvalue()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    }
    else {
      throw std::string("wrong type in assignment to double");
    }

  }
  // Strings
  else if (node->lvalue()->is_typed(cdk::TYPE_STRING)) {

    if (node->rvalue()->is_typed(cdk::TYPE_STRING)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
    }
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {           
      node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
    }
    else {
      throw std::string("wrong type in assignment to string");
    }

  }
  else if (node->lvalue()->is_typed(cdk::TYPE_POINTER)) {

    if (node->rvalue()->is_typed(cdk::TYPE_POINTER)) {
      if (!(acceptable_pointer_type(node->lvalue()->type(), node->rvalue()->type()))) {
        throw std::string("wrong assignment to pointer.");
      }

      node->type(node->rvalue()->type());
    }
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {             
      node->type(cdk::primitive_type::create(4, cdk::TYPE_ERROR));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_ERROR));
    }
    else {
      throw std::string("wrong type in assignment to pointer");
    }

  }
  else if (node->lvalue()->is_typed(cdk::TYPE_FUNCTIONAL)) {  

      if (node->rvalue()->is_typed(cdk::TYPE_FUNCTIONAL)) {
        if (!(acceptable_functional_type(cdk::functional_type::cast(node->lvalue()->type()), 
                                         cdk::functional_type::cast(node->rvalue()->type())) ||
              (node->rvalue()->is_typed(cdk::TYPE_POINTER) && 
               cdk::reference_type::cast(node->rvalue()->type())->referenced() == nullptr))) {
              
          throw std::string("wrong type for initializer (function expected).");
        }

      node->type(node->rvalue()->type());
    }
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {              
      node->type(cdk::primitive_type::create(4, cdk::TYPE_ERROR));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_ERROR));
    }
    else {
      throw std::string("wrong type in assignment to function");
    }

  }
  else {
    throw std::string("wrong types in assignment");
  }
}

void mml::type_checker::do_variable_declaration_node(mml::variable_declaration_node *const node, int lvl) {
  if (node->initializer() != nullptr) {
    node->initializer()->accept(this, lvl + 2);

    if (node->type()) {
      // Ints.
      if (node->is_typed(cdk::TYPE_INT)) {
        if (!node->initializer()->is_typed(cdk::TYPE_INT)) {
          throw std::string("wrong type for initializer (integer expected).");
        }
      }
      // Doubles.
      else if (node->is_typed(cdk::TYPE_DOUBLE)) {
        // Ints can be used in place of doubles.
        if (!node->initializer()->is_typed(cdk::TYPE_INT) &&
            !node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
          throw std::string("wrong type for initializer (integer or double expected).");
        }
      }
      // Strings.
      else if (node->is_typed(cdk::TYPE_STRING)) {
        if (!node->initializer()->is_typed(cdk::TYPE_STRING)) {
          throw std::string("wrong type for initializer (string expected).");
        }
      }
      // Pointers.
      else if (node->is_typed(cdk::TYPE_POINTER)) {
        if (!(node->initializer()->is_typed(cdk::TYPE_POINTER) && 
             acceptable_pointer_type(node->type(), node->initializer()->type()))) {
          throw std::string("wrong type for initializer (pointer expected).");
        }
      }
      // Functional types.
      else if (node->is_typed(cdk::TYPE_FUNCTIONAL)) { 
        if (!((node->initializer()->is_typed(cdk::TYPE_FUNCTIONAL) && 
               acceptable_functional_type(cdk::functional_type::cast(node->type()), 
                                          cdk::functional_type::cast(node->initializer()->type()))) ||
              ((node->initializer()->is_typed(cdk::TYPE_POINTER) && 
                cdk::reference_type::cast(node->initializer()->type())->referenced() == nullptr)))) {
          throw std::string("wrong type for initializer (function expected).");
        }
      }
      else {
        throw std::string("unknown type for initializer.");
      }
    }
    else {
      node->type(node->initializer()->type());
    }
  }

  auto new_symbol = mml::make_symbol(node->type(), node->identifier(), (bool)node->initializer(), node->qualifier());
  std::shared_ptr<mml::symbol> prev_symbol = _symtab.find_local(new_symbol->name());

  // Symbol has already been declared.
  if (prev_symbol) {
    if (prev_symbol->type()->name() == cdk::TYPE_FUNCTIONAL &&
        new_symbol->type()->name() == cdk::TYPE_FUNCTIONAL && 
        acceptable_functional_type(cdk::functional_type::cast(prev_symbol->type()),
                                   cdk::functional_type::cast(new_symbol->type()))) {
      _symtab.replace(new_symbol->name(), new_symbol);
    }
    else if (prev_symbol->type()->name() == cdk::TYPE_POINTER && 
             new_symbol->type()->name() == cdk::TYPE_POINTER && 
             acceptable_pointer_type(prev_symbol->type(), new_symbol->type())) {
      _symtab.replace(new_symbol->name(), new_symbol);
    }
    else if (prev_symbol->type()->name() == new_symbol->type()->name()) {
        _symtab.replace(new_symbol->name(), new_symbol);
    }
    else {
      throw std::string(node->identifier() + " has already been declared before.");
    }
  }
  // Symbol has not been declared.
  else {
    _symtab.insert(node->identifier(), new_symbol);
  }

  _parent->set_new_symbol(new_symbol);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_evaluation_node(mml::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void mml::type_checker::do_input_node(mml::input_node *const node, int lvl) {
    node->type(cdk::primitive_type::create(0, cdk::TYPE_UNSPEC));
}

//---------------------------------------------------------------------------
//-----------------------------> INSTRUCTIONS <------------------------------
//---------------------------------------------------------------------------

void mml::type_checker::do_block_node(mml::block_node *const node, int lvl) {
  // EMPTY.
}

//---------------------------------------------------------------------------

void mml::type_checker::do_while_node(mml::while_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void mml::type_checker::do_if_node(mml::if_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void mml::type_checker::do_if_else_node(mml::if_else_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_stop_node(mml::stop_node *const node, int lvl) {
  // We can call stop with no arguments.
  if (node->level()) {
    if (!isdigit(node->level()))
      throw std::string("wrong type for stop argument (integer expected).");
  }
}

void mml::type_checker::do_next_node(mml::next_node *const node, int lvl) {
  if (node->level()) {
    if (!isdigit(node->level()))
      throw std::string("wrong type for next argument (integer expected).");
  }
}

//---------------------------------------------------------------------------

void mml::type_checker::do_return_node(mml::return_node *const node, int lvl) {
  auto main_symbol = _symtab.find("_main", 1);

  // Main function.
  if (main_symbol) {       
    if (!node->retval()) {
        throw std::string("wrong type for main's return expression (integer expected).");
    }

    node->retval()->accept(this, lvl + 2);

    if (!node->retval()->is_typed(cdk::TYPE_INT)) {
      throw std::string("wrong type for main's return expression (integer expected).");
    }
  }
  // Other functions.
  else {
    auto func_symbol = _symtab.find("@", 1);

    if (node->retval()) {
      std::shared_ptr<cdk::functional_type> return_type = cdk::functional_type::cast(func_symbol->type());
      return_type = cdk::functional_type::cast(return_type->output(0));

      auto ret_type = return_type->output();
      auto ret_type_name = return_type->output(0)->name();


      if (ret_type != nullptr && ret_type_name == cdk::TYPE_VOID) {
        throw std::string("return value specified for void function.");
      }

      node->retval()->accept(this, lvl + 2);

      if (ret_type != nullptr && ret_type_name == cdk::TYPE_INT) {
        if (!node->retval()->is_typed(cdk::TYPE_INT)) {
          throw std::string("wrong type for return expression (integer expected).");
        }
      }
      else if (ret_type != nullptr && ret_type_name == cdk::TYPE_DOUBLE) {
        // Ints can be used in place of doubles.
        if (!node->retval()->is_typed(cdk::TYPE_INT) && !node->retval()->is_typed(cdk::TYPE_DOUBLE)) {
          throw std::string("wrong type for return expression (integer or double expected).");
        }
      }
      else if (ret_type != nullptr && ret_type_name == cdk::TYPE_STRING) {
        if (!node->retval()->is_typed(cdk::TYPE_STRING)) {
          throw std::string("wrong type for return expression (string expected).");
        }
      }
      else if (ret_type != nullptr && ret_type_name == cdk::TYPE_POINTER) {
        if (node->retval()->is_typed(cdk::TYPE_POINTER)) {
          if (!(acceptable_pointer_type(return_type->output(0), node->retval()->type()))) {
            throw std::string("wrong type for return expression (pointer expected).");
          }
        }
      }
      else if (ret_type != nullptr && ret_type_name == cdk::TYPE_FUNCTIONAL) {
        node->retval()->accept(this, lvl + 2);
        
        if (node->retval()->is_typed(cdk::TYPE_FUNCTIONAL)) {
          if (!(acceptable_functional_type(cdk::functional_type::cast(return_type->output(0)), 
                                           cdk::functional_type::cast(node->retval()->type())) ||
               (node->retval()->is_typed(cdk::TYPE_POINTER) && 
                cdk::reference_type::cast(node->retval()->type())->referenced() == nullptr))) {

            throw std::string("wrong type for return expression (function expected).");
          }
        }
      }
      else {
        throw std::string("unknown type for return expression.");
      }
    }
  }  
}

//---------------------------------------------------------------------------

void mml::type_checker::do_print_node(mml::print_node *const node, int lvl) {
  node->arguments()->accept(this, lvl + 2);
}

void mml::type_checker::do_with_update_node(mml::with_update_node *const node, int lvl) {

}

//---------------------------------------------------------------------------
//-------------------------------> FUNCTIONS <-------------------------------
//---------------------------------------------------------------------------

void mml::type_checker::do_function_call_node(mml::function_call_node *const node, int lvl) {
  ASSERT_UNSPEC;
  std::vector<std::shared_ptr<cdk::basic_type>> arg_types;
  std::shared_ptr<cdk::basic_type> return_type;

  // Recursive call.
  if (!node->func()) {      
    auto symbol = _symtab.find("@", 1);

    if (symbol == nullptr) {
      throw std::string("recursive call is not inside function.");
    }

    if (symbol->main()) {
      throw std::string("recursive call is in main function.");
    }

    arg_types = cdk::functional_type::cast(symbol->type())->input()->components();
    return_type = cdk::functional_type::cast(symbol->type())->output(0);
  }
  // Normal call.
  else {
    node->func()->accept(this, lvl + 2);

    if (!(node->func()->type()->name() == cdk::TYPE_FUNCTIONAL)) {
      throw std::string("expected function pointer on function call.");
    }

    arg_types = cdk::functional_type::cast(node->func()->type())->input()->components();
    return_type = cdk::functional_type::cast(node->func()->type())->output(0);
  }

  node->type(return_type);

  if (node->arguments()->size() == arg_types.size()) {
    node->arguments()->accept(this, lvl + 4);
    
    for (size_t ax = 0; ax < node->arguments()->size(); ax++) {
      if (node->argument(ax)->type()->name() == arg_types[ax]->name()) {
        continue;
      }

      // Ints can be used in place of doubles.
      if (arg_types[ax]->name() == cdk::TYPE_DOUBLE && node->argument(ax)->is_typed(cdk::TYPE_INT)) {
        continue;
      }
      
      throw std::string("type mismatch for argument " + std::to_string(ax + 1) + ".");
    }
  }
  else {
    throw std::string(std::to_string(arg_types.size()) + " arguments expected but " +
                      std::to_string(node->arguments()->size()) + " given.");
  }
}


void mml::type_checker::do_function_definition_node(mml::function_definition_node *const node, int lvl) {  
  if (node->main()) {
    // The main function can't have recursive calls.
    auto fun = mml::make_symbol(cdk::functional_type::create(node->type()), "_main", 0, tPUBLIC);

    if (_symtab.find_local(fun->name())) {
      _symtab.replace(fun->name(), fun);
    }
    else {
      if (!_symtab.insert(fun->name(), fun)) {
        return;
      }
    }

    fun->set_main(true);

    _parent->set_new_symbol(fun);
  }
  else {
    std::vector<std::shared_ptr<cdk::basic_type>> arg_types;

    if (node->arguments()) {
      for (size_t ax = 0; ax < node->arguments()->size(); ax++) {
        arg_types.push_back(node->argument(ax)->type());
      }
    }
    
    node->type(cdk::functional_type::create(arg_types, node->return_type()));
    auto fun = mml::make_symbol(node->type(), "@", 0, tPRIVATE);

    if (_symtab.find_local(fun->name())) {
      _symtab.replace(fun->name(), fun);
    }
    else {
      if (!_symtab.insert(fun->name(), fun)) {
        return;
      }
    }

    _parent->set_new_symbol(fun);
  }
}

//---------------------------------------------------------------------------
//----------------------------> RELATED TO MEMORY <--------------------------
//---------------------------------------------------------------------------

void mml::type_checker::do_address_of_node(mml::address_of_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 2);
  node->type(cdk::reference_type::create(4, node->lvalue()->type()));
}

void mml::type_checker::do_sizeof_node(mml::sizeof_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->expression()->accept(this, lvl + 2);
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void mml::type_checker::do_stack_alloc_node(mml::stack_alloc_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in allocation expression.");
  }
 
  auto mytype = cdk::reference_type::create(4, cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  node->type(mytype);
}

void mml::type_checker::do_index_node(mml::index_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->base()->accept(this, lvl + 2);
  
  std::shared_ptr<cdk::reference_type> btype = cdk::reference_type::cast(node->base()->type());
  if (!node->base()->is_typed(cdk::TYPE_POINTER)) throw std::string("pointer expression expected in index left-value.");

  node->index()->accept(this, lvl + 2);
  if (!node->index()->is_typed(cdk::TYPE_INT)) throw std::string("integer expression expected in left-value index.");

  node->type(btype->referenced());
}

