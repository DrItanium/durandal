#ifndef _register_indirect_pass_h
#define _register_indirect_pass_h
/* IndirectRegisterPass is a class used to register indirect passes in a fashion
 * similar to how llvm::RegisterPass works.
 */
#include "llvm/Pass.h" 
#include "indirect/IndirectPassRegistry.h"
#include "indirect/IndirectPassHeader.h"

namespace indirect {
   class RegisterIndirectPass : public llvm::PassInfo {
      public:
         RegisterIndirectPass(IndirectPassHeader* header);
         RegisterIndirectPass(const char* passArg, const char* passName, 
               bool cfgOnly = false, bool isAnalysis = false);
                  
   };
}

#endif
