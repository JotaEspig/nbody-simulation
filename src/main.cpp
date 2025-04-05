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
    bool use_grav_grid = false;
    for (int i = 2; i < argc; ++i) {
        if (std::string(argv[i]) == "--simulate") {
            choice = 0;
        }
        else if (std::string(argv[i]) == "--bake") {
            choice = 1;
        }
        else if (std::string(argv[i]) == "--render") {
            choice = 2;
        }
        else if (std::string(argv[i]) == "--grav-grid") {
            use_grav_grid = true;
        }
        else if (std::string(argv[i]) == "--version") {
            std::cout << title << std::endl;
            return 0;
        }
        else {
            std::cout
                << "Usage: ./bin/nbody-simulation <config json file> [options]"
                << std::endl
                << "Options:" << std::endl
                << "  --version     Show version" << std::endl
                << "  --simulate    Simulate the system (Default)" << std::endl
                << "  --bake        Bake the simulation" << std::endl
                << "  --render      Render the simulation" << std::endl
                << "  --grav-grid   Use gravitational grid (Only works on "
                   "--simulate)"
                << std::endl
                << "  --help       Show this help message" << std::endl;
            return 0;
        }
    }

    App app{};
    app.set_title(title);
    app.set_window_size(800, 800);
    app.set_color(0x10, 0x10, 0x10);
    std::string json_path = std::string(argv[1]);
    if (choice == 1)
        app.bake(json_path.c_str());
    else if (choice == 2)
        app.render_loop(json_path.c_str());
    else
        app.main_loop(json_path.c_str(), use_grav_grid);
    return 0;
}
