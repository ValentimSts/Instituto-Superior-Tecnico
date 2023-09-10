#ifndef __MML_TARGETS_SYMBOL_H__
#define __MML_TARGETS_SYMBOL_H__

#include <string>
#include <vector>
#include <memory>
#include <cdk/types/basic_type.h>

namespace mml {

  class symbol {
    std::shared_ptr<cdk::basic_type> _type; // type (type id + type size)
    std::string _name;
    long _value; // hack!
    int _qualifier; // qualifiers: public, forward, "private" (i.e., none)
    bool _main;

    int _offset = 0;

  public:
    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, long value, int qualifier) :
        _type(type), _name(name), _value(value), _qualifier(qualifier), _main(false) {
    }

    virtual ~symbol() {
      // EMPTY
    }

    std::shared_ptr<cdk::basic_type> type() const {
      return _type;
    }
    void set_type(std::shared_ptr<cdk::basic_type> t) {
      _type = t;
    }
    bool is_typed(cdk::typename_type name) const {
      return _type->name() == name;
    }
    const std::string &name() const {
      return _name;
    }
    
    long value() const {
      return _value;
    }
    long value(long v) {
      return _value = v;
    }

    int qualifier() {
      return _qualifier;
    }
    int qualifier(int q) {
      return _qualifier = q;
    }

    bool main() {
      return _main;
    }
    bool set_main(bool m) {
      return _main = m;
    }

    int offset() {
      return _offset;
    }
    int set_offset(int o) {
      return _offset = o;
    }

    bool is_global() const {
      return _offset == 0;
    }
  };

  inline auto make_symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, long value, int qualifier) {
    return std::make_shared<symbol>(type, name, value, qualifier);
  }

} // mml

#endif
