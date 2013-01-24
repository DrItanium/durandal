#include "knowledge/KnowledgeConstructor.h"
extern "C" {
#include "clips.h"
}

namespace knowledge {
   KnowledgeConstructor::KnowledgeConstructor(void* env, void* tObj) {
      tmp = new std::string();
      names = new llvm::DenseMap<PointerAddress, std::string>();
      instanceStream = new llvm::raw_string_ostream(*tmp);
      theEnv = env;
      targetObject = tObj;
   }
   KnowledgeConstructor::~KnowledgeConstructor() {
      delete instances;
      delete instanceStream;
      delete tmp;
   }

   std::string KnowledgeConstructor::getInstancesAsString() { 
      return instanceStream->str(); 
   }
   void KnowledgeConstructor::addToInstances(std::string &instance) {
      (*instanceStream) << instance << " ";
   }
   void KnowledgeConstructor::registerKnowledgeName(PointerAddress ptr, 
         std::string &name) {
      std::pair<PointerAddress, std::string&> pair (ptr, instance);
      names->insert(pair);
   }
   std::string KnowledgeConstructor::getObjectKnowledgeName(
         PointerAddress ptr) {
      return names->lookup(ptr);
   }

   bool KnowledgeConstructor::objectAlreadyConvertedToKnowledge(
         PointerAddress ptr) {
      return names->count(ptr);
   }
   void KnowledgeConstructor::gensym(char* buffer) {
      gensprintf(buffer,"gen%lld",MiscFunctionData(theEnv)->GensymNumber);
      MiscFunctionData(theEnv)->GensymNumber++;
      EnvAddSymbol(theEnv,buffer);
   }

}

