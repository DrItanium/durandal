#include "pipeline/clips/CLIPSPass.h"

namespace pipeline {
	namespace clips {
		CLIPSPass::CLIPSPass() {

		}
		CLIPSPass::~CLIPSPass() {

		}

		void CLIPSPass::setEnvironment(void* theEnv) {
			envPtr = theEnv;
		}

		void* CLIPSPass::getEnvironment() {
			return envPtr;
		}
	}
}
