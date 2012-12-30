#include "pipeline/clips/CLIPSPassTemplates.h"
#include "ExpertSystem/KnowledgeConstructionEngine.h"
extern "C" {
#include "clips.h"
}
namespace pipeline {
	namespace clips {
		bool CLIPSModulePass::runOnModule(llvm::Module& module) {
			//assume the environment has already been setup in one way or another.
			void* env = getEnvironment();		
			CLIPSEnvironment* clEnv = new CLIPSEnvironment(env);
			EnvReset(env);
			CLIPSPassHeader* header = (CLIPSPassHeader*)getIndirectPassHeader();
			char* passes = CharBuffer(strlen(header->passes) + 64);
			sprintf(passes,"(passes %s)", header->passes);
			EnvAssertString(env, passes);
			free(passes);

			KnowledgeConstructor tmp;
			tmp.route(module);
			clEnv->makeInstances((char*)tmp.getInstancesAsString().c_str());
			llvm::PassManager manager;
			const llvm::PassInfo* ci = registry->getPassInfo(
						llvm::StringRef("function-to-knowledge"));
         ExpertSystem::FunctionKnowledgeConversionPass* copy = 
            (ExpertSystem::FunctionKnowledgeConversionPass*)ci->createPass();
			copy->setEnvironment(clEnv);
			copy->setSkipLoops(!header->needsLoops());
			copy->setSkipRegions(!header->needsRegions());
			manager.add(copy);
			//need to have the conversion code
			manager.run(&module);
			//TODO: put in the line to build the actual knowledge
			EnvRun(env, -1L);
			//it's up to the code in the expert system to make changes
			EnvReset(env);
			return true;
		}

		bool CLIPSFunctionPass::runOnFunction(llvm::Function& function) {
			void* env = getEnvironment();
			CLIPSEnvironment* clEnv = new CLIPSEnvironment(env);
			EnvReset(env);
			CLIPSPassHeader* header = (CLIPSPassHeader*)getIndirectPassHeader();
			char* passes = CharBuffer(strlen(header->passes) + 64);
			sprintf(passes,"(passes %s)", header->passes);
			EnvAssertString(env, passes);
			free(passes);
			KnowledgeConstructor tmp;
			if(header->needsLoops() && header->needsRegions()) {
				llvm::LoopInfo& li = getAnalysis<LoopInfo>();
				llvm::RegionInfo& ri = getAnalysis<RegionInfo>();
				tmp.route(function, li, ri);
			} else if(header->needsLoops() && !header->needsRegions()) {
				llvm::LoopInfo& li = getAnalysis<LoopInfo>();
				tmp.route(function, li);
			} else if(header->needsRegions() && !header->needsLoops()) {
				llvm::RegionInfo& ri = getAnalysis<RegionInfo>();
				tmp.route(function, ri);
			} else {
				tmp.route(function);
			}
			clEnv->makeInstances((char*)tmp.getInstancesAsString().c_str());
			//TODO: put in the line to build the actual knowledge
			EnvRun(env, -1L);
			//it's up to the code in the expert system to make changes
			EnvReset(env);
			return true;
		}
		bool CLIPSBasicBlockPass::runOnBasicBlock(llvm::BasicBlock& bb) {
			void* env = getEnvironment();
			CLIPSEnvironment* clEnv = new CLIPSEnvironment(env);
			EnvReset(env);
			CLIPSPassHeader* header = (CLIPSPassHeader*)getIndirectPassHeader();
			char* passes = CharBuffer(strlen(header->passes) + 64);
			sprintf(passes,"(passes %s)", header->passes);
			EnvAssertString(env, passes);
			free(passes);
			KnowledgeConstructor tmp;
			FunctionNamer namer;
			tmp.route(&bb, namer, (char*)"");
			clEnv->makeInstances((char*)tmp.getInstancesAsString().c_str());
			EnvRun(env, -1L);
			//it's up to the code in the expert system to make changes
			EnvReset(env);
			return true;
		}
		bool CLIPSRegionPass::runOnRegion(llvm::Region* region, llvm::RGPassManager& rg) {
			void* env = getEnvironment();
			CLIPSEnvironment* clEnv = new CLIPSEnvironment(env);
			EnvReset(env);
			CLIPSPassHeader* header = (CLIPSPassHeader*)getIndirectPassHeader();
			char* passes = CharBuffer(strlen(header->passes) + 64);
			sprintf(passes,"(passes %s)", header->passes);
			EnvAssertString(env, passes);
			free(passes);
			KnowledgeConstructor tmp;
			FunctionNamer namer;
			tmp.route(region, namer, (char*)"");
			clEnv->makeInstances((char*)tmp.getInstancesAsString().c_str());
			//TODO: put in code to allow us to manipulate the RGPassManager
			EnvRun(env, -1L);
			//it's up to the code in the expert system to make changes
			EnvReset(env);
			return true;
		}
		bool CLIPSLoopPass::runOnLoop(llvm::Loop* loop, llvm::LPPassManager& rg) {
			void* env = getEnvironment();
			CLIPSEnvironment* clEnv = new CLIPSEnvironment(env);
			EnvReset(env);
			CLIPSPassHeader* header = (CLIPSPassHeader*)getIndirectPassHeader();
			char* passes = CharBuffer(strlen(header->passes) + 64);
			sprintf(passes,"(passes %s)", header->passes);
			EnvAssertString(env, passes);
			free(passes);
			KnowledgeConstructor tmp;
			FunctionNamer namer;
			tmp.route(loop, namer, (char*)"");
			clEnv->makeInstances((char*)tmp.getInstancesAsString().c_str());
			//TODO: put in code to allow us to manipulate the LPPassManager
			EnvRun(env, -1L);
			//it's up to the code in the expert system to make changes
			EnvReset(env);
			return true;
		}
		bool CLIPSMachineFunctionPass::runOnMachineFunction(llvm::MachineFunction& mf) {
			llvm::errs() << "Machine function passes are not valid clips pass types\n";
			return false;
		}
		bool CLIPSCallGraphSCCPass::runOnSCC(llvm::CallGraphSCC& mf) {
			llvm::errs() << "CallGraphSCC passes are currently not supported\n";
			return false;
		}
	}
}
