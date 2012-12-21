#ifndef _indirect_unique_identifier_h
#define _indirect_unique_identifier_h
/* This is a class that defines a pointer used to provide a unique ID useful
 * for pass registration. Thanks goes to silvas on #llvm for providing me with
 * this technique instead of my over the top static SSD GC idea. 
 *
 * This class stores a single char* ptr that is used as the unique identifer
 * that the static char ID field provides in native LLVM passes.
 */
namespace indirect {
   class IndirectUniqueIdentifier {
      private:
         char* id;
      public:
         IndirectUniqueIdentifier();
         ~IndirectUniqueIdentifier();
         char* getID();
   };
}

#endif
