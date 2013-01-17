#include "ExpertSystem/KnowledgeTypeNameTrait.h"
namespace knowledge {
   template<typename T>
      const char* GetTypeName() {
         if(HasKnowledgeTypeNameTrait<T>::hasName) {
            return KnowledgeTypeNameTrait<T>::getTypeName();
         } else {
            assert(false && "ERROR: Tried to get the type name of an unregistered type");
            return KnowledgeTypeNameTrait<T>::getTypeName();
         }
      }
}
