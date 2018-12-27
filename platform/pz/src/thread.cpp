#include <mbgl/util/platform.hpp>

#include <string>

namespace mbgl {
namespace platform {

std::string getCurrentThreadName() {
	return "unknown";
}

void setCurrentThreadName(const std::string& name) {
	// TODO
}

void makeThreadLowPriority() {
	// TODO
}

} // namespace platform
} // namespace mbgl
