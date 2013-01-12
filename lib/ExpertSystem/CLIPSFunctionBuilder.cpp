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
   kc->addToKnowledgeBase((PointerAddress)arg, str);
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
      case CallingConv::HiPE:
         selection = "HiPE";
         break;
      case CallingConv::FirstTargetCC:
         selection = "FirstTargetCC";
         break;
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
      case CallingConv::ARM_AAPCS_VFB:
         selection = "arm-aapcs-vfb";
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
      case CallingConv::SPIR_FUNC:
         selection = "spir-func";
         break;
      case CallingConv::SPIR_KERNEL:
         selection = "spir-kernel";
         break;
      case CallingConv::Intel_OCL_BI:
         selection = "intel-ocl-bi";
         break;
      default:
         selection = "Unknown";
         break;
   }
   addField("CallingConvention", selection);
}
void CLIPSFunctionBuilder::setAttributes(const AttributeSet& attr) {

}
void CLIPSFunctionBuilder::addFields(Function* func, 
      KnowledgeConstructor* kc) {

   CLIPSGlobalValueBuilder::addFields(func, kc);
   //this part contains the code for building the function itself
   FunctionNamer& namer = getNamer();
   addField("ReturnType", kc->route(func->getReturnType(), namer));
   addField("FunctionType", kc->route(func->getFunctionType(), namer));
   if(func.isVarArg()) {
      addTrueField("IsVarArg");
   }
   addField("IntrinsicID", func->getIntrinsicID());
   if(func.isIntrinsic()) {
      addTrueField("IsIntrinsic");
   }
   setCallingConvention(func->getCallingConv());
   setAttributes(func->getAttributes());
   if(func.hasGC()) {
      addTrueField("HasGC");
      addField("GC", func->getGC());
   }
   if(func.doesNotAccessMemory()) {
      addTrueField("DoesNotAccessMemory");
   }
   if(func.onlyReadsMemory()) {
      addTrueField("OnlyReadsMemory");
   }
   if(func.doesNotReturn()) {
      addTrueField("DoesNotReturn");
   }
   if(func.cannotDuplicate()) {
      addTrueField("CannotDuplicate");
   }
   if(func.hasUWTable()) {
      addTrueField("HasUWTable");
   }
   if(func.needsUnwindTableEntry()) {
      addTrueField("NeedsUnwindTableEntry");
   }
   if(func.hasStructRetAttr()) {
      addTrueField("HasStructRetAttr");
   }
   if(func.isDefTriviallyDead()) {
      addTrueField("IsDefTriviallyDead");
   }
   if(func.callsFunctionThatReturnsTwice()) {
      addTrueField("CallsFunctionThatReturnsTwice");
   }
   char* fnName = (char*)func.getName().data();
   FunctionNamer& namer = getNamer();
   addField("EntryBlock", kc->route(func.getEntryBlock(), fnName, namer));
   openField("Arguments"); {
      for(Function::arg_iterator s = func.arg_begin(), f = func.arg_end(); 
            s != f; ++s) {
         Argument* a = s;
         //build it if we need to :)
         appendValue(kc->route(a, namer, fnName));
      }
   } closeField();
   openField("contents"); {
      for(llvm::Function::iterator i = func.begin() , e = func.end(); 
            i != e; ++i) {
         BasicBlock* bb = i; 
         appendValue(kc->route(bb, namer, fnName));
      }
   } closeField();
}
/* Begin C functions for allowing CLIPS to interact with LLVM */
#define msg(x) (char*) x
#define werror msg(werror)
#define name_LLVMFunctionSetGC \
   msg("llvm-function-set-gc")
#define name_LLVMFunctionClearGC \
   msg("llvm-function-clear-gc")
#define name_LLVMFunctionDeleteBody \
   msg("llvm-function-delete-body")
#define name_LLVMFunctionRemoveFromParent \
   msg("llvm-function-remove-from-parent")
#define name_LLVMFunctionEraseFromParent \
   msg("llvm-function-erase-from-parent")
#define name_LLVMFunctionSetDoesNotAccessMemory \
   msg("llvm-function-set-does-not-access-memory")
#define name_LLVMFunctionSetDoesNotAlias \
   msg("llvm-function-set-does-not-alias")
#define name_LLVMFunctionSetDoesNotCapture \
   msg("llvm-function-set-does-not-capture")
#define name_LLVMFunctionSetDoesNotThrow \
   msg("llvm-function-set-does-not-throw")
#define name_LLVMFunctionSetHasUWTable \
   msg("llvm-function-set-has-uw-table")
#define name_LLVMFunctionSetDoesNotReturn \
   msg("llvm-function-set-does-not-return")
#define name_LLVMFunctionSetCannotDuplicate \
   msg("llvm-function-set-cannot-duplicate")
#define name_LLVMFunctionSetOnlyReadsMemory \
   msg("llvm-function-set-only-reads-memory")
#define name_LLVMFunctionDropAllReferences \
   msg("llvm-function-drop-all-references")
#define name_LLVMFunctionHasAddressTaken \
   msg("llvm-function-has-address-taken")
#define name_LLVMFunctionCreate \
   msg("llvm-function-create")
/* Getters */
#define name_LLVMFunctionDoesNotAlias \
   msg("llvm-function-does-not-alias")
extern "C" unsigned LLVMFunctionDoesNotAlias(void* theEnv) {
   DATA_OBJECT ptrDO,
               indexDO;
   unsigned index; 
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionDoesNotAlias, 
            EXACTLY, 2) == -1) {
      return 0;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionDoesNotAlias, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return 0;
   }
   if(EnvArgTypeCheck(theenv, name_LLVMFunctionDoesNotAlias, 1, 
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
      return fn->doesNotAlias(index);
   }
}
#define name_LLVMFunctionDoesNotCapture \
   msg("llvm-function-does-not-capture")
extern "C" unsigned LLVMFunctionDoesNotCapture(void* theEnv) {
   DATA_OBJECT ptrDO,
               indexDO;
   unsigned index; 
   if(EnvArgCountCheck(theEnv, name_LLVMFunctionDoesNotCapture, 
            EXACTLY, 2) == -1) {
      return 0;
   }
   if(EnvArgTypeCheck(theEnv, name_LLVMFunctionDoesNotCapture, 1, 
            INTEGER, &ptrDO) == FALSE) {
      return 0;
   }
   if(EnvArgTypeCheck(theenv, name_LLVMFunctionDoesNotCapture, 1, 
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
      return fn->doesNotCapture(index);
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
   if(EnvArgTypeCheck(theenv, name_LLVMFunctionGetParamAlignment, 1, 
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
extern "C" void RegisterLLVMFunctionManipulationFunctions(void *theEnv) {
   EnvDefineFunction(theEnv, name_LLVMFunctionDoesNotCapture, 'i', 
         PTIEF LLVMFunctionDoesNotCapture, 
         msg("LLVMFunctionDoesNotCapture"));
   EnvDefineFunction(theEnv, name_LLVMFunctionDoesNotAlias, 'i', 
         PTIEF LLVMFunctionDoesNotAlias, 
         msg("LLVMFunctionDoesNotAlias"));
   EnvDefineFunction(theEnv, name_LLVMFunctionGetParamAlignment, 'i', 
         PTIEF LLVMFunctionGetParamAlignment, 
         msg("LLVMFunctionGetParamAlignment"));
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
#undef name_LLVMFunctionCreate
#undef msg
/* End C Interaction Functions */
