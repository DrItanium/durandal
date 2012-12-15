#include "ExpertSystem/CLIPSModuleBuilder.h"
#include "llvm/Module.h"

using namespace llvm;

CLIPSModuleBuilder::CLIPSModuleBuilder(std::string nm, std::string ty) : CLIPSObjectBuilder(nm, ty) { }
void CLIPSModuleBuilder::build(Module* mod, KnowledgeConstructor* kc) {
   open(); 
   addField("pointer", (PointerAddress)mod);
   addFields(mod, kc);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)mod, str);
}

void CLIPSModuleBuilder::addFields(Module* mod, KnowledgeConstructor* kc) {
   std::string triple(mod->getTargetTriple());
   std::string dataLayout(mod->getDataLayout());
   std::string modIdent(mod->getModuleIdentifier());
   llvm::errs() << "Target triple is " << triple << "\n";
   addField("triple", triple); 
   addField("data-layout", dataLayout);
   addField("module-identifier", modIdent);
   Module::Endianness endian = mod->getEndianness();
   Module::PointerSize psize = mod->getPointerSize();
   if(endian == Module::LittleEndian) {
            addField("endianness", "little");
   } else if (endian == llvm::Module::BigEndian) {
            addField("endianness", "big");
   } else {
            addField("endianness", "any");
   }

   if(psize == Module::Pointer32) {
         addField("pointer-size", "pointer32");
   } else if (psize == Module::Pointer64) {
         addField("pointer-size", "pointer64");
   } else {
         addField("pointer-size", "any");
   }
   addField("inline-asm", mod->getModuleInlineAsm());
}
