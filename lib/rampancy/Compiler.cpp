#include "rampancy/Compiler.h"
#include "llvm/Analysis/RegionInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "ExpertSystem/KnowledgeConstructor.h"
#include "ExpertSystem/Types.h"
extern "C" {
#include "clips.h"
}

namespace rampancy {
   Compiler::Compiler(char ID) : llvm::ModulePass(ID) { }
   Compiler::~Compiler() { }

   void Compiler::getAnalysisUsage(llvm::AnalysisUsage& info) {
      info.addRequired<llvm::RegionInfo>();
      info.addRequired<llvm::LoopInfo>();
   }

   bool Compiler::runOnModule(llvm::Module& module) {
      //a cheap hack to do what we need to do
      CLIPSEnvironment* theEnv = getEnvironment();
      beforeKnowledgeConstruction(&module);
      KnowledgeConstructor tmp;
      tmp.route(&module);
      for(Module::iterator i = module.begin(), e = module.end(); 
            i != e; ++i) {
           llvm::RegionInfo &ri = getAnalysis<llvm::RegionInfo>(*i);
           llvm::LoopInfo &li = getAnalysis<llvm::LoopInfo>(*i);
           tmp.route(*i, li, ri);
      }
      theEnv->makeInstances((char*)tmp.getInstancesAsString().c_str());
      afterKnowledgeConstruction(&module);
      return false;
   }

   CLIPSEnvironment* Compiler::getEnvironment() {
      return env;
   }
   void Compiler::setEnvironment(CLIPSEnvironment* theEnv) {
      env = theEnv; 
   }
   llvm::LLVMContext* Compiler::getContext() {
      return context;
   }
   void Compiler::setContext(llvm::LLVMContext* llvmContext) {
      context = llvmContext; 
   }
   void Compiler::beforeKnowledgeConstruction(llvm::Module* module) {
      //we need to setup this module within CLIPS
      CLIPSEnvironment* theEnv = getEnvironment();
      DATA_OBJECT rtn;
      char* gensym;
      char* cmd = CharBuffer(512);
      char* cmd2 = CharBuffer(512); 
      EnvFunctionCall(theEnv->getEnvironment(), "gensym*", NULL, &rtn);
      gensym = DOToString(rtn);
      sprintf(cmd, "(defmodule module-%s (import core ?ALL) (import llvm ?ALL) (export ?ALL))", gensym);
      sprintf(cmd2, "(set-current-module module-%s)", gensym);
      theEnv->eval(cmd);
      theEnv->eval(cmd2);
      free(cmd);
      free(cmd2);
   }
   void Compiler::afterKnowledgeConstruction(llvm::Module* module) {
      //switch back to the MAIN environment
      CLIPSEnvironment* theEnv = getEnvironment();
      theEnv->eval("(printout t \"Created a new module named \" (set-current-module MAIN) crlf)");
   }

}
