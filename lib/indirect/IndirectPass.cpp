#include "indirect/IndirectPass.h"

namespace indirect {
   IndirectPass::IndirectPass() { }
   IndirectPass::~IndirectPass() { }

   void IndirectPass::setIndirectPassHeader(IndirectPassHeader* hdr) {
      header = hdr;
   }
   IndirectPassHeader* IndirectPass::getIndirectPassHeader() {
      return header;
   }
   void IndirectPass::applyIndirectAnalysisUsage(llvm::AnalysisUsage& usage) const {
      (*header)(usage);
   }
}
