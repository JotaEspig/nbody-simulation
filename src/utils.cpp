#include <filesystem>
#include <string>

#include "utils.hpp"

std::string path(const std::string &filename) {
    // get absolute path
    std::filesystem::path p = PROJECT_DIR;
    p /= filename;
    return p.string();
}
