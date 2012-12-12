#include "rampancy/RampancyCLIPS.h"
extern "C" {
   #include "clips.h"
}

extern "C" void CompileFileIntoKnowledge(void* env);
extern "C" void SaveBitcodeVersionOfModule(void* env);
extern "C" void RunFunctionAsMain(void* env);
extern "C" void SetupRampancyExpertSystemInterfaces(void* env) {
   EnvDefineFunction(env, "rampancy-compile-file-into-knowledge", 'v',
         PTIEF CompileFileIntoKnowledge, "CompileFileIntoKnowledge");
   EnvDefineFunction(env, "rampancy-save-module-to-bitcode", 'v',
         PTIEF SaveBitcodeVersionOfModule, "SaveBitcodeVersionOfModule");
   EnvDefineFunction(env, "rampancy-run-function-as-main", 'v',
         PTIEF RunFunctionAsMain, "RunFunctionAsMain");
}

extern "C" void CompileFileIntoKnowledge(void* env) {

}

extern "C" void SaveBitcodeVersionOfModule(void* env) {

}

extern "C" void RunFunctionAsMain(void* env) {

}
