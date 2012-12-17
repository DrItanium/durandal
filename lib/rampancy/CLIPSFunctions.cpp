#include "rampancy/CLIPSFunctions.h"
#include "rampancy/Cortex.h"
extern "C" {
   #include "clips.h"
}

extern "C" void CompileFileIntoKnowledge(void* env);
extern "C" void InterpretCodeIntoKnowledge(void* env);
extern "C" void SetupRampancyExpertSystemInterfaces(void* env) {
   EnvDefineFunction(env, "rampancy-compile", 'v',
         PTIEF CompileFileIntoKnowledge, "CompileFileIntoKnowledge");
   EnvDefineFunction(env, "rampancy-interpret", 'v',
         PTIEF InterpretCodeIntoKnowledge, "InterpretCodeIntoKnowledge");
}

extern "C" void CompileFileIntoKnowledge(void* env) {
   rampancy::Cortex* globalCortex = rampancy::Cortex::getRampantCortex();
   globalCortex->compileToKnowledge(env);
}

extern "C" void InterpretCodeIntoKnowledge(void* env) {
   rampancy::Cortex* globalCortex = rampancy::Cortex::getRampantCortex();
   globalCortex->interpretToKnowledge(env);
}
