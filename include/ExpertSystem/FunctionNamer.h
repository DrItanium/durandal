#ifndef _FUNCTION_NAMER_H
#define _FUNCTION_NAMER_H
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/DenseMap.h"
#include "ExpertSystem/Types.h"
#include <string>
#include <map>
extern "C" {
#include <cstdio>
#include <cstring>
}
using namespace llvm;
class FunctionNamer { 
   private:
      PointerAddress gensymID;
      PointerAddress loopID;
      PointerAddress registerID;
      PointerAddress basicBlockID;
      PointerAddress regionID;
      llvm::DenseMap<PointerAddress, std::string>* names;
      std::map<std::string, PointerAddress>* instructionIndices;
      char* fnName;

   public:
      FunctionNamer();
      ~FunctionNamer();
      void setFunctionName(char* functionName);
      char* getFunctionName(); 
      PointerAddress nextLoopID();
      PointerAddress nextRegisterID();
      PointerAddress nextBasicBlockID();
      PointerAddress nextRegionID();
      PointerAddress nextGensymID();
      PointerAddress getRegisterIDCount();
      PointerAddress getBasicBlockIDCount();
      PointerAddress getRegionIDCount();
      PointerAddress getGensymIDCount();
      PointerAddress getLoopIDCount();
      llvm::DenseMap<PointerAddress, std::string>& getTranslationTable();
      void reset();
      void makeBasicBlockID(raw_string_ostream& container);
      void makeRegisterID(raw_string_ostream& container);
      void makeRegionID(raw_string_ostream& container);
      void makeGensymID(raw_string_ostream& container);
      void makeLoopID(raw_string_ostream& container);
      void makeBasicBlockID(char* container);
      void makeRegisterID(char* container);
      void makeRegionID(char* container);
      void makeGensymID(char* container);
      void makeLoopID(char* container);
      void concatFunctionNameToFront(char* container, char* name);
      void concatFunctionNameToFront(raw_string_ostream& container, char* name);
      bool basicBlockRegistered(std::string name);
      void registerBasicBlock(std::string name);
      PointerAddress numberOfInstructionsForBasicBlock(std::string name);
      PointerAddress registerInstructionWithBasicBlock(std::string name);
      bool pointerRegistered(PointerAddress ptr);
      void registerPointerToName(PointerAddress ptr, std::string& name);
      std::string nameFromPointer(PointerAddress ptr);
      void tryRegisterPointerToName(PointerAddress ptr, std::string& name);
};
#endif
