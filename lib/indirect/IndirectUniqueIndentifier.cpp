#include "indirect/IndirectUniqueIdentifier.h"

namespace indirect {
   IndirectUniqueIdentifier::IndirectUniqueIdentifier() {
      id = (char*)calloc(1, sizeof(char));
   }
   IndirectUniqueIdentifier::~IndirectUniqueIdentifier() {
      free(id);
   }
   char* getID() {
      return id;
   }
}
