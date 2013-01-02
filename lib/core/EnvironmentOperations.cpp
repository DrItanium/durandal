#include "core/EnvironmentOperations.h"
extern "C" {
#include "clips.h"
#include <stdio.h>
}

#define werror (char*)"werror"
#define wdisplay (char*)"wdisplay"
#define wwarning (char*)"wwarning"
#define PrintError(e, msg) EnvPrintRouter(e, werror, (char*) msg)
#define CharBuffer(sz) (char*)calloc(sz, sizeof(char))

extern "C" void EnvironmentDestroy(void* theEnv);
extern "C" void* GetCurrentlyExecutingEnvironment(void* theEnv);
extern "C" void* IsCurrentlyExecutingEnvironment(void* theEnv);
extern "C" void* ToPointer(void* theEnv);
extern "C" void RunEnvironment(void* theEnv);
extern "C" void EnvironmentEval(void* theEnv, DATA_OBJECT_PTR ret);
extern "C" void EnvironmentBuild(void* theEnv);
extern "C" void EnvironmentFacts(void* theEnv);
extern "C" void EnvironmentInstances(void* theEnv);
extern "C" void EnvironmentRules(void* theEnv);
extern "C" void EnvironmentAssertString(void* theEnv);
extern "C" void* EnvironmentCreate(void* theEnv);

void EnvironmentOperationsDefinitions(void* theEnv) {
	EnvDefineFunction(theEnv, "env-destroy", 'v', 
			PTIEF EnvironmentDestroy, 
			"EnvironmentDestroy"); 
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
	EnvDefineFunction(theEnv, "env-create", 'a', PTIEF EnvironmentCreate, 
			"EnvironmentCreate");
}
void EnvironmentDestroy(void* theEnv) {
	DATA_OBJECT arg0;
	if((EnvArgCountCheck(theEnv, "env-destroy", EXACTLY, 1) == -1)) {
		return;
	}
	if(EnvArgTypeCheck(theEnv, "env-destroy", 1, EXTERNAL_ADDRESS, &arg0)) {
		void* targetEnv = (void*)GetValue(arg0);
		DestroyEnvironment(targetEnv);
		return;
	} else if(EnvArgTypeCheck(theEnv, "env-destroy", 1, INTEGER, &arg0)) {
		void* targetEnv = (void*)(long long)DOToLong(arg0);
		DestroyEnvironment(targetEnv);
		return;
	} else {
		return;
	}
}

void* GetCurrentlyExecutingEnvironment(void* theEnv) {
	if(EnvArgCountCheck(theEnv, "get-currently-executing-environment", EXACTLY, 0) == -1) {
		PrintError(theEnv, 
				"get-currently-executing-environment does not accept arguments\n"); 
		return (void*)0;
	}
	return GetCurrentEnvironment();
}

void* IsCurrentlyExecutingEnvironment(void* theEnv) {
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
	if(EnvArgTypeCheck(theEnv, "to-pointer", 1, INTEGER, &arg0) != -1) {
		return (void*)DOToLong(arg0);
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

	if(EnvArgTypeCheck(theEnv, "env-run", 1, EXTERNAL_ADDRESS, &arg0) == -1) {
		PrintError(theEnv, "Provided pointer address isn't an external address!\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-run", 2, INTEGER, &arg1) == -1) {
		PrintError(theEnv, "Provided duration is not an integer!\n");
		return;
	}
	long value = (long)DOToLong(arg1);
	address = (void*)GetValue(arg0);
	if(value > 0L || value == -1) {
		SetCurrentEnvironment(address);
		EnvRun(address, value);
		SetCurrentEnvironment(theEnv);
		return;
	} else {
		PrintError(theEnv, "Provided duration must be greater than zero or -1\n");
		return;
	}
}

void EnvironmentEval(void* theEnv, DATA_OBJECT_PTR ret) {
	DATA_OBJECT arg0, arg1;
	void* address;
	char* result;
	char* tmp;
	if(EnvArgCountCheck(theEnv, "env-eval", EXACTLY, 2) == -1) {
		PrintError(theEnv, "env-eval accepts exactly two arguments\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-eval", 1, EXTERNAL_ADDRESS, &arg0) == -1) {
		PrintError(theEnv, "First argument must be an external address\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-eval", 2, STRING, &arg1) == -1) {
		PrintError(theEnv, "Second argument must be a string\n");
		return;
	}
	address = (void*)GetValue(arg0);
	tmp = DOToString(arg1);
	result = CharBuffer(strlen(tmp) + 1);
	sprintf(result, "%s", tmp);
	SetCurrentEnvironment(address);
	EnvEval(address, result, ret);
	SetCurrentEnvironment(theEnv);
	free(result);
}

void EnvironmentBuild(void* theEnv) {
	DATA_OBJECT arg0, arg1;
	void* address;
	char* result;
	char* tmp;
	if(EnvArgCountCheck(theEnv, "env-build", EXACTLY, 2) == -1) {
		PrintError(theEnv, "env-build accepts exactly two arguments\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-build", 1, EXTERNAL_ADDRESS, &arg0) == -1) {
		PrintError(theEnv, "First argument must be an external address\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-build", 2, STRING, &arg1) == -1) {
		PrintError(theEnv, "Second argument must be a string\n");
		return;
	}
	address = (void*)GetValue(arg0);
	tmp = DOToString(arg1);
	result = CharBuffer(strlen(tmp) + 1);
	sprintf(result, "%s", tmp);
	SetCurrentEnvironment(address);
	EnvBuild(address, result);
	SetCurrentEnvironment(theEnv);
	free(result);
}

void EnvironmentAssertString(void* theEnv) {
	DATA_OBJECT arg0, arg1;
	void* address;
	char* result;
	char* tmp;
	if(EnvArgCountCheck(theEnv, "env-assert-string", EXACTLY, 2) == -1) {
		PrintError(theEnv, "Only two arguments are allowed\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-assert-string", 1, EXTERNAL_ADDRESS, &arg0) == -1) {
		PrintError(theEnv, "First argument must be an external address\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-assert-string", 2, STRING, &arg1) == -1) {
		PrintError(theEnv, "Second argument must be a string\n");
		return;
	}
	address = (void*)GetValue(arg0);
	tmp = DOToString(arg1);
	result = CharBuffer(strlen(tmp) + 1);
	sprintf(result, "%s", tmp);
	SetCurrentEnvironment(address);
	EnvAssertString(address, result);
	SetCurrentEnvironment(theEnv);
	free(result);
}

void EnvironmentFacts(void* theEnv) {
	DATA_OBJECT arg0;
	void* address;
	if(EnvArgCountCheck(theEnv, "env-facts", EXACTLY, 1) == -1) {
		PrintError(theEnv, "Too many or too few arguments provided to env-facts\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-facts", 1, EXTERNAL_ADDRESS, &arg0) == -1) {
		PrintError(theEnv, "The provided argument must be an external address\n");
		return;
	}

	address = (void*)GetValue(arg0);
	SetCurrentEnvironment(address);
	EnvFacts(address, wdisplay, NULL, -1, -1, -1);
	SetCurrentEnvironment(theEnv);
}

void EnvironmentInstances(void* theEnv) {
	DATA_OBJECT arg0;
	void* address;
	if(EnvArgCountCheck(theEnv, "env-instances", EXACTLY, 1) == -1) {
		PrintError(theEnv, "Too many or too few arguments provided to env-instances\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-instances", 1, EXTERNAL_ADDRESS, &arg0) == -1) {
		PrintError(theEnv, "The provided argument must be an external address\n");
		return;
	}

	address = (void*)GetValue(arg0);
	SetCurrentEnvironment(address);
	EnvInstances(address, wdisplay, NULL, NULL, 1);
	SetCurrentEnvironment(theEnv);
}

void EnvironmentRules(void* theEnv) {
	DATA_OBJECT arg0;
	void* address;
	if(EnvArgCountCheck(theEnv, "env-rules", EXACTLY, 1) == -1) {
		PrintError(theEnv, "Too many or too few arguments provided to env-rules\n");
		return;
	}

	if(EnvArgTypeCheck(theEnv, "env-rules", 1, EXTERNAL_ADDRESS, &arg0) == -1) {
		PrintError(theEnv, "The provided argument must be an external address\n");
		return;
	}
	address = (void*)GetValue(arg0);
	SetCurrentEnvironment(address);
	EnvListDefrules(address, wdisplay, NULL);
	SetCurrentEnvironment(theEnv);
}

void* EnvironmentCreate(void* theEnv) {
	/*
	 * This is really unsafe to call on it's own unless you REALLY know what
	 * you're doing
	 */
 	void* address;
	if(EnvArgCountCheck(theEnv, "env-create", EXACTLY, 0) == -1) {
		PrintError(theEnv, "ERROR: env-create takes in no arguments\n");
		return (void*)0;
	}
	address = CreateEnvironment();
	SetCurrentEnvironment(theEnv);
	return address;
}


#undef werror
#undef wdisplay
#undef wwarning
#undef PrintError
#undef CharBuffer

