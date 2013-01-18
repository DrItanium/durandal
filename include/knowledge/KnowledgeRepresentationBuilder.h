#ifndef _knowledge_representation_builder_h
#define _knowledge_representation_builder_h
#include "llvm/Support/raw_ostream.h"
#include "knowledge/Types.h"
namespace knowledge {
   class KnowledgeRepresentationBuilder {
      bool opened;
      bool closed;
      bool in_multifield;
      llvm::raw_string_ostream* stream;
      std::string* name;
      std::string* type;
      std::string* parent;
      std::string* tmp;
      PointerAddress ptr;
      public:
      KnowledgeRepresentationBuilder(std::string& n, std::string& t, 
            PointerAddress p, const char* parent);
      ~KnowledgeRepresentationBuilder();
      std::string getCompletedString();
      bool isOpen() { return opened; }
      bool isClosed() { return closed; }
      bool inMultifield() { return in_multifield; }
      std::string& getName() { return *name; }
      void addField(const char* n, unsigned v);
      void addField(const char* n, PointerAddress v); 
      void addField(const char* n, int v); 
      void addField(const char* n, float v);
      void addField(const char* n, double v); 
      void addField(const char* n, char* v); 
      void addField(const char* n, const char* v); 
      void addField(const char* n, std::string v); 
		void addField(const char* n, llvm::StringRef ref);
      void addField(const char* n, bool v); 
      void addField(const char* n, uint64_t v);
      void addField(const char* n, int64_t v); 
      void addStringField(const char* n, const std::string& str);
		void addTrueField(const char* n);
		void addFalseField(const char* n);
		void openMultifield(const char* name);
		void appendValue(unsigned v);
		void appendValue(PointerAddress v);
		void appendValue(int v);
		void appendValue(float v);
		void appendValue(double v);
		void appendValue(char* v);
		void appendValue(const char* v);
		void appendValue(std::string v);
		void appendValue(llvm::StringRef ref);
		void appendValue(bool v);
		void appendValue(uint64_t v);
		void appendValue(int64_t v);
		void appendTrue();
		void appendFalse();
		void closeMultifield();
      void open();
      void close();
   };
}

#endif
