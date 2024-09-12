#include <fstream>
#include <iostream>
#include <regex>

#include "app.hpp"

std::string get_version_from_file(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return "";
    }

    std::string line;
    std::regex version_pattern(R"(^\s*version\s*=\s*\"([^\"]+)\"\s*$)");
    std::smatch match;

    while (std::getline(file, line)) {
        if (std::regex_match(line, match, version_pattern)) {
            if (match.size() == 2) {
                file.close();
                return match[1];
            }
        }
    }

    file.close();
    return "";
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Invalid command" << std::endl
                  << "Usage: ./bin/nbody-simulation <config json file>"
                  << std::endl;
        return 1;
    }

    std::string version = get_version_from_file(".cz.toml");
    std::string title = "N-Body Simulation";
    if (!version.empty()) {
        title += " v" + version;
    }

    std::cout << title << std::endl;
    int choice = 0;
    std::cout << "[0] - Real time simulation" << std::endl
              << "[1] - Bake" << std::endl
              << "[2] - Render baked file" << std::endl
              << "> ";
    std::cin >> choice;

    App app{};
    app.set_title(title);
    app.set_window_size(800, 800);
    app.set_color(0x10, 0x10, 0x10);
    if (choice == 1)
        app.bake(argv[1]);
    else if (choice == 2)
        app.render_loop(argv[1]);
    else
        app.main_loop(argv[1]);
    return 0;
}
