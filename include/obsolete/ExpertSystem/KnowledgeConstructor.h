#ifndef _knowledge_construction_pass_h
#define _knowledge_construction_pass_h
#include "llvm/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "llvm/Instruction.h"
#include "llvm/Instructions.h"
#include "llvm/IntrinsicInst.h"
#include "llvm/Argument.h"
#include "llvm/Analysis/RegionInfo.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/User.h"
#include "llvm/Value.h"
#include "llvm/Type.h"
#include "llvm/BasicBlock.h"
#include "llvm/Analysis/RegionIterator.h"
#include "llvm/Operator.h"
#include "llvm/Metadata.h"
#include "llvm/InlineAsm.h"
#include "llvm/Argument.h"
#include "llvm/Constant.h"
#include "llvm/Constants.h"
#include "llvm/GlobalVariable.h"
#include "llvm/GlobalValue.h"
#include "llvm/GlobalAlias.h"
#include "llvm/CodeGen/PseudoSourceValue.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Operator.h"
#include "llvm/Function.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Value.h"
#include "ExpertSystem/Types.h"
#include "ExpertSystem/FunctionNamer.h"
#include "ExpertSystem/CLIPSEnvironment.h"
#include <string>

using namespace llvm;
class KnowledgeConstructor {
   llvm::DenseMap<PointerAddress, std::string>* instances;
   llvm::raw_string_ostream* instanceStream; 
   std::string* tmp;
   PointerAddress moduleCount;
   public:
   KnowledgeConstructor();
   ~KnowledgeConstructor();
   void resetInstanceStream();

   llvm::DenseMap<PointerAddress, std::string>* getInstances();
   std::string getInstancesAsString();
   PointerAddress getModuleCount();
   void addToInstanceStream(std::string &instance);
   void registerInstance(PointerAddress ptrAddress, std::string &instance);
   void addToKnowledgeBase(PointerAddress ptrAddress, std::string &instance);
   std::string route(Value* val, FunctionNamer& namer, char* parent);
   std::string route(Value* val, FunctionNamer& namer);
   std::string route(User* user, FunctionNamer& namer, char* parent);
   std::string route(Constant* cnst, FunctionNamer& namer);
   std::string route(Constant* cnst, FunctionNamer& namer, char* parent);
   std::string route(Instruction* inst, FunctionNamer& namer, char* parent);
   std::string route(Type* t, FunctionNamer& namer);
   std::string route(Operator* op, FunctionNamer& namer); 
   std::string route(Operator* op, FunctionNamer& namer, char* parent);
   std::string route(BasicBlock* bb, FunctionNamer& namer, char* parent, bool constructInstructions = true);
   std::string route(Region* region, FunctionNamer& namer, char* parent);
   std::string route(Argument* arg, FunctionNamer& namer, char* parent);
   std::string route(Loop* loop, FunctionNamer& namer, char* parent);
   std::string route(MDString* mds, FunctionNamer& namer, char* parent);
   std::string route(MDNode* mdn, FunctionNamer& namer, char* parent);
   std::string route(InlineAsm* iasm, FunctionNamer& namer, char* parent);
   void route(RegionInfo& ri, FunctionNamer& namer, char* parent);
   void route(LoopInfo& li, FunctionNamer& namer, char* parent);
   void updateFunctionContents(Function& fn, FunctionNamer& namer);
   void route(Function& fn, LoopInfo& li, RegionInfo& ri);
   void route(Function& fn, RegionInfo& ri);
   void route(Function& fn, LoopInfo& ri);
   void route(Function& fn);
   std::string route(Module* module);
};
#endif
