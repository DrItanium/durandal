#ifndef _traits_h
#define _traits_h
#include "knowledge/KnowledgeConstructor.h"
#include "knowledge/KnowledgeRepresentationBuilder.h"
#define DECLARE_KNOWLEDGE_CONVERSION_LOGIC namespace knowledge
namespace knowledge {

   template<typename T>
      struct HasCustomNameGenerationLogic {
         static const bool value = false;
      };

#define DECLARE_HAS_CUSTOM_NAME_GENERATION_LOGIC(Type) \
   template<> \
   struct HasCustomNameGenerationLogic< Type > { \
      static const bool value = true; \
   };
   template<bool b> 
      struct CustomNameGenerationLogic {
         template<typename T>
            static std::string getUniqueName(T* obj, KnowledgeConstructor* kc) {
               if(obj->hasName()) {
                  std::string tmp(obj->getName().data());
                  return tmp;
               } else {
                  char* tmp = (char*)calloc(128, sizeof(char));
                  kc->gensym(tmp);
                  std::string name(tmp);
                  free(tmp);
                  return name;
               }
            }
      };

   template<typename T>
      std::string GetUniqueName(T* obj, KnowledgeConstructor* kc) {
         return CustomNameGenerationLogic<
            HasCustomNameGenerationLogic<T>::value>::getUniqueName(obj, kc);
      }

   template<typename T>
      struct HasCLIPSTypeNameDefined {
         static const bool value = false;
      };
#define DECLARE_TYPE_HAS_CLIPS_REPRESENTATION(Type) \
   template<> \
   struct HasCLIPSTypeNameDefined< Type > { \
      static const bool value = true; \ 
   };

   template<typename T>
      struct CLIPSTypeNameDefined {
         static const char* getTypeName() {
            return "";
         }
      };

#define DECLARE_CLIPS_TYPE_NAME(CPPType, CLIPSTypeName) \
   DECLARE_TYPE_HAS_CLIPS_REPRESENTATION(CPPType) \
   template<> \
   struct CLIPSTypeNameDefined < CPPType > { \
      static const char* getTypeName() { \
         return CLIPSTypeName; \
      } \
   };

#define DECLARE_CLIPS_TYPE_NAME_SYMBOL(CPPType, CLIPSTypeName) \
   DECLARE_CLIPS_TYPE_NAME(CPPType, #CLIPSTypeName )

   template<typename T>
      const char* GetTypeName() {
         if(HasCLIPSTypeNameDefined<T>::value) {
            return CLIPSTypeNameDefined<T>::getTypeName();
         } else {
            assert(false && 
                  "ERROR: Type name missing for knowledge conversion\n");
            return "";
         }
      }
   template<typename T>
      struct HasKnowledgeRepresentationPopulationLogic {
         static const bool value = false;
      };
#define DECLARE_HAS_KNOWLEDGE_REPRESENTATION_POPULATION_LOGIC(Type) \
   template<> \
   struct HasKnowledgeRepresentationPopulationLogic< Type > { \
      static const bool value = true; \
   };
   template<typename T>
      struct KnowledgeRepresentationPopulationLogic {
         static void populateKnowledgeRepresentation(
               T* obj,
               KnowledgeRepresentationBuilder* krb,
               KnowledgeConstructor* kc) {
            //do nothing
         }
      };
   template<typename T>
      void PopulateKnowledgeRepresentation(T* obj, 
            KnowledgeRepresentationBuilder* krb, 
            KnowledgeConstructor* kc) {
         if(HasKnowledgeRepresentationPopulationLogic<T>::value) {
            KnowledgeRepresentationPopulationLogic<
               T>::populateKnowledgeRepresentation(obj, krb, kc, parent);
         } else {
            assert( false && "ERROR: No population logic provided for selected type");
            //just in case assertions are disabled
            KnowledgeRepresentationPopulationLogic<T>::
               populateKnowledgeRepresentation(obj, krb, kc, parent);
         }
      }
   template<typename T> 
      struct HasCustomRouterLogic {
         static const bool value = false;
      };

   template<bool b>
      struct CustomRouterLogic {
         template<typename T> 
            static std::string route(T* obj, 
                  KnowledgeConstructor* kc, 
                  char* parent) {
               //first we need to get a name as well as the type name
               std::string n = GetUniqueName(obj, kc);
               std::string& name = n;
               kc->registerKnowledgeName((PointerAddress)obj, name);
               std::string type = GetTypeName<T>();
               KnowledgeRepresentationBuilder* krb = new 
                  KnowledgeRepresentationBuilder(name, type, 
                        (PointerAddress)obj, parent);
               krb->open();
               PopulateKnowledgeRepresentation(obj, krb, kc);
               krb->close();
               std::string tmp = krb->getCompletedString();
               kc->addToInstances(tmp);
               delete krb;
               return name;
            }
      };

   template<typename T>
      std::string Route(T* obj, KnowledgeConstructor* kc, char* parent) {
         if(kc->objectAlreadyConvertedToKnowledge((PointerAddress)obj)) {
            return kc->getObjectKnowledgeName((PointerAddress)obj);
         } else {
            return CustomRouterLogic<HasCustomRouterLogic<T>::value>::route(
                  obj, kc, parent);
         }
      }
}
#endif
