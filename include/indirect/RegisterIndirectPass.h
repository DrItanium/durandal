#ifndef _register_indirect_pass_h
#define _register_indirect_pass_h
/* IndirectRegisterPass is a class used to register indirect passes in a fashion
 * similar to how llvm::RegisterPass works.
 */
#include "llvm/Pass.h" 
#include "indirect/IndirectPassRegistry.h"

namespace indirect {
   class RegisterIndirectPass : public llvm::PassInfo {
      public:
         RegisterIndirectPass(const char* passArg, const char* passName, 
               bool cfgOnly = false, bool isAnalysis = false) : PassInfo(passName,
                  passArg, indirect::IndirectPassRegistry::getUniqueId(passArg), 
                  /* we can't provide a default constructor because it's necessary
                   * to dynamically acquire the unique id of the indirect pass.
                   * Since LLVM was not designed to do this I have to work within
                   * it's confines. I have no intention of introducing changes to
                   * a system that is already working quite well. 
                   *
                   * Thus there is no default constructor for indirect passes.
                   * There are ones for the templates though.
                   */
                  0, cfgOnly, isAnalysis) {
                  llvm::PassRegistry::getPassRegistry()->registerPass(*this);
               }
   };
}

#endif
