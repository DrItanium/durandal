// program to generate the corresponding CLIPS defclasses that the knowledge
// construction engine uses
#include "knowledge/ClassSelector.h"

#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__
#define EndNode };
template<typename T>
struct ElectronClassInheritanceHierarchy {
	static void getSupertypes(llvm::raw_string_ostream& str) {
		str << "USER";
	}
};

#define ElectronClassInheritanceAssociation(type, inherits) \
	template<> \
struct ElectronClassInheritanceHierarchy<type> { \
	static void getSupertypes(llvm::raw_string_ostream& str) { \
		str << inherits ; \
	} \
}
#define X(unused, type, unused2, inherits) \
	ElectronClassInheritanceAssociation(type, inherits);
#include "knowledge/EngineNodes.def"
#undef X
template<typename T>
struct ElectronClassSlotBuilder {
	static void AddSlots(llvm::raw_string_ostream& str) { }
};
template<typename T>
void defslot(llvm::raw_string_ostream& str, const std::string& name) {
	str << "(slot " << name << "\n"
		<< "(visibility public))\n";
}
template<typename T>
void defmultislot(llvm::raw_string_ostream& str, const std::string& name) {
	str << "(multislot " << name << "\n"
		<< "(visibility public))\n";
}

#define defdefslot(type) \
	template<> \
	void defslot<type>(llvm::raw_string_ostream& str, const std::string& name) 

template<>
void defslot<bool>(llvm::raw_string_ostream& str, const std::string& name) {
	str << "(slot " << name << "\n"
		<< "(type SYMBOL)\n"
		<< "(visibility public)\n"
		<< "(allowed-values FALSE\nTRUE))\n";
}
#define defstring_defslot(type) \
template<> \
void defslot<type>(llvm::raw_string_ostream& str, const std::string& name) { \
	str << "(slot " << name << "\n" \
		<< "(type LEXEME)\n" \
		<< "(visibility public))\n"; \
}

defstring_defslot(std::string)
defstring_defslot(llvm::StringRef)
defstring_defslot(const std::string &)
defstring_defslot(std::string&)
defstring_defslot(const std::string)
defstring_defslot(const char*)
defstring_defslot(char*)

template<> 
void defslot<unsigned int>(llvm::raw_string_ostream& str, const std::string& name) { 
	str << "(slot " << name << "\n" 
		<< "(type NUMBER)\n" 
		<< "(visibility public)\n" 
		<< "(range 0 ?VARIABLE))\n"; 
}
template<>
void defslot<int>(llvm::raw_string_ostream& str, const std::string& name) {
	str << "(slot " << name << "\n" 
		<< "(type NUMBER)\n" 
		<< "(visibility public))\n";
}

#define slot(type, name) \
		defslot<type>(str, name)
#define multislot(type, name) \
		defmultislot<type>(str, name)

template<typename T>
struct DefClassBuilderNode {
	static void populateSlots(llvm::raw_string_ostream& str) { }
};
//Build DefClass Node
#define BeginDefClassBuilderNode(type) \
	template<> \
	struct DefClassBuilderNode<type> { \
		static void populateSlots(llvm::raw_string_ostream& str)

#define EndDefClassBuilderNode EndNode

//#include "knowledge/defclass_slots.def"
#include "knowledge/defclass_enums.def"
#define Begin(type) \
		BeginDefClassBuilderNode(llvm:: type) {
#define End } EndDefClassBuilderNode
#define super(_) 
#define XReference(vtype, name) slot(vtype, name);
#define XValue(vtype, name) slot(vtype, name);
#define XReference(vtype, name) slot(vtype, name);
#define XMultifield(vtype, name) multislot(vtype, name);
#define XConditionalValue(vtype, name) slot(vtype, name);
#define X(vtype, name, _, type,  ...) \
		CAT(X, type)(vtype, name)
#include "knowledge/builder_nodes.def"
#undef X
#undef super
#undef Begin
#undef End
#undef XValue
#undef XConditionalValue
#undef XMultifield
#undef XReference

template<typename T>
void defclass() {
	std::string tmp;
	llvm::raw_string_ostream str(tmp);
	str << "(defclass ";
	knowledge::ElectronClassNameSelector<T>::selectName(str);
	str << "\n(is-a ";
	ElectronClassInheritanceHierarchy<T>::getSupertypes(str);
	str << ")\n ";
	// slots are going to go here, need another router group to dispatch for
	// this purpose
	DefClassBuilderNode<T>::populateSlots(str);
	str << ")\n";
	// print it out at the end
	printf("%s\n", str.str().c_str());
}

void generateDefClasses() {
#define X(_, fullType, __, ___) \
	defclass<fullType>();
#include "knowledge/EngineNodes.def"
#undef X
}

int main(int argc, char** argv) {
	generateDefClasses();
}
