#include "pipeline/clips/CLIPSPassHeader.h"

namespace pipeline {
	namespace clips {
		CLIPSPassHeader::CLIPSPassHeader() { }

		bool CLIPSPassHeader::needsRegions() {
			return needRegions;
		}

		void CLIPSPassHeader::setNeedsRegions(bool v) {
			needRegions = v;
		}

		bool CLIPSPassHeader::needsLoops() {
			return needLoops;
		}

		void CLIPSPassHeader::setNeedsLoops(bool v) {
			needLoops = v;
		}

		char* CLIPSPassHeader::getPasses() {
			return passes;
		}

		void CLIPSPassHeader::setPasses(char* p) {
			passes = p;	
		}
	}
}
