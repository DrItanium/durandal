#include "ExpertSystem/CLIPSRegionBuilder.h"
CLIPSRegionBuilder::CLIPSRegionBuilder(std::string nm, FunctionNamer& namer) : CLIPSObjectBuilder(nm, "Region", namer) { }
void CLIPSRegionBuilder::addFields(Region* region, KnowledgeConstructor* kc, char* parent) {
   //we should edit the parent to be a loop if it turns out that we
   //are part of a loop
   CLIPSObjectBuilder::addFields((PointerAddress)region, kc, parent);
   addField("Depth", region->getDepth());
   if(region->isTopLevelRegion()) addTrueField("IsTopLevelRegion");
   if(region->isSimple()) addTrueField("IsSimple");
   BasicBlock* entry = region->getEntry();
   BasicBlock* exit = region->getExit();
   if(entry != NULL) addInstanceNameField("Entrances", entry->getName());
   if(exit != NULL) addInstanceNameField("Exits", exit->getName());
   char* name = (char*)getName().c_str();
   FunctionNamer& namer = getNamer();
   //open contents 
   openField((char*)"contents");
   for(Region::element_iterator i = region->element_begin(), e = region->element_end(); i != e; ++i) {
      RegionNode* rn = *i;
      if(!rn->isSubRegion()) {
         BasicBlock* bb = rn->getEntry(); 
         appendInstanceName(kc->route(bb, namer, name));
      } else {
         Region* subRegion = rn->getNodeAs<Region>();
         appendInstanceName(kc->route(subRegion, namer, name));
      }
   }
   closeField();
}
void CLIPSRegionBuilder::build(Region* r, KnowledgeConstructor* kc, char* parent) {
   open();
   addFields(r, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)r, str);
}
