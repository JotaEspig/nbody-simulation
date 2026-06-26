#include <fstream>
#include <iostream>
#include <regex>

#include "app.hpp"

enum class Mode { Simulate, Bake, Render, Benchmark };

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
        std::cout << "Usage: ./bin/nbody-simulation <config.json> [options]\n";
        return 1;
    }

    std::string version
        = get_version_from_file(std::string(PROJECT_DIR) + "/.cz.toml");

    std::string title = "N-Body Simulation";
    if (!version.empty()) {
        title += " v" + version;
    }

    std::cout << title << std::endl;

    Mode mode = Mode::Simulate;
    bool use_grav_grid = false;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--simulate") {
            mode = Mode::Simulate;
        }
        else if (arg == "--bake") {
            mode = Mode::Bake;
        }
        else if (arg == "--render") {
            mode = Mode::Render;
        }
        else if (arg == "--benchmark") {
            mode = Mode::Benchmark;
        }
        else if (arg == "--grav-grid") {
            use_grav_grid = true;
        }
        else if (arg == "--version") {
            std::cout << title << std::endl;
            return 0;
        }
        else if (arg == "--help") {
            std::cout
                << "Usage: ./bin/nbody-simulation <config.json> [options]\n\n"
                << "Options:\n"
                << "  --simulate     Run simulation (default)\n"
                << "  --bake         Bake the simulation\n"
                << "  --render       Render baked simulation\n"
                << "  --benchmark    Benchmark all simulation algorithms\n"
                << "  --grav-grid    Enable gravitational grid (simulation "
                   "only)\n"
                << "  --version      Show version\n"
                << "  --help         Show this help message\n";
            return 0;
        }
        else {
            std::cerr << "Unknown option: " << arg << '\n';
            std::cerr << "Use --help for usage information.\n";
            return 1;
        }
    }

    App app{};
    app.set_title(title);
    app.set_window_size(800, 800);
    app.set_color(0x10, 0x10, 0x10);

    const std::string json_path = argv[1];

    switch (mode) {
    case Mode::Bake:
        app.bake(json_path.c_str());
        break;

    case Mode::Render:
        app.render_loop(json_path.c_str());
        break;

    case Mode::Benchmark:
        app.benchmark(json_path.c_str());
        break;

    case Mode::Simulate:
    default:
        app.main_loop(json_path.c_str(), use_grav_grid);
        break;
    }

    return 0;
}
