#include "clips.h"
#include "core/EnvironmentOperations.h"

extern void DestroyEnvironment(void* theEnv) {
	DATA_OBJECT arg0;
	if((EnvArgCountCheck(theEnv, "env-destroy", EXACTLY, 1) == -1)) {
		return;
	}
	if(EnvArgTypeCheck(theEnv, "env-destroy", 1, EXTERNAL_ADDRESS, &arg0)) {
		void* targetEnv = (void*)GetValue(arg0);
		EnvDestroyEnvironment(targetEnv);
		return;
	} else if(EnvArgTypeCheck(theEnv, "env-destroy", 1, INTEGER, &arg0)) {
		void* targetEnv = (void*)(long long)DOToLong(arg0);
		EnvDestroyEnvironment(targetEnv);
		return;
	} else {
		return;
	}
}

extern void* GetCurrentlyExecutingEnvironment(void* theEnv) {
	return theEnv;
}

extern void* IsCurrentlyExecutingEnvironment(void* theEnv) {
	DATA_OBJECT arg0;
	if((EnvArgCountCheck(theEnv, "is-currently-executing-environment", EXACTLY, 1) == -1)) {
		EnvPrintRouter(theEnv, 
				(char*)"werror",
				(char*)"Either too many or too few arguments provided\n");
		return FalseSymbol(); 
	}

	if(EnvArgTypeCheck(theEnv, "is-currently-executing-environment", 1, EXTERNAL_ADDRESS, &arg0)) {
		void* otherEnv = (void*)GetValue(arg0);
		if(otherEnv == theEnv) {
			return TrueSymbol();
		} else {
			return FalseSymbol();
		}
	} else if (EnvArgTypeCheck(theEnv, "is-currently-executing-environment", 1, INTEGER, &arg0)) {
		void* otherEnv = (void*)(long long)DOToLong(arg0);	
		if(otherEnv == theEnv) {
			return TrueSymbol();
		} else {
			return FalseSymbol();
		}
	} else {
		EnvPrintRouter(theEnv, 
				(char*)"werror",
				(char*)"Target argument isn't an integer or external-address.\n");
		return FalseSymbol();
	}
}
