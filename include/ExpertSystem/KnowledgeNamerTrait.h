#ifndef _knowledge_namer_trait_h
#define _knowledge_namer_trait_h
#include "ExpertSystem/KnowledgeConstructor.h"
namespace knowledge {
   template<typename T>
      struct HasCustomNamingLogicTrait {
         static const bool value = false;
      };
      template<bool b>
      struct CustomNamingLogicTrait {
         template<typename T>
         static std::string getUniqueName(T* obj, KnowledgeConstructor* kc) {
            if(obj->hasName()) {
               std::string tmp(obj->getName().data());
               return tmp;
            } else {
               char* buf = CharBuffer(128);
               kc->gensym(buf);
               std::string name(buf);
               free(buf);
               return name;
            }
         }
      };
      template<typename T> 
      std::string GetUniqueName(T* obj, KnowledgeConstructor* kc);
}

#endif
