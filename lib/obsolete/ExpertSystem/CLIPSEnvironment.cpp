#include "ExpertSystem/CLIPSEnvironment.h"

extern "C" {
#include "clips.h"
}

CLIPSEnvironment::CLIPSEnvironment(void* theEnv) {
   if(!theEnv) {
      environment = CreateEnvironment();
      fullyOwned = true;
   } else {
      environment = theEnv;
      fullyOwned = false;
   }
}
CLIPSEnvironment::~CLIPSEnvironment() {
   if(fullyOwned) {
      DestroyEnvironment(environment);
   }
}

bool CLIPSEnvironment::environmentCreated() { return (environment != 0); }
void* CLIPSEnvironment::getEnvironment() { return environment; }
void CLIPSEnvironment::reset() { 
   EnvReset(environment); 
}
void CLIPSEnvironment::clear() {
   EnvClear(environment);
}
void CLIPSEnvironment::run(ExecutionDuration count) {
   if(count <= 0L) {
      EnvRun(environment, -1L);
   } else {
      EnvRun(environment, count);
   }
}
void CLIPSEnvironment::saveInstances(char* fileName) {
   EnvSaveInstances(environment, fileName, VISIBLE_SAVE, NULL, TRUE);
}
void CLIPSEnvironment::assertFact(char* str) {
   EnvAssertString(environment, str);
}
bool CLIPSEnvironment::makeInstances(char* str) {
   return EnvLoadInstancesFromString(environment, str, -1) != -1;
}
bool CLIPSEnvironment::makeInstance(char* str) {
   return (EnvMakeInstance(environment, str) != NULL);
}
void CLIPSEnvironment::batchStar(char* str) {
   EnvBatchStar(environment, str);
}

void CLIPSEnvironment::eval(char* stmt) {
   DATA_OBJECT tmp;
   EnvEval(environment, stmt, &tmp);
}

void CLIPSEnvironment::build(char* stmt) {
   EnvBuild(environment, stmt);
}
