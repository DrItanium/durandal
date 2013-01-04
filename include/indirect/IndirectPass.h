#ifndef _indirect_pass_h
#define _indirect_pass_h
#include "indirect/IndirectPassHeader.h"
#include "llvm/Pass.h"

/* This class is meant to interface with the different template pass classes to
 * provide the indirection functionality without having to reinvent the wheel
 * for each pass. 
 */

namespace indirect {
   class IndirectPass {
      private:
         IndirectPassHeader* header;
      public:
         IndirectPass();
         ~IndirectPass();
         void setIndirectPassHeader(IndirectPassHeader* header);
         IndirectPassHeader* getIndirectPassHeader();
         void applyIndirectAnalysisUsage(llvm::AnalysisUsage& usage) const;
   };
}
#endif
