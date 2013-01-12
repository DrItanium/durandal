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
   addField("ReturnType", kc->route(fn->getReturnType(), namer));
   addField("FunctionType", kc->route(fn->getFunctionType(), namer));
   if(fn.isVarArg()) {
      addTrueField("IsVarArg");
   }
   addField("IntrinsicID", fn->getIntrinsicID());
   if(fn.isIntrinsic()) {
      addTrueField("IsIntrinsic");
   }
   setCallingConvention(func->getCallingConv());
   setAttributes(func->getAttributes());
   if(fn.hasGC()) {
      addTrueField("HasGC");
   }
   if(fn.doesNotAccessMemory()) {
      addTrueField("DoesNotAccessMemory");
   }
   if(fn.onlyReadsMemory()) {
      addTrueField("OnlyReadsMemory");
   }
   if(fn.doesNotReturn()) {
      addTrueField("DoesNotReturn");
   }
   if(fn.cannotDuplicate()) {
      addTrueField("CannotDuplicate");
   }
   if(fn.hasUWTable()) {
      addTrueField("HasUWTable");
   }
   if(fn.needsUnwindTableEntry()) {
      addTrueField("NeedsUnwindTableEntry");
   }
   if(fn.hasStructRetAttr()) {
      addTrueField("HasStructRetAttr");
   }
   //TODO: Expose doesNotAlias, doesNotCapture, and getParamAttributes to 
   //CLIPS as functions
   char* fnName = (char*)fn.getName().data();
   FunctionNamer& namer = getNamer();
   addField("EntryBlock", kc->route(fn.getEntryBlock(), fnName, namer));

   if(traverseFields) {

   }
}
#define msg(x) (char*) x
#define werror msg(werror)
#define name_LLVMFunctionDoesNotAlias msg("llvm-function-does-not-alias")
#define name_LLVMFunctionDoesNotCapture msg("llvm-function-does-not-capture")
#define name_LLVMFunctionGetParamAlignment \
   msg("llvm-function-get-param-alignment")
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
#undef msg
