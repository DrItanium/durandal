namespace knowledge {
   char openParen[2] = "(";
   char closeParen[2] = ")";
   char space[2] = " ";
   std::string KnowledgeRepresentationBuilder::getCompletedString() { return stream->str(); }
   KnowledgeRepresentationBuilder::KnowledgeRepresentationBuilder(
         std::string n, 
         std::string t, 
         PointerAddress p, 
         const char* pr) {
      opened = false;
      closed = false;
      in_multifield = false;
      name = new std::string(n.c_str());
      type = new std::string(n.c_str());
      tmp = new std::string();
      parent = new std::string(pr);
      ptr = p;
      stream = new raw_string_ostream(&tmp);
   }
   KnowledgeRepresentationBuilder::~KnowledgeRepresentationBuilder() {
      delete stream;
      delete type;
      delete tmp;
      delete parent;
      delete name;
   }
   void KnowledgeRepresentationBuilder::addField(const char* n, unsigned v) { 
      if(v != 0) {
         (*stream) << openParen << n << space << v << closeParen; 
      }
   }
   void KnowledgeRepresentationBuilder::addField(const char* n, PointerAddress v) { 
      if(v != 0) {
         (*stream) << openParen << n << space << v << closeParen; 
      }
   }
   void KnowledgeRepresentationBuilder::addField(const char* n, float v) { 
      (*stream) << openParen << n << space << v << closeParen; 
   }
   void KnowledgeRepresentationBuilder::addField(const char* n, double v) { 
      (*stream) << openParen << n << space << v << closeParen; 
   }
   void KnowledgeRepresentationBuilder::addField(const char* n, char* v) { 
      (*stream) << openParen << n << space << v << closeParen; 
   }
   void KnowledgeRepresentationBuilder::addField(const char* n, const char* v) { 
      (*stream) << openParen << n << space << v << closeParen; 
   }
   void KnowledgeRepresentationBuilder::addField(const char* n, std::string v) {
      if(v.length() > 0) (*stream) << openParen << n << space << v << closeParen; 
   }
   void KnowledgeRepresentationBuilder::addField(const char* n, bool v) { 
      (*stream) << openParen << n;
      if(v) {
         (*stream) << " TRUE)";
      } else {
         (*stream) << " FALSE)";
      }
   }
   void KnowledgeRepresentationBuilder::addField(const char* n, int v) { 
      if(v != 0) {
         (*stream) << openParen << n << space << v << closeParen; 
      }
   }
   void KnowledgeRepresentationBuilder::addField(const char* n, uint64_t v) { 
      if(v != 0) {
         (*stream) << openParen << n << space << v << closeParen; 
      }
   }
   void KnowledgeRepresentationBuilder::addField(const char* n, int64_t v) {
      if(v != 0) {
         (*stream) << openParen << n << space << v << closeParen; 
      }
   }
   void KnowledgeRepresentationBuilder::addField(const char* n, llvm::StringRef ref) {
      (*stream) << openParen << n << space << ref << closeParen;
   }
   void KnowledgeRepresentationBuilder::open() {
      if(!opened) {
         (*stream) << "(" << name << " of " << type << " ";
         addField("pointer", ptr);
         addField("parent", parent.c_str());
         opened = true;
      }
   }
   void KnowledgeRepresentationBuilder::close() {
      if(!closed) {
         (*stream) << closeParen; //close it
         closed = true;
      }
   }

   void KnowledgeRepresentationBuilder::addTrueField(const char* n) {
      (*stream) << openParen << n << " TRUE)";
   }
   void KnowledgeRepresentationBuilder::addFalseField(const char* n) {
      (*stream) << openParen << n << " FALSE)";
   }
   void KnowledgeRepresentationBuilder::appendValue(unsigned v) {
      if(in_multifield) {
         (*stream) << space << v;
      }
   }
   void KnowledgeRepresentationBuilder::appendValue(PointerAddress v) {
      if(in_multifield) {
         (*stream) << space << v;
      }
   }
   void KnowledgeRepresentationBuilder::appendValue(int v) {
      if(in_multifield) {
         (*stream) << space << v;
      }
   }
   void KnowledgeRepresentationBuilder::appendValue(float v) {
      if(in_multifield) { 
         (*stream) << space << v;
      }
   }
   void KnowledgeRepresentationBuilder::appendValue(double v) {
      if(in_multifield) {
         (*stream) << space << v;
      }
   }
   void KnowledgeRepresentationBuilder::appendValue(char* v) {
      if(in_multifield) {
         (*stream) << space << v;
      }
   }
   void KnowledgeRepresentationBuilder::appendValue(const char* v) {
      if(in_multifield) {
         (*stream) << space << v;
      }
   }
   void KnowledgeRepresentationBuilder::appendValue(std::string v) {
      if(in_multifield && !v.empty()) {
         (*stream) << space << v;
      }
   }
   void KnowledgeRepresentationBuilder::appendValue(llvm::StringRef v) {
      if(in_multifield) {
         (*stream) << space << v;
      }
   }
   void KnowledgeRepresentationBuilder::appendValue(bool v) {
      if(in_multifield) {
         if(v) {
            appendTrue();
         } else {
            appendFalse();
         }
      }
   }
   void KnowledgeRepresentationBuilder::appendValue(uint64_t v) {
      if(in_multifield) {
         (*stream) << space << v;
      }
   }
   void KnowledgeRepresentationBuilder::appendValue(int64_t v) {
      if(in_multifield) {
         (*stream) << space << v;
      }
   }
   void KnowledgeRepresentationBuilder::appendTrue() {
      if(in_multifield) {
         (*stream) << " TRUE";
      }
   }
   void KnowledgeRepresentationBuilder::appendFalse() {
      if(in_multifield) {
         (*stream) << " FALSE";
      }
   }
   void KnowledgeRepresentationBuilder::openMultifield(const char* name) {
      if(!in_multifield) {
         (*stream) << openParen << name;
         in_multifield = true;
      }
   }
   void KnowledgeRepresentationBuilder::closeMultifield() {
      if(in_multifield) {
         (*stream) << closeParen;
         in_multifield = false;
      }
   }
   void KnowledgeRepresentationBuilder::addStringField(const char* n, const std::string& string) {
      if(string.size() > 0) {
         (*stream) << "(" << n << " \"" << string << "\")";
      }
   }
}
