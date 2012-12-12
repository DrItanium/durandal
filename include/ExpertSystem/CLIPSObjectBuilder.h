#ifndef _clips_object_builder_h
#define _clips_object_builder_h
#include <string.h>
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringRef.h"
#include "ExpertSystem/Types.h"
#include "ExpertSystem/FunctionNamer.h"
#include "ExpertSystem/KnowledgeConstructor.h"


using namespace llvm;
class CLIPSObjectBuilder {
   private:
      bool converted;
      bool opened;
      bool closed;
      std::string name;
      std::string type;
      std::string tmp;
      raw_string_ostream* stream;
      FunctionNamer* namer;
   public:
      CLIPSObjectBuilder(std::string n, std::string t, FunctionNamer& uidCreator);
      ~CLIPSObjectBuilder();
      std::string getCompletedString();
      std::string& getName();
      std::string& getType();
      FunctionNamer& getNamer();
      raw_string_ostream& getStream();
      bool isOpen();
      bool isClosed(); 
      bool isKnowledge();
      void setParent(char* p); 
      void setParent(const char* p); 
      void setPointer(PointerAddress v); 
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
		void openField(const char* name);
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
		void closeField();
      void open();
      void close();
      void convertToKnowledge(void* theEnv);
      void addFields(PointerAddress pointer, KnowledgeConstructor* kc, char* parent);
};

#endif
