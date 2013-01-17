#include "ExpertSystem/KnowledgeNamerTrait.h"

namespace knowledge {
   template<typename T>
      std::string GetUniqueName(T* obj, KnowledgeConstructor* kc) {
         return CustomNamingLogicTrait<
            HasCustomNamingLogicTrait<T>::value>::getUniqueName(obj, kc);
      }
}
