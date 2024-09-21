#ifndef __MML_TARGETS_POSTFIX_WRITER_H__
#define __MML_TARGETS_POSTFIX_WRITER_H__

#include "targets/basic_ast_visitor.h"

#include <sstream>
#include <set>
#include <stack>
#include <cdk/emitters/basic_postfix_emitter.h>

namespace mml {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<mml::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;

    bool _in_fun_args;
    bool _in_fun_body;
    bool _return_seen;
    std::stack<int> _while_cond;
    std::stack<int> _while_end;

    std::string _fun_label;
    std::vector<std::shared_ptr<mml::symbol>> _fun_symbols;
    std::vector<std::string> _return_labels;

    std::string _foreign_label;
    std::set<std::string> _functions_to_declare;

    std::set<std::string> _symbols_to_declare;

    int _offset = 0; // current framepointer offset (0 means no vars defined)

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<mml::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0),
        _in_fun_args(false), _in_fun_body(false), _return_seen(false) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }


    bool fun_args() {
      return _in_fun_args;
    }
    void set_in_fun_args(bool b) {
      _in_fun_args = b;
    }


    bool fun_body() {
      return _in_fun_body;
    }
    void set_in_fun_body(bool b) {
      _in_fun_body = b;
    }


    bool return_seen() {
      return _return_seen;
    }
    void set_return_seen(bool b) {
      _return_seen = b;
    }


    std::string fun_label() {
      return _fun_label;
    }
    void set_fun_label(std::string l) {
      _fun_label = l;
    }


    std::string foreign_label() {
      return _foreign_label;
    }
    void set_foreign_label(std::string l) {
      _foreign_label = l;
    }


    int offset() {
      return _offset;
    }
    void set_offset(int o) {
      _offset = o;
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // mml

#endif
