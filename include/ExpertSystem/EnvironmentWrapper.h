#ifndef _clips_environment_h
#define _clips_environment_h
class CLIPSEnvironment {
   private:
      void* environment;
   public:
      CLIPSEnvironment();
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
};
#endif
