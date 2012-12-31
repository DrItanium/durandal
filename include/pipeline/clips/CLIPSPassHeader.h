#ifndef _clips_pass_header_h
#define _clips_pass_header_h

#include "indirect/IndirectPassHeader.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallVector.h"

using namespace indirect;
namespace pipeline {
   namespace clips {
      /* An extension of the IndirectPassHeader that adds some extra logic
       * specific to dynamic CLIPS passes.
       */
      class CLIPSPassHeader : public IndirectPassHeader {
         private:
				char* passes;
            bool needRegions;
            bool needLoops;
				bool deletePassSet;
         public:
            CLIPSPassHeader();
				~CLIPSPassHeader();
            bool needsRegions();
            void setNeedsRegions(bool v);
            bool needsLoops();
            void setNeedsLoops(bool v);
				char* getPasses();
				void setPasses(char* passes, bool copy = false);
      };
   }
}

#endif
