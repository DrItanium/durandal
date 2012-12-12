#include "ExpertSystem/CLIPSEnvironment.h"

extern "C" {
#include "clips.h"
}

CLIPSEnvironment::CLIPSEnvironment() {
   environment = CreateEnvironment();
}
CLIPSEnvironment::~CLIPSEnvironment() {
   DestroyEnvironment(environment);
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
