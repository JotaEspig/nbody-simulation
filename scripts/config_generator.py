import json

from binarygenerator import gen_binary
from galaxygenerator import gen_galaxy

BINARY = 0
GALAXY = 1
G = 6.67 * 10 ** -11


def save_config(config_dict: dict) -> None:
    print("type the filename:")
    filename = input("> ")
    with open(f"./config/auto_gen_{filename}.json", "w") as file:
        file.write(json.dumps(config_dict, indent=4))


def gen_file(type: int) -> None:
    print("Delta time multiplier (how many times will the time be accelerated): ")
    dt_multiplier = float(input("> "))

    config_dict = dict()
    config_dict["dt_multiplier"] = dt_multiplier
    if type == BINARY:
        config_dict["bodies"] = gen_binary()
    elif type == GALAXY:
        config_dict["bodies"] = gen_galaxy()
    else:
        print("Invalid celestial body system type!")
        return

    save_config(config_dict)


def main() -> None:
    print("Choose your celestial body system type:")
    print("0 - Binary system")
    print("1 - Galaxy")
    system_type = int(input("> "))

    gen_file(system_type)


if __name__ == "__main__":
    main()
