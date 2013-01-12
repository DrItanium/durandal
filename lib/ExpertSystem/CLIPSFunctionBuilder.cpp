#include "ExpertSystem/CLIPSFunctionBuilder.h"
extern "C" {
#include "clips.h"
}

CLIPSFunctionBuilder::CLIPSFunctionBuilder(std::string nm, 
      FunctionNamer& namer) : CLIPSGlobalValueBuilder(nm, "Function",  namer) { 
}

void CLIPSFunctionBuilder::build(Function* fn, KnowledgeConstructor* kc) {
   open();
   addFields(fn, kc);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)fn, str);
}

void CLIPSFunctionBuilder::setCallingConvention(CallingConv::ID id) {
   const char* selection;
   switch(id) {
      case CallingConv::C:
         selection = "C";
         break;
      case CallingConv::Fast:
         selection = "Fast";
         break;
      case CallingConv::Cold:
         selection = "Cold";
         break;
      case CallingConv::GHC:
         selection = "GHC";
         break;
         /*
      case CallingConv::FirstTargetCC:
         selection = "FirstTargetCC";
         break;
         */
      case CallingConv::X86_StdCall:
         selection = "x86-stdcall";
         break;
      case CallingConv::X86_FastCall:
         selection = "x86-fastcall";
         break;
      case CallingConv::ARM_APCS:
         selection = "arm-apcs";
         break;
      case CallingConv::ARM_AAPCS:
         selection = "arm-aapcs";
         break;
      case CallingConv::ARM_AAPCS_VFP:
         selection = "arm-aapcs-vfp";
         break;
      case CallingConv::MSP430_INTR:
         selection = "msp430-intr";
         break;
      case CallingConv::X86_ThisCall:
         selection = "x86-thiscall";
         break;
      case CallingConv::PTX_Kernel:
         selection = "ptx-kernel";
         break;
      case CallingConv::PTX_Device:
         selection = "ptx-device";
         break;
      case CallingConv::MBLAZE_INTR:
         selection = "mblaze-intr";
         break;
      case CallingConv::MBLAZE_SVOL:
         selection = "mblaze-svol";
         break;
         /*
          * Most likely in LLVM 3.2 
          */
         /*
      case CallingConv::HiPE:
         selection = "HiPE";
         break;
      case CallingConv::SPIR_FUNC:
         selection = "spir-func";
         break;
      case CallingConv::SPIR_KERNEL:
         selection = "spir-kernel";
         break;
      case CallingConv::Intel_OCL_BI:
         selection = "intel-ocl-bi";
         break;
         */
      default:
         selection = "Unknown";
         break;
   }
   addField("CallingConvention", selection);
}
/*
void CLIPSFunctionBuilder::setAttributes(const AttributeSet& attr) {
   
}
*/
void CLIPSFunctionBuilder::addFields(Function* func, 
      KnowledgeConstructor* kc) {

   CLIPSGlobalValueBuilder::addFields(func, kc, (char*)"");
   //this part contains the code for building the function itself
   FunctionNamer& namer = getNamer();
   addField("ReturnType", kc->route(func->getReturnType(), namer));
   addField("FunctionType", kc->route(func->getFunctionType(), namer));
   if(func->isVarArg()) {
      addTrueField("IsVarArg");
   }
   addField("IntrinsicID", func->getIntrinsicID());
   if(func->isIntrinsic()) {
      addTrueField("IsIntrinsic");
   }
   setCallingConvention(func->getCallingConv());
  // setAttributes(func->getAttributes());
   if(func->hasGC()) {
      addTrueField("HasGC");
      addField("GC", func->getGC());
   }
   if(func->doesNotAccessMemory()) {
      addTrueField("DoesNotAccessMemory");
   }
   if(func->onlyReadsMemory()) {
      addTrueField("OnlyReadsMemory");
   }
   if(func->doesNotReturn()) {
      addTrueField("DoesNotReturn");
   }
   /*
   if(func->cannotDuplicate()) {
      addTrueField("CannotDuplicate");
   }
   */
   if(func->hasUWTable()) {
      addTrueField("HasUWTable");
   }
   if(func->needsUnwindTableEntry()) {
      addTrueField("NeedsUnwindTableEntry");
   }
   if(func->hasStructRetAttr()) {
      addTrueField("HasStructRetAttr");
   }
   if(func->isDefTriviallyDead()) {
      addTrueField("IsDefTriviallyDead");
   }
   if(func->callsFunctionThatReturnsTwice()) {
      addTrueField("CallsFunctionThatReturnsTwice");
   }
   char* fnName = (char*)func->getName().data();
   BasicBlock& bb = func->getEntryBlock();
   
   addField("EntryBlock", kc->route(&bb, namer, fnName));
   openField("Arguments"); {
      for(Function::arg_iterator s = func->arg_begin(), f = func->arg_end(); 
            s != f; ++s) {
         Argument* a = s;
         //build it if we need to :)
         appendValue(kc->route(a, namer, fnName));
      }
   } closeField();
   openField("contents"); {
      for(llvm::Function::iterator i = func->begin() , e = func->end(); 
            i != e; ++i) {
         BasicBlock* bb = i; 
         appendValue(kc->route(bb, namer, fnName));
      }
   } closeField();
}
/* Begin C functions for allowing CLIPS to interact with LLVM */
#define msg(x) (char*) x
#define werror msg("werror")
#define name_LLVMFunctionSetGC \
   msg("llvm-function-set-gc")
extern "C" void LLVMFunctionSetGC(void* theEnv) {
   DATA_OBJECT ptrDO,
               indexDO;
   char* index; 
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionSetGC, 
            EXACTLY, 2) == -1) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionSetGC, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionSetGC, 2, 
            SYMBOL_OR_STRING, &indexDO) == FALSE) {
      return;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return;
   } else {
      index = DOToString(indexDO);
      fn->setGC(index);
      return;
   }
}
#define name_LLVMFunctionClearGC \
   msg("llvm-function-clear-gc")
extern "C" void LLVMFunctionClearGC(void* theEnv) {
   DATA_OBJECT ptrDO;
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionClearGC, 
            EXACTLY, 1) == -1) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionClearGC, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return;
   } else {
      fn->clearGC();
      return;
   }
}
#define name_LLVMFunctionDeleteBody \
   msg("llvm-function-delete-body")
extern "C" void LLVMFunctionDeleteBody(void* theEnv) {
   DATA_OBJECT ptrDO;
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionDeleteBody, 
            EXACTLY, 1) == -1) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionDeleteBody, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return;
   } else {
      fn->deleteBody();
      return;
   }
}
#define name_LLVMFunctionRemoveFromParent \
   msg("llvm-function-remove-from-parent")
void RemoveFromParentWrapper(Function* fn) {
   /* 
    * This method is a non-extern "C" function that calls a 
    * virtual method in the llvm::Function class.
    */
   fn->removeFromParent();
}
extern "C" void LLVMFunctionRemoveFromParent(void* theEnv) {
   DATA_OBJECT ptrDO;
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionRemoveFromParent, 
            EXACTLY, 1) == -1) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionRemoveFromParent, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return;
   } else {
      RemoveFromParentWrapper(fn);
      return;
   }
}
#define name_LLVMFunctionEraseFromParent \
   msg("llvm-function-erase-from-parent")
   /* 
    * This method is a non-extern "C" function that calls a 
    * virtual method in the llvm::Function class.
    */
void EraseFromParentWrapper(Function* fn) {
   fn->eraseFromParent();
}
extern "C" void LLVMFunctionEraseFromParent(void* theEnv) {
   DATA_OBJECT ptrDO;
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionEraseFromParent, 
            EXACTLY, 1) == -1) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionEraseFromParent, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return;
   } else {
      EraseFromParentWrapper(fn);
      return;
   }
}
#define name_LLVMFunctionSetDoesNotAccessMemory \
   msg("llvm-function-set-does-not-access-memory")
extern "C" void LLVMFunctionSetDoesNotAccessMemory(void* theEnv) {
   DATA_OBJECT ptrDO;
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionSetDoesNotAccessMemory, 
            EXACTLY, 1) == -1) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionSetDoesNotAccessMemory, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return;
   } else {
      fn->setDoesNotAccessMemory();
      return;
   }
}
#define name_LLVMFunctionSetDoesNotAlias \
   msg("llvm-function-set-does-not-alias")
extern "C" void LLVMFunctionSetDoesNotAlias(void* theEnv) {
   DATA_OBJECT ptrDO,
               indexDO;
   unsigned index; 
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionSetDoesNotAlias, 
            EXACTLY, 2) == -1) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionSetDoesNotAlias, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionSetDoesNotAlias, 2, 
            INTEGER, &indexDO) == FALSE) {
      return;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return;
   } else {
      index = (unsigned)DOToInteger(indexDO);
      fn->setDoesNotAlias(index);
      return;
   }
}
#define name_LLVMFunctionSetDoesNotCapture \
   msg("llvm-function-set-does-not-capture")
extern "C" void LLVMFunctionSetDoesNotCapture(void* theEnv) {
   DATA_OBJECT ptrDO,
               indexDO;
   unsigned index; 
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionSetDoesNotCapture, 
            EXACTLY, 2) == -1) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionSetDoesNotCapture, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionSetDoesNotCapture, 2, 
            INTEGER, &indexDO) == FALSE) {
      return;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return;
   } else {
      index = (unsigned)DOToInteger(indexDO);
      fn->setDoesNotCapture(index);
      return;
   }
}
#define name_LLVMFunctionSetDoesNotThrow \
   msg("llvm-function-set-does-not-throw")
extern "C" void LLVMFunctionSetDoesNotThrow(void* theEnv) {
   DATA_OBJECT ptrDO;
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionSetDoesNotThrow, 
            EXACTLY, 1) == -1) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionSetDoesNotThrow, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return;
   } else {
      fn->setDoesNotThrow();
      return;
   }
}
#define name_LLVMFunctionSetHasUWTable \
   msg("llvm-function-set-has-uw-table")
extern "C" void LLVMFunctionSetHasUWTable(void* theEnv) {
   DATA_OBJECT ptrDO;
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionSetHasUWTable, 
            EXACTLY, 1) == -1) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionSetHasUWTable, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return;
   } else {
      fn->setHasUWTable();
      return;
   }
}
#define name_LLVMFunctionSetDoesNotReturn \
   msg("llvm-function-set-does-not-return")
extern "C" void LLVMFunctionSetDoesNotReturn(void* theEnv) {
   DATA_OBJECT ptrDO;
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionSetDoesNotReturn, 
            EXACTLY, 1) == -1) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionSetDoesNotReturn, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return;
   } else {
      fn->setDoesNotReturn();
      return;
   }
}
/*
 * Not Defined in LLVM 3.1
 */
/*
#define name_LLVMFunctionSetCannotDuplicate \
   msg("llvm-function-set-cannot-duplicate")
extern "C" void LLVMFunctionSetCannotDuplicate(void* theEnv) {
   DATA_OBJECT ptrDO;
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionSetCannotDuplicate, 
            EXACTLY, 1) == -1) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionSetCannotDuplicate, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return;
   } else {
      fn->setCannotDuplicate();
      return;
   }
}
*/
#define name_LLVMFunctionSetOnlyReadsMemory \
   msg("llvm-function-set-only-reads-memory")
extern "C" void LLVMFunctionSetOnlyReadsMemory(void* theEnv) {
   DATA_OBJECT ptrDO;
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionSetOnlyReadsMemory, 
            EXACTLY, 1) == -1) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionSetOnlyReadsMemory, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return;
   } else {
      fn->setOnlyReadsMemory();
      return;
   }
}
#define name_LLVMFunctionDropAllReferences \
   msg("llvm-function-drop-all-references")
extern "C" void LLVMFunctionDropAllReferences(void* theEnv) {
   DATA_OBJECT ptrDO;
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionDropAllReferences, 
            EXACTLY, 1) == -1) {
      return;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionDropAllReferences, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return;
   } else {
      fn->dropAllReferences();
      return;
   }
}
/* Getters */
#define name_LLVMFunctionDoesNotAlias \
   msg("llvm-function-does-not-alias")
extern "C" void* LLVMFunctionDoesNotAlias(void* theEnv) {
   DATA_OBJECT ptrDO,
               indexDO;
   unsigned index; 
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionDoesNotAlias, 
            EXACTLY, 2) == -1) {
      return FalseSymbol();
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionDoesNotAlias, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return FalseSymbol();
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionDoesNotAlias, 2, 
            INTEGER, &indexDO) == FALSE) {
      return FalseSymbol();
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return FalseSymbol();
   } else {
      index = (unsigned)DOToInteger(indexDO);
      if(fn->doesNotAlias(index)) {
         return TrueSymbol();
      } else {
         return FalseSymbol();
      }
   }
}
#define name_LLVMFunctionDoesNotCapture \
   msg("llvm-function-does-not-capture")
extern "C" void* LLVMFunctionDoesNotCapture(void* theEnv) {
   DATA_OBJECT ptrDO,
               indexDO;
   unsigned index; 
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionDoesNotCapture, 
            EXACTLY, 2) == -1) {
      return FalseSymbol();
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionDoesNotCapture, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return FalseSymbol();
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionDoesNotCapture, 2, 
            INTEGER, &indexDO) == FALSE) {
      return FalseSymbol();
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return FalseSymbol();
   } else {
      index = (unsigned)DOToInteger(indexDO);
      if(fn->doesNotCapture(index)) {
         return TrueSymbol();
      } else {
         return FalseSymbol();
      }
   }
}
#define name_LLVMFunctionGetParamAlignment \
   msg("llvm-function-get-param-alignment")
extern "C" unsigned LLVMFunctionGetParamAlignment(void* theEnv) {
   DATA_OBJECT ptrDO,
               indexDO;
   unsigned index; 
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionGetParamAlignment, 
            EXACTLY, 2) == -1) {
      return 0;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionGetParamAlignment, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return 0;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionGetParamAlignment, 2, 
            INTEGER, &indexDO) == FALSE) {
      return 0;
   }
   Function* fn = (Function*)(PointerAddress)DOToLong(ptrDO);
   if(!fn) {
      EnvPrintRouter(theEnv, werror,
            msg("ERROR: The provided function pointer isn't valid"));
      return 0;
   } else {
      index = (unsigned)DOToInteger(indexDO);
      return fn->getParamAlignment(index);
   }
}
#define str(x) #x
#define getClipsName(n) name_ ## n
#define SimpleEnvDefineFunction(e,n,r) \
   EnvDefineFunction(e, getClipsName(n) , r, PTIEF n, msg(str(n)))
extern "C" void RegisterLLVMFunctionManipulationFunctions(void *theEnv) {
   EnvDefineFunction(theEnv, name_LLVMFunctionDoesNotCapture, 'b', 
         PTIEF LLVMFunctionDoesNotCapture, 
         msg("LLVMFunctionDoesNotCapture"));
   EnvDefineFunction(theEnv, name_LLVMFunctionDoesNotAlias, 'b', 
         PTIEF LLVMFunctionDoesNotAlias, 
         msg("LLVMFunctionDoesNotAlias"));
   EnvDefineFunction(theEnv, name_LLVMFunctionGetParamAlignment, 'i', 
         PTIEF LLVMFunctionGetParamAlignment, 
         msg("LLVMFunctionGetParamAlignment"));
   SimpleEnvDefineFunction(theEnv, LLVMFunctionSetGC, 'v');
}
#undef werror
#undef name_LLVMFunctionDoesNotCapture
#undef name_LLVMFunctionDoesNotAlias
#undef name_LLVMFunctionGetParamAlignment
#undef name_LLVMFunctionSetOnlyReadsMemory
#undef name_LLVMFunctionSetGC
#undef name_LLVMFunctionClearGC
#undef name_LLVMFunctionDeleteBody
#undef name_LLVMFunctionRemoveFromParent
#undef name_LLVMFunctionEraseFromParent
#undef name_LLVMFunctionSetDoesNotAccessMemory
#undef name_LLVMFunctionSetDoesNotAlias
#undef name_LLVMFunctionSetDoesNotCapture
#undef name_LLVMFunctionSetDoesNotThrow
#undef name_LLVMFunctionSetHasUWTable
#undef name_LLVMFunctionSetDoesNotReturn
#undef name_LLVMFunctionSetCannotDuplicate
#undef name_LLVMFunctionDropAllReferences
#undef name_LLVMFunctionHasAddressTaken
#undef msg
/* End C Interaction Functions */
