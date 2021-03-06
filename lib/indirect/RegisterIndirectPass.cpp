#include "indirect/RegisterIndirectPass.h"
#include "indirect/IndirectPassRegistry.h"

/* we can't provide a default constructor because it's necessary
 * to dynamically acquire the unique id of the indirect pass.
 * Since LLVM was not designed to do this I have to work within
 * it's confines. I have no intention of introducing changes to
 * a system that is already working quite well. 
 *
 * Thus there is no default constructor for indirect passes.
 * There are ones for the templates though.
 */
namespace indirect {
   RegisterIndirectPass::RegisterIndirectPass(const char* passArg, 
         const char* passName, 
         bool cfgOnly, 
         bool isAnalysis) : llvm::PassInfo(passName, passArg, 
            indirect::IndirectPassRegistry::getUniqueId(passArg), 
            0, cfgOnly, isAnalysis) {
            llvm::PassRegistry::getPassRegistry()->registerPass(*this);
         }
   RegisterIndirectPass::RegisterIndirectPass(IndirectPassHeader* header) : 
      llvm::PassInfo(header->getPassDescription(), header->getPassName(), 
            (void*)header->getID(), 0, header->getIsCFGOnlyPass(), 
            header->getIsAnalysis()) {
         llvm::PassRegistry::getPassRegistry()->registerPass(*this);
      }
}
