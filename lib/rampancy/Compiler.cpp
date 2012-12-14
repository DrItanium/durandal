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
      info.addRequired<RegionInfo>();
      info.addRequired<LoopInfo>();
   }

   bool Compiler::runOnModule(llvm::Module* module) {
      //a cheap hack to do what we need to do
      CLIPSEnvironment* env = getManagingEnvironment();
      beforeKnowledgeConstruction(module, env);
      bool result = runOnModule(module, env);
      afterKnowledgeConstruction(module, env);
      return result;

   }

   bool Compiler::runOnModule(llvm::Module* module, CLIPSEnvironment* theEnv) {
      if(theEnv) {
         KnowledgeConstructor tmp;
         tmp.route(module);
         for(Module::iterator i = module->begin(), e = module->end(); i != e; ++i) {
            RegionInfo &ri = getAnalysis<RegionInfo>(*i);
            LoopInfo &li = getAnalysis<LoopInfo>(*i);
            tmp.route(*i, li, ri);
         }
         theEnv->makeInstances((char*)tmp.getInstancesAsString().c_str());
      } else {
         llvm::errs() << "ERROR: no environment specified\n"; 
      }
      return false;
   }

   CLIPSEnvironment* Compiler::getManagingEnvironment() {
      //for now return nil
      return (CLIPSEnvironment*)0;
   }
   void Compiler::beforeKnowledgeConstruction(llvm::Module* module, CLIPSEnvironment* theEnv) {
      //we need to setup this module within CLIPS
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
   void Compiler::afterKnowledgeConstruction(llvm::Module* module, CLIPSEnvironment* theEnv) {
      //switch back to the MAIN environment
      theEnv->eval("(printout t \"Created a new module named \" (set-current-module MAIN) crlf)");
   }

}
