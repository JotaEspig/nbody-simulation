#include <iostream>

#include <app.hpp>

int main(int argc, char **argv)
{
    std::cout << "Solar System 3D" << std::endl;
    App app{};
    app.set_title("Uepa");
    app.set_width(600);
    app.set_height(600);
    app.set_color(0x10, 0x10, 0x10);
    if (argc < 2)
    {
        std::cout << "Invalid command" << std::endl
                  << "Usage: ./bin/nbody-simulation config.json" << std::endl;
    }
    else
    {
        app.main_loop(argv[1]);
    }
    return 0;
}
