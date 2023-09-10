#ifndef __MML_FACTORY_H__
#define __MML_FACTORY_H__

#include <memory>
#include <cdk/yy_factory.h>
#include "mml_scanner.h"

namespace mml {

  /**
   * This class implements the compiler factory for the MML compiler.
   */
  class factory: public cdk::yy_factory<mml_scanner> {
    /**
     * This object is automatically registered by the constructor in the
     * superclass' language registry.
     */
    static factory _self;

  protected:
    /**
     * @param language name of the language handled by this factory (see .cpp file)
     */
    factory(const std::string &language = "mml") :
        cdk::yy_factory<mml_scanner>(language) {
    }

  };

} // mml

#endif
