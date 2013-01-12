#include "ExpertSystem/CLIPSFunctionBuilder.h"

CLIPSFunctionBuilder::CLIPSFunctionBuilder(std::string nm, 
      FunctionNamer& namer) : CLIPSGlobalValueBuilder(nm, "Function",  namer) { 
}

void CLIPSFunctionBuilder::build(Function* fn, KnowledgeConstructor* kc, bool traverseFields) {
   open();
   addFields(fn, kc, traverseFields);
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

void CLIPSFunctionBuilder::addFields(Function* func, KnowledgeConstructor* kc, bool traverseFields) {
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
   setCallingConvention(func->getCallingConv(), kc);

   if(traverseFields) {

   }
}

