#include <string>
#include <vector>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "targets/frame_size_calculator.h"
#include "targets/symbol.h"
#include ".auto/all_nodes.h"  // all_nodes.h is automatically generated

#include <mml_parser.tab.h>


//---------------------------------------------------------------------------

void mml::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void mml::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------
//-------------------> LITERALS / ARITHMETIC EXPRESSIONS <-------------------
//---------------------------------------------------------------------------

void mml::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  if (_in_fun_body) {
    _pf.INT(node->value()); 
  }
  else {
    _pf.SINT(node->value()); 
  }
}

void mml::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  std::string lbl = mklbl(++_lbl);

  _pf.RODATA(); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are aligned
  _pf.LABEL(lbl); // give the string a name
  _pf.SSTRING(node->value()); // output string characters
  
  if (_in_fun_body) {
    _pf.TEXT(_return_labels.back()); // TODO: check this.
    _pf.ADDR(lbl);
  } else {
    _pf.DATA();
    _pf.SADDR(lbl);
  }
}

// New implementations:
void mml::postfix_writer::do_nullptr_node(mml::nullptr_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_in_fun_body) {
    _pf.INT(0);
  }
  else {
    _pf.SINT(0);
  }
}

void mml::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  if (_in_fun_body) {
    _pf.DOUBLE(node->value()); // load number to the stack
  }
  else {
    _pf.SDOUBLE(node->value());    // double is on the DATA segment
  }


}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.NEG(); // 2-complement
}

// New implementations:
void mml::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl + 2);
  _pf.INT(0);
  _pf.EQ();
  }

void mml::postfix_writer::do_identity_node(mml::identity_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D(); // Converts from int to double.
  }
  else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.INT(cdk::reference_type::cast(node->right()->type())->referenced()->size());
    _pf.MUL();
  }

  node->right()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }
  else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.INT(cdk::reference_type::cast(node->left()->type())->referenced()->size());
    _pf.MUL();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DADD();
  } else {
    _pf.ADD();
  }
}

void mml::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D(); // Converts from int to double.
  }
  else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.INT(cdk::reference_type::cast(node->right()->type())->referenced()->size());
    _pf.MUL();
  }

  node->right()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }
  else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.INT(cdk::reference_type::cast(node->left()->type())->referenced()->size());
    _pf.MUL();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DSUB();
  }
  // Distance between two pointers.
  else if (node->is_typed(cdk::TYPE_POINTER) &&
           node->right()->is_typed(cdk::TYPE_POINTER) &&
           node->left()->is_typed(cdk::TYPE_POINTER)) {
    _pf.SUB();
  }
  else {
    _pf.SUB();
  }
}

void mml::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);

  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  node->right()->accept(this, lvl + 2);

  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DMUL();
  } else {
    _pf.MUL();
  }
}

void mml::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);

  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  node->right()->accept(this, lvl + 2);

  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DDIV();
  } else {
    _pf.DIV();
  }
}

void mml::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.I2D();
  }
  node->right()->accept(this, lvl + 2);

  if (node->right()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.I2D();
  }

  _pf.MOD();
}

void mml::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.I2D();
  }
  node->right()->accept(this, lvl + 2);

  if (node->right()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.I2D();
  }

  _pf.LT();
}

void mml::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.I2D();
  }
  node->right()->accept(this, lvl + 2);

  if (node->right()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.I2D();
  }
  
  _pf.LE();
}

void mml::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.I2D();
  }
  node->right()->accept(this, lvl + 2);

  if (node->right()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.I2D();
  }

  _pf.GE();
}

void mml::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.I2D();
  }
  node->right()->accept(this, lvl + 2);

  if (node->right()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.I2D();
  }

  _pf.GT();
}

void mml::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.I2D();
  }
  node->right()->accept(this, lvl + 2);

  if (node->right()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.I2D();
  }

  _pf.NE();
}

void mml::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.I2D();
  }
  node->right()->accept(this, lvl + 2);

  if (node->right()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.I2D();
  }

  _pf.EQ();
}

// New implementations:
void mml::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::string lbl = mklbl(++_lbl);

  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JZ(lbl);

  node->right()->accept(this, lvl + 2);
  _pf.AND();
  _pf.ALIGN();
  _pf.LABEL(lbl);
}

void mml::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::string lbl = mklbl(++_lbl);

  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JNZ(lbl);
  
  node->right()->accept(this, lvl + 2);
  _pf.OR();
  _pf.ALIGN();
  _pf.LABEL(lbl);
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  auto symbol = _symtab.find(node->name());

  if (symbol->qualifier() == tFOREIGN ||
      symbol->qualifier() == tFORWARD) {
    set_foreign_label(node->name());
  }
  else if(symbol->is_global()) {
    _pf.ADDR(symbol->name());
  }
  else {
    _pf.LOCAL(symbol->offset());
  }  
}

void mml::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.LDDOUBLE();
  }
  else {
    if (_foreign_label.empty()) {
      _pf.LDINT();
    }
  }
}

void mml::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->rvalue()->accept(this, lvl + 2);

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    if (node->rvalue()->is_typed(cdk::TYPE_INT)) {
      _pf.I2D();
    }
    _pf.DUP64();
  }
  else {
    _pf.DUP32();
  }

  node->lvalue()->accept(this, lvl + 2);

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.STDOUBLE();
  }
  else {
    _pf.STINT();
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_variable_declaration_node(mml::variable_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int offset = 0;
  int typesize = node->type()->size();

    
  if (_in_fun_body) {
    _offset -= typesize;
    offset = _offset;
  }
  else if (_in_fun_args) {
    offset = _offset;
    _offset += typesize;
  }
  else {
    offset = 0;
  }
  
  auto symbol = new_symbol();

  if (symbol) {
    symbol->set_offset(offset);
    reset_new_symbol();
  }

  if (!_in_fun_args && !_in_fun_body) {
    _symbols_to_declare.insert(symbol->name());
  }

  if (node->initializer()) {

    if (_in_fun_body) {
      node->initializer()->accept(this, lvl);

      if (symbol->is_typed(cdk::TYPE_INT) || symbol->is_typed(cdk::TYPE_STRING) || 
          symbol->is_typed(cdk::TYPE_POINTER) || symbol->is_typed(cdk::TYPE_FUNCTIONAL)) {
        _pf.LOCAL(symbol->offset());
        _pf.STINT();
      }
      else if (symbol->is_typed(cdk::TYPE_DOUBLE)) {
        if (node->is_typed(cdk::TYPE_INT)) {
          _pf.I2D();
        }

        _pf.LOCAL(symbol->offset());
        _pf.STDOUBLE();
      }
      else {
        std::cerr << "unknown variable type" << std::endl;
        return;
      }
    }
    else {
      if (symbol->is_typed(cdk::TYPE_INT) || symbol->is_typed(cdk::TYPE_DOUBLE) ||
          symbol->is_typed(cdk::TYPE_POINTER)) {

        _pf.DATA();
        _pf.ALIGN();
        _pf.LABEL(symbol->name());

        if (symbol->is_typed(cdk::TYPE_INT) || symbol->is_typed(cdk::TYPE_POINTER)) {
          node->initializer()->accept(this, lvl);
        }
        else if (symbol->is_typed(cdk::TYPE_DOUBLE)) {
          if (node->is_typed(cdk::TYPE_DOUBLE)) {
            node->initializer()->accept(this, lvl);
          }
          else if (node->is_typed(cdk::TYPE_INT)) {
            cdk::integer_node *dclini = dynamic_cast<cdk::integer_node*>(node->initializer());
            cdk::double_node ddi(dclini->lineno(), dclini->value());
            ddi.accept(this, lvl);
          }
          else {
            std::cerr << node->lineno() << ": (" << node->identifier() <<
                  ") has bad initializer for real value" << std::endl;
          }
        }
      }
      else if (symbol->is_typed(cdk::TYPE_STRING)) {
          _pf.DATA();
          _pf.ALIGN();
          _pf.LABEL(symbol->name());
          node->initializer()->accept(this, lvl);
      }
      else if (symbol->is_typed(cdk::TYPE_FUNCTIONAL)) {
        _fun_symbols.push_back(symbol);
        reset_new_symbol();

        node->initializer()->accept(this, lvl);

        _pf.DATA();

        if (_fun_symbols.back()->qualifier() == tPUBLIC) {
          _pf.GLOBAL(_fun_symbols.back()->name(), _pf.OBJ());
        }
        _pf.ALIGN();
        _pf.LABEL(symbol->name());

        _pf.SADDR(_fun_label);
        _fun_label.clear();
      }
      else {
        std::cerr << node->lineno() << ": (" << node->identifier() <<
              ") has unexpected initializer in declaration" << std::endl;
      }
    }

    _symbols_to_declare.erase(symbol->name());
  } 
}


//---------------------------------------------------------------------------

void mml::postfix_writer::do_evaluation_node(mml::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  _pf.TRASH(node->argument()->type()->size());
}

void mml::postfix_writer::do_input_node(mml::input_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->is_typed(cdk::TYPE_INT)) {
    _functions_to_declare.insert("readi");
    _pf.CALL("readi");
    _pf.LDFVAL32();
  }
  else if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _functions_to_declare.insert("readd");
    _pf.CALL("readd");
    _pf.LDFVAL64();
  }
  else {
    std::cerr << "FATAL: " << node->lineno() << ": cannot read input type" << std::endl;
  }
}


//---------------------------------------------------------------------------
//-----------------------------> INSTRUCTIONS <------------------------------
//---------------------------------------------------------------------------

void mml::postfix_writer::do_block_node(mml::block_node *const node, int lvl) {
  _symtab.push();
  if (node->declarations()) {
    node->declarations()->accept(this, lvl + 2);
  }
  if (node->instructions()) {
    node->instructions()->accept(this, lvl + 2);
  }
  _symtab.pop();
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_while_node(mml::while_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  _while_cond.push(++_lbl);
  _while_end.push(++_lbl);

  _symtab.push(); // new context

  _pf.ALIGN();
  _pf.LABEL(mklbl(_while_cond.top()));

  node->condition()->accept(this, lvl + 2);

  _pf.JZ(mklbl(_while_end.top()));

  node->block()->accept(this, lvl + 2);

  _pf.JMP(mklbl(_while_cond.top()));
  _pf.ALIGN();
  _pf.LABEL(mklbl(_while_end.top()));

  _symtab.pop();
  
  _while_end.pop();
  _while_cond.pop();
}

void mml::postfix_writer::do_if_node(mml::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::string lbl = mklbl(++_lbl);

  node->condition()->accept(this, lvl);
  _pf.JZ(lbl);

  node->block()->accept(this, lvl + 2);
  _pf.LABEL(lbl);
}

void mml::postfix_writer::do_if_else_node(mml::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::string lbl1 = mklbl(++_lbl);
  std::string lbl2 = mklbl(++_lbl);

  node->condition()->accept(this, lvl);
  _pf.JZ(lbl1);

  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(lbl2);
  _pf.LABEL(lbl1);

  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(lbl2);
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_stop_node(mml::stop_node *const node, int lvl) {
  // If stop was called without a value, then it should stop the innermost loop.
  size_t val = (node->level() ? node->level() : 1);

  if (_while_end.size() < val) {
    std::cerr << node->lineno() << ": stop value out of range" << std::endl;
    return;
  }
  else {
    int jmp_lbl;
    std::stack<int> end_temp;
    std::stack<int> cond_temp;

    for (size_t i = 1; i < val; i++) {
      end_temp.push(_while_end.top());
      cond_temp.push(_while_cond.top());

      _while_cond.pop();
      _while_end.pop();
    }

    jmp_lbl = _while_end.top();

    for (size_t i = 1; i < val; i++) {
      _while_cond.push(cond_temp.top());
      _while_end.push(end_temp.top());

      cond_temp.pop();
      end_temp.pop();
    }

    _pf.JMP(mklbl(jmp_lbl));
  }
}

void mml::postfix_writer::do_next_node(mml::next_node *const node, int lvl) {
  // If next was called without a value, then it should restart the innermost loop.
  size_t val = (node->level() ? node->level() : 1);

  if (_while_cond.size() < val) {
    std::cerr << node->lineno() << ": next value out of range" << std::endl;
    return;
  }
  else {
    int jmp_lbl;
    std::stack<int> end_temp;
    std::stack<int> cond_temp;

    for (size_t i = 1; i < val; i++) {
      end_temp.push(_while_end.top());
      cond_temp.push(_while_cond.top());

      _while_cond.pop();
      _while_end.pop();
    }

    jmp_lbl = _while_cond.top();

    for (size_t i = 1; i < val; i++) {
      _while_cond.push(cond_temp.top());
      _while_end.push(end_temp.top());

      cond_temp.pop();
      end_temp.pop();
    }

    _pf.JMP(mklbl(jmp_lbl));
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_return_node(mml::return_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  set_return_seen(true);

  auto curr_fun = _fun_symbols.back();

  std::shared_ptr<cdk::basic_type> return_type = cdk::functional_type::cast(curr_fun->type())->output(0);

  // If the current function isn't main.
  if (return_type->name() != cdk::TYPE_INT) {
    return_type = cdk::functional_type::cast(return_type)->output(0);
  }

  if (return_type->name() != cdk::TYPE_VOID) {
    node->retval()->accept(this, lvl + 2);

    if (return_type->name() == cdk::TYPE_INT ||
        return_type->name() == cdk::TYPE_STRING ||
        return_type->name() == cdk::TYPE_POINTER || 
        return_type->name() == cdk::TYPE_FUNCTIONAL) {
      _pf.STFVAL32();
    }
    else if (return_type->name() == cdk::TYPE_DOUBLE) {
      if (node->retval()->type()->name() == cdk::TYPE_INT) {
        _pf.I2D();
      }
      _pf.STFVAL64();
    }
    else {
      std::cerr << node->lineno() << ": unknown return type" << std::endl;
    }
  }

  _pf.LEAVE();
  _pf.RET();

  set_return_seen(false);
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_print_node(mml::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  for (size_t ix = 0; ix < node->arguments()->size(); ix++) {
    auto arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ix));
    arg->accept(this, lvl); // expression to print

    if (arg->is_typed(cdk::TYPE_INT)) {
      _functions_to_declare.insert("printi");
      _pf.CALL("printi");
      _pf.TRASH(4); // trash int
    }
    else if (arg->is_typed(cdk::TYPE_DOUBLE)) {
      _functions_to_declare.insert("printd");
      _pf.CALL("printd");
      _pf.TRASH(8); // trash double
    }
    else if (arg->is_typed(cdk::TYPE_STRING)) {
      _functions_to_declare.insert("prints");
      _pf.CALL("prints");
      _pf.TRASH(4); // trash char pointer
    }
    else {
      std::cerr << node->lineno() << ": cannot print expression of unknown type" << std::endl;
      return;
    }
  }

  if (node->newline()) {
    _functions_to_declare.insert("println");
    _pf.CALL("println");
  }
}

//---------------------------------------------------------------------------
//-------------------------------> FUNCTIONS <-------------------------------
//---------------------------------------------------------------------------

void mml::postfix_writer::do_function_call_node(mml::function_call_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  std::vector<std::shared_ptr<cdk::basic_type>> arg_types;

  // Recursive call
  if (!node->func()) {
    auto curr_fun = _fun_symbols.back();
    arg_types = cdk::functional_type::cast(curr_fun->type())->input()->components();
  }
  // Normal call
  else {
    arg_types = cdk::functional_type::cast(node->func()->type())->input()->components();
  }

  size_t args_size = 0;
  if (node->arguments() && node->arguments()->size() > 0) {

    for (int ax = node->arguments()->size() - 1; ax >= 0; ax--) {
      auto arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ax));

      arg->accept(this, lvl + 2);
      
      if (arg_types.at(ax)->name() == cdk::TYPE_DOUBLE && arg->is_typed(cdk::TYPE_INT)) {
        _pf.I2D();
        args_size += 4;
      }

      args_size += arg->type()->size();
    }
  }

  // Recursive call
  if (!node->func()) {
    _pf.CALL(_return_labels.back());
  }
  // Normal call
  else {                    
    _foreign_label.clear();

    node->func()->accept(this, lvl + 2);

    if (!_foreign_label.empty()) {
      _pf.CALL(_foreign_label);
    }
    else {
      _pf.BRANCH();
    }
  }

  if (args_size != 0) {
    _pf.TRASH(args_size);
  }
  
  if (node->is_typed(cdk::TYPE_POINTER) ||
      node->is_typed(cdk::TYPE_STRING) ||
      node->is_typed(cdk::TYPE_FUNCTIONAL)) {
    _pf.LDFVAL32();
  }
  else if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.LDFVAL64();
  }
  else if (node->is_typed(cdk::TYPE_INT)) {
      _pf.LDFVAL32();
  }

  _foreign_label.clear();
}

void mml::postfix_writer::do_function_definition_node(mml::function_definition_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->main()) {

    for (std::string name : _symbols_to_declare) {
      auto symbol = _symtab.find(name, 0);

      if (symbol->qualifier() == tFOREIGN ||
          symbol->qualifier() == tFORWARD) {
        _functions_to_declare.insert(name);
      }
      else {
        _pf.BSS();
        _pf.ALIGN();
        _pf.LABEL(name);
        _pf.SALLOC(symbol->type()->size());    
      }
    }
    
    auto symbol = new_symbol();
    if (symbol) {
      _fun_symbols.push_back(symbol);
      reset_new_symbol();
    }

    _return_labels.push_back("_main");
    
    _symtab.push();
    _pf.TEXT(_return_labels.back());
    _pf.ALIGN();
    _pf.GLOBAL("_main", _pf.FUNC());
    _pf.LABEL("_main");

    frame_size_calculator lsc(_compiler, _symtab, symbol);

    _symtab.push();
    node->accept(&lsc, lvl);
    _symtab.pop();
  
    _pf.ENTER(lsc.localsize());

    bool temp = _return_seen;

    _return_seen = false;
    _in_fun_body = true;
    if (node->block()) {
      node->block()->accept(this, lvl + 2);
    }
    _in_fun_body = false;

    _symtab.pop();
    
    _return_labels.pop_back();
    if (!_return_seen) {
      _pf.INT(0);
      _pf.STFVAL32();
    }
    
    _pf.LEAVE();
    _pf.RET();

    _fun_symbols.pop_back();
    for (std::string ext : _functions_to_declare) {
      _pf.EXTERN(ext);
    }
    _functions_to_declare.clear();
    _return_seen = temp;

  }
  else {

    auto symbol = new_symbol();
    if (symbol) {
      _fun_symbols.push_back(symbol);
      reset_new_symbol();
    }

    set_offset(8); // prepare for arguments (4: remember to account for return address)
    _symtab.push(); // scope of args

    if (node->arguments()) {
      set_in_fun_args(true);

      for (size_t ix = 0; ix < node-> arguments()->size(); ix++){
        cdk::basic_node *arg = node->arguments()->node(ix);

        if (!arg) {
          break; // this means an empty sequence of arguments
        }

        arg->accept(this, 0); // the function symbol is at the top of the stack
      }

      set_in_fun_args(false);
    }

    std::string lbl = mklbl(++_lbl);

    _return_labels.push_back(lbl);
    _pf.TEXT(_return_labels.back());
    _pf.ALIGN();

    if (symbol->is_global()) {
      //_pf.GLOBAL(symbol->name(), _pf.FUNC());
    }

    _pf.LABEL(lbl);
    frame_size_calculator lsc(_compiler, _symtab, symbol);

    _symtab.push(); 
    node->accept(&lsc, lvl);
    _symtab.pop();
    
    _pf.ENTER(lsc.localsize());

    set_offset(0); // Prepare for local variables

    bool temp = _in_fun_body;

    set_in_fun_body(true);
    if (node->block()) {
      node->block()->accept(this, lvl + 4);
    }
    set_in_fun_body(temp);

    _symtab.pop();
  
    if (!_return_seen) {
      _pf.LEAVE();
      _pf.RET();
    }

    _return_labels.pop_back();
    if (symbol) {
      _fun_symbols.pop_back();
    }

    // Since a function definition is an expression, the last line must be its value (i.e., the address of its code)
    if (_in_fun_body) {
      _pf.TEXT(_return_labels.back());
      _pf.ADDR(lbl);
    } 

    _fun_label = lbl;
  }
  
}

//---------------------------------------------------------------------------
//----------------------------> RELATED TO MEMORY <--------------------------
//---------------------------------------------------------------------------

void mml::postfix_writer::do_address_of_node(mml::address_of_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl + 2);
}

void mml::postfix_writer::do_stack_alloc_node(mml::stack_alloc_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  auto size = cdk::reference_type::cast(node->type())->referenced()->size();
  
  node->argument()->accept(this, lvl);
  _pf.INT(size);
  _pf.MUL();
  _pf.ALLOC(); // allocate
  _pf.SP(); // put base pointer in stack
}

void mml::postfix_writer::do_sizeof_node(mml::sizeof_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (_in_fun_body) {
    _pf.INT(node->expression()->type()->size());
  }
  else {
    _pf.SINT(node->expression()->type()->size());
  }
}

void mml::postfix_writer::do_index_node(mml::index_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->base()->accept(this, lvl);
  node->index()->accept(this, lvl);
  _pf.INT(node->type()->size());
  _pf.MUL();
  _pf.ADD(); // add pointer and index
}
