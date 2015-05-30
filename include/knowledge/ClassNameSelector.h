#ifndef _CLASS_NAME_SELECTOR_H_
#define _CLASS_NAME_SELECTOR_H_
#include "llvm/Support/Casting.h"
namespace knowledge {
template<typename T>
struct ElectronClassNameSelector {
	static void selectName(llvm::raw_string_ostream& str) {
		str << "";
	}
};

#define ElectronClassNameAssociation(type, className) \
	template<> \
struct ElectronClassNameSelector<type> { \
	static void selectName(llvm::raw_string_ostream& str) { \
		str << className ; \
	} \
}

}
#endif // _CLASS_NAME_SELECTOR_H_
