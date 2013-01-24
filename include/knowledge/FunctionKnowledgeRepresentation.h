#ifndef _global_value_knowledge_representation_h
#define _global_value_knowledge_representation_h
#include "knowledge/Traits.h"
#include "knowledge/Types.h"
#include "llvm/Function.h"
namespace knowledge {
   DECLARE_CLIPS_TYPE_NAME(llvm::Function, "LLVMFunction");
   DECLARE_HAS_KNOWLEDGE_REPRESENTATION_POPULATION_LOGIC(llvm::Function);
   DECLARE_HAS_CUSTOM_ROUTER_LOGIC(llvm::Function);
   template<>
      struct CustomRouterLogic<true> {
         template<>
            static std::string route<llvm::Function>(llvm::Function* obj,
                  KnowledgeConstructor* kc, char* parent) {
               void* f = (void*)obj;
               if(kc->isModulePass() || 
                     (kc->isFunctionPass() && 
                      kc->getTargetOfPass() == f)) {
                  return CustomRouterLogic<false>::route(obj, kc, parent);
               } else {
                  //just register the thing and go on our merry way
                  std::string n = GetUniqueName(obj, kc);
                  std::string& name = n;
                  kc->registerKnowledgeName((PointerAddress)obj, name);
                  return name;
               }
            }
      };
   template<>
      struct KnowledgeRepresentationPopulationLogic<llvm::Function> {
         static void setCallingConvention(CallingConv::ID id,
               KnowledgeRepresentationBuilder* krb) {
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
            krb->addField("CallingConvention", selection);
         }
         static void populateKnowledgeRepresentation(llvm::Function* func,
               KnowledgeRepresentationBuilder* krb,
               KnowledgeConstructor* kc) {
            PopulateKnowledgeRepresentation((llvm::GlobalValue*)func, krb, kc); 
            krb->addField("ReturnType", Route(func->getReturnType(), kc, (char*)""));
            krb->addField("FunctionType", Route(func->getFunctionType(), kc, (char*)""));

            if(func->isVarArg()) {
               krb->addTrueField("IsVarArg");
            }
            krb->addField("IntrinsicID", func->getIntrinsicID());
            if(func->isIntrinsic()) {
               krb->addTrueField("IsIntrinsic");
            }
            setCallingConvention(func->getCallingConv(), krb);
            // setAttributes(func->getAttributes());
            if(func->hasGC()) {
               krb->addTrueField("HasGC");
               krb->addField("GC", func->getGC());
            }
            if(func->doesNotAccessMemory()) {
               krb->addTrueField("DoesNotAccessMemory");
            }
            if(func->onlyReadsMemory()) {
               krb->addTrueField("OnlyReadsMemory");
            }
            if(func->doesNotReturn()) {
               krb->addTrueField("DoesNotReturn");
            }
            /*
               if(func->cannotDuplicate()) {
               addTrueField("CannotDuplicate");
               }
               */
            if(func->hasUWTable()) {
               krb->addTrueField("HasUWTable");
            }
            if(func->needsUnwindTableEntry()) {
               krb->addTrueField("NeedsUnwindTableEntry");
            }
            if(func->hasStructRetAttr()) {
               krb->addTrueField("HasStructRetAttr");
            }
            if(func->isDefTriviallyDead()) {
               krb->addTrueField("IsDefTriviallyDead");
            }
            if(func->callsFunctionThatReturnsTwice()) {
               krb->addTrueField("CallsFunctionThatReturnsTwice");
            }
            char* fnName = (char*)func->getName().data();
            BasicBlock& bb = func->getEntryBlock();

            krb->addField("EntryBlock", Route(&bb, kc, fnName));
            krb->openMultifield("Arguments"); {
               for(Function::arg_iterator s = func->arg_begin(), f = func->arg_end(); 
                     s != f; ++s) {
                  Argument* a = s;
                  //build it if we need to :)
                  krb->appendValue(Route(a, kc, fnName));
               }
            } krb->closeMultifield();
            krb->openMultifield("contents"); {
               for(llvm::Function::iterator i = func->begin() , e = func->end(); 
                     i != e; ++i) {
                  BasicBlock* bb = i; 
                  krb->appendValue(Route(bb, kc, fnName));
               }
            } krb->closeMultifield();
         }
      };
}
#endif
