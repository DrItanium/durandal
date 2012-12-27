#include "clips.h"
#include "core/EnvironmentOperations.h"

#define werror (char*)"werror"
#define wdisplay (char*)"wdisplay"
#define wwarning (char*)"wwarning"
#define PrintError(e, msg) EnvPrintRouter(e, werror, (char*) msg)

extern void EnvironmentOperationsDefinitions(void* theEnv) {
	EnvDefineFunction(theEnv, "env-destroy", 'v', 
			PTIEF DestroyEnvironment, 
			"DestroyEnvironment"); 
	EnvDefineFunction(theEnv, "get-currently-executing-environment", 'a',
			PTIEF GetCurrentlyExecutingEnvironment,
			"GetCurrentlyExecutingEnvironment");
	EnvDefineFunction(theEnv, "is-currently-executing-environment", 'w',
			PTIEF IsCurrentlyExecutingEnvironment,
			"IsCurrentlyExecutingEnvironment");
	EnvDefineFunction(theEnv, "to-pointer", 'a', PTIEF ToPointer, "ToPointer");
	EnvDefineFunction(theEnv, "env-run", 'v', PTIEF RunEnvironment, "RunEnvironment");
	EnvDefineFunction(theEnv, "env-eval", 'u', PTIEF EnvironmentEval, 
			"EnvironmentEval");
	EnvDefineFunction(theEnv, "env-build", 'v', PTIEF EnvironmentBuild, 
			"EnvironmentBuild");
	EnvDefineFunction(theEnv, "env-facts", 'v', PTIEF EnvironmentFacts, 
			"EnvironmentFacts");
	EnvDefineFunction(theEnv, "env-instances", 'v', PTIEF EnvironmentInstances, 
			"EnvironmentInstances");
	EnvDefineFunction(theEnv, "env-rules", 'v', PTIEF EnvironmentRules, 
			"EnvironmentRules");
	EnvDefineFunction(theEnv, "env-assert-string", 'v', PTIEF EnvironmentAssertString, 
			"EnvironmentAssertString");
}
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
	if(EnvArgCountCheck(theEnv, "get-currently-executing-environment", EXACTLY, 0) == -1) {
		PrintError(theEnv, 
				"get-currently-executing-environment does not accept arguments\n"); 
		return;
	}
	return theEnv;
}

extern void* IsCurrentlyExecutingEnvironment(void* theEnv) {
	DATA_OBJECT arg0;
	if((EnvArgCountCheck(theEnv, "is-currently-executing-environment", EXACTLY, 1) == -1)) {
		PrintError(theEnv, "Either too many or too few arguments provided\n");
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
		PrintError(theEnv, "Target argument isn't an integer or external-address.\n");
		return FalseSymbol();
	}
}

extern void* ToPointer(void* theEnv) {
	DATA_OBJECT arg0;
	void* nil = (void*)0;
	if(EnvArgCountCheck(theEnv, "to-pointer", EXACTLY, 1) == -1) {
		PrintError(theEnv, "Too few or too many arguments provided\n");
		return nil;
	}
	if(EnvArgTypeCheck(theEnv, "to-pointer", 1, INTEGER, &arg0)) {
		return (void*)(long long)DOToLong(arg0);
	} else if(EnvArgTypeCheck(theEnv, "to-pointer", 1, EXTERNAL_ADDRESS, &arg0)) {
		return (void*)GetValue(arg0);
	} else {
		return nil;
	}
}

extern void RunEnvironment(void* theEnv) {
	DATA_OBJECT arg0, arg1;
	void* address;
	if(EnvArgCountCheck(theEnv, "env-run", EXACTLY, 2) == -1) {
		PrintError(theEnv,"Too few or too many arguments provided!\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-run", 1, EXTERNAL_ADDRESS, &arg0) == FALSE) {
		PrintError(theEnv, "Provided pointer address isn't an external address!\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-run", 2, INTEGER, &arg1) == FALSE) {
		PrintError(theEnv, "Provided duration is not an integer!\n");
		return;
	}
	long value = (long)DOToLong(arg1);
	address = (void*)GetValue(arg0);
	if(value > 0L || value == -1) {
		EnvRun(address, value);
		return;
	} else {
		PrintError(theEnv, "Provided duration must be greater than zero or -1\n");
		return;
	}
}

extern void EnvironmentEval(void* theEnv, DATA_OBJECT_PTR ret) {
	DATA_OBJECT arg0, arg1;
	void* address;
	char* result, tmp;
	if(EnvArgCountCheck(theEnv, "env-eval", EXACTLY, 2) == -1) {
		PrintError(theEnv, "env-eval accepts exactly two arguments\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-eval", 1, EXTERNAL_ADDRESS, &arg0) == FALSE) {
		PrintError(theEnv, "First argument must be an external address\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-eval", 2, STRING, &arg1) == FALSE) {
		PrintError(theEnv, "Second argument must be a string\n");
		return;
	}
	address = (void*)GetValue(arg0);
	tmp = DOToString(arg1);
	result = CharBuffer(strlen(tmp) + 1);
	sprintf(result, "%s", tmp);
	EnvEval(address, result, ret);
	free(result);
}

extern void EnvironmentBuild(void* theEnv) {
	DATA_OBJECT arg0, arg1;
	void* address;
	char* result, tmp;
	if(EnvArgCountCheck(theEnv, "env-build", EXACTLY, 2) == -1) {
		PrintError(theEnv, "env-build accepts exactly two arguments\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-build", 1, EXTERNAL_ADDRESS, &arg0) == FALSE) {
		PrintError(theEnv, "First argument must be an external address\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-build", 2, STRING, &arg1) == FALSE) {
		PrintError(theEnv, "Second argument must be a string\n");
		return;
	}
	address = (void*)GetValue(arg0);
	tmp = DOToString(arg1);
	result = CharBuffer(strlen(tmp) + 1);
	sprintf(result, "%s", tmp);
	EnvBuild(address, result);
	free(result);
}

extern void EnvironmentAssertString(void* theEnv) {
	DATA_OBJECT arg0, arg1;
	void* address;
	char* result, tmp;
	if(EnvArgCountCheck(theEnv, "env-assert-string", EXACTLY, 2) == -1) {
		PrintError(theEnv, "Only two arguments are allowed\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-assert-string", 1, EXTERNAL_ADDRESS, &arg0) == FALSE) {
		PrintError(theEnv, "First argument must be an external address\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-assert-string", 2, STRING, &arg1) == FALSE) {
		PrintError(theEnv, "Second argument must be a string\n");
		return;
	}
	address = (void*)GetValue(arg0);
	tmp = DOToString(arg1);
	result = CharBuffer(strlen(tmp) + 1);
	sprintf(result, "%s", tmp);
	EnvAssertString(address, result);
	free(result);
}

extern void EnvironmentFacts(void* theEnv) {
	DATA_OBJECT arg0;
	void* address;
	if(EnvArgCountCheck(theEnv, "env-facts", EXACTLY, 1) == -1) {
		PrintError(theEnv, "Too many or too few arguments provided to env-facts\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-facts", 1, EXTERNAL_ADDRESS, &arg0) == FALSE) {
		PrintError(theEnv, "The provided argument must be an external address\n");
		return;
	}

	address = (void*)GetValue(arg0);
	EnvFacts(address, wdisplay, NULL, -1, -1, -1);
}

extern void EnvironmentInstances(void* theEnv) {
	DATA_OBJECT arg0;
	void* address;
	if(EnvArgCountCheck(theEnv, "env-instances", EXACTLY, 1) == -1) {
		PrintError(theEnv, "Too many or too few arguments provided to env-instances\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-instances", 1, EXTERNAL_ADDRESS, &arg0) == FALSE) {
		PrintError(theEnv, "The provided argument must be an external address\n");
		return;
	}

	address = (void*)GetValue(arg0);
	EnvInstances(address, wdisplay, NULL, NULL, 1);
}

extern void EnvironmentRules(void* theEnv) {
	DATA_OBJECT arg0;
	void* address;
	if(EnvArgCountCheck(theEnv, "env-rules", EXACTLY, 1) == -1) {
		PrintError(theEnv, "Too many or too few arguments provided to env-rules\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-rules", 1, EXTERNAL_ADDRESS, &arg0) == FALSE) {
		PrintError(theEnv, "The provided argument must be an external address\n");
		return;
	}
	address = (void*)GetValue(arg0);
	EnvListDefrules(address, wdisplay, NULL);
}




#undef werror
#undef wdisplay
#undef wwarning
#undef PrintError

