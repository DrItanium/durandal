#ifndef _clips_user_builder_h
#define _clips_user_builder_h
#include "llvm/User.h"
#include "ExpertSystem/CLIPSValueBuilder.h"

using namespace llvm;
class CLIPSUserBuilder : public CLIPSValueBuilder {
   public:
      CLIPSUserBuilder(std::string nm, std::string ty, FunctionNamer& namer);
      void addFields(User* user, KnowledgeConstructor* kc, char* parent);
		void build(User* user, KnowledgeConstructor* kc, char* parent);
};
#endif
