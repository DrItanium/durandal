#ifndef _clips_pipeline_functions_h
#define _clips_pipeline_functions_h
extern "C" void RegisterCLIPSPipelineFunctions(void* theEnv);
namespace pipeline {
	namespace clips {
		void initializeCLIPSIndirector();
	}
}
#endif
