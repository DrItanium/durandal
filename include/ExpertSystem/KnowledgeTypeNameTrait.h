#ifndef _knowledge_type_name_trait_h
#define _knowledge_type_name_trait_h
namespace knowledge {
   template<typename T>
      struct HasKnowledgeTypeNameTrait {
         static const bool hasName = false;
      };
#define DECLARE_HAS_KNOWLEDGE_TYPE_NAME(Type) \
   template<> \
   struct HasKnowledgeTypeNameTrait< Type > { \
      static const bool hasName = true; \
   }; 


   template<typename T>
      struct KnowledgeTypeNameTrait {
         static const char* getTypeName() {
            return "";
         }
      };
#define DECLARE_KNOWLEDGE_TYPE_NAME(Type, Name) \
   template<> \
   struct KnowledgeTypeNameTrait< Type > { \
      static const char* getTypeName() { \
         return Name ; \
      } \
   };

#define DECLARE_KNOWLEDGE_CONVERSION(Type, Name) \
   DECLARE_HAS_KNOWLEDGE_TYPE_NAME(Type) \
   DECLARE_KNOWLEDGE_TYPE_NAME(Type, Name)

#define DECLARE_KNOWLEDGE_CONVERSION_SYMBOL(Type, Name) \
   DECLARE_KNOWLEDGE_CONVERSION(Type, #Name)

   template<typename T> const char* GetTypeName();
}
#endif
