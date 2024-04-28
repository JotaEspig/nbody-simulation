#include <iostream>

#include "app.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Invalid command" << std::endl
                  << "Usage: ./bin/nbody-simulation <config json file>"
                  << std::endl;
        return 1;
    }

    std::cout << "N-Body Simulation" << std::endl;
    int choice = 0;
    std::cout << "[0] - Real time simulation" << std::endl
              << "[1] - Bake" << std::endl
              << "[2] - Render baked file" << std::endl
              << "> ";
    std::cin >> choice;

    App app{};
    app.set_title("Uepa");
    app.set_width(600);
    app.set_height(600);
    app.set_color(0x10, 0x10, 0x10);
    if (choice == 1)
        app.bake(argv[1]);
    else if (choice == 2)
        app.render_loop(argv[1]);
    else
        app.main_loop(argv[1]);
    return 0;
}
