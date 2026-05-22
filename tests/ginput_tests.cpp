#include "ginput/ginput.hpp"

#include <cstdlib>
#include <iostream>

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << "\n";
        std::exit(1);
    }
}

} // namespace

int main() {
    require(ginput::version_major() == 0, "version placeholder");
    std::cout << "ginput_tests passed\n";
    return 0;
}
