#ifndef _clips_environment_h
#define _clips_environment_h
#include "ExpertSystem/Types.h"
class CLIPSEnvironment {
   private:
      void* environment;
      bool fullyOwned;
   public:
      CLIPSEnvironment(void* theEnv=0);
      ~CLIPSEnvironment();
      bool environmentCreated(); 
      void* getEnvironment();
      void reset();
      void clear();
      void run(ExecutionDuration count);
      void saveInstances(char* fileName);
      void assertFact(char* str);
      bool makeInstances(char* str);
      bool makeInstance(char* str);
      void batchStar(char* str);
      void eval(char* str);
      void build(char* str);
};
#endif
