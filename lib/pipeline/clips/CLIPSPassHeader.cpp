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

		const char* CLIPSPassHeader::getPasses() {
			return passes.c_str();
		}

		void CLIPSPassHeader::setPasses(const char* p) {
			std::string tmp(p);
			passes = tmp;
		}
	}
}
