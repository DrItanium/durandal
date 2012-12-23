#ifndef _clips_pass_generator_h
#define _clips_pass_generator_h
#include "indirect/IndirectPassGenerator.h"
#include "pipeline/clips/CLIPSPassTemplates.h"

using namespace indirect;
namespace pipeline {
   namespace clips {
      /*
       * Use this class to generate indirect passes representing a clips pass
       * or a set of clips passes. 
       */
      class CLIPSPassGenerator : public IndirectPassGeneratorTemplate<
                                 CLIPSModulePass,
                                 CLIPSFunctionPass,
                                 CLIPSBasicBlockPass,
                                 CLIPSLoopPass,
                                 CLIPSRegionPass,
                                 CLIPSMachineFunctionPass,
                                 CLIPSCallGraphSCCPass> {

                                 };
                                 
   }
}
#endif
