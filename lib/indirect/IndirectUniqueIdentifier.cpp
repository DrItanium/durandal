#include "indirect/IndirectUniqueIdentifier.h"
#include <cstdlib>

namespace indirect {
   IndirectUniqueIdentifier::IndirectUniqueIdentifier() {
      id = (char*)calloc(1, sizeof(char));
   }
   IndirectUniqueIdentifier::~IndirectUniqueIdentifier() {
      free(id);
   }
   char* IndirectUniqueIdentifier::getID() {
      return id;
   }
}
