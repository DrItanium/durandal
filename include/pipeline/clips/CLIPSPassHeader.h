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
         public:
            typedef llvm::SmallVector<char*, 64> LoadPathVector;
            typedef llvm::SmallVector<char*, 32> PassNameVector;
         private:
            LoadPathVector loadPaths;
            PassNameVector passNames;
            char* onLoadString;
         public:
            CLIPSPassHeader();
            char* getOnLoadString();
            void setOnLoadString(char* value);
            void addLoadPath(char* path);
            void addLoadPath(const char* path);
            void addLoadPath(llvm::StringRef path);
            void addPassName(char* path);
            void addPassName(const char* path);
            void addPassName(llvm::StringRef path);
            void applyToEnvironment(void* theEnv);
      };
   }
}

#endif
