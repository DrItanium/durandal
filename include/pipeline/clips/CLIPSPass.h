#ifndef _clips_pass_h
#define _clips_pass_h

namespace pipeline {
   namespace clips {
      class CLIPSPass {
         private:
            void* envPtr;
         public:
            CLIPSPass();
            ~CLIPSPass();
            void setEnvironment(void* theEnv);
            void* getEnvironment();
      };
   }
}

#endif
