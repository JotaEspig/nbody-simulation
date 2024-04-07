import json
from typing import List

BINARY = 0
GALAXY = 1


def save_config(config_dict: List[dict]) -> None:
    print("type the filename:")
    filename = input("> ")

    with open(f"./config/auto_gen_{filename}.json", "w") as file:
        file.write(json.dumps(config_dict, indent=4))


def gen_binary() -> List[dict]:
    config_dict: List[dict] = list()
    for i in range(2):
        obj = dict()
        pos = dict()
        vel = dict()

        print(f"Body {i}: type mass:")
        obj["mass"] = float(input("> "))

        print(f"Body {i}: type pos.x:")
        pos["x"] = float(input("> "))
        print(f"Body {i}: type pos.y:")
        pos["y"] = float(input("> "))
        print(f"Body {i}: type pos.z:")
        pos["z"] = float(input("> "))

        print(f"Body {i}: type velocity.x:")
        vel["x"] = float(input("> "))
        print(f"Body {i}: type velocity.y:")
        vel["y"] = float(input("> "))
        print(f"Body {i}: type velocity.z:")
        vel["z"] = float(input("> "))

        obj["pos"] = pos
        obj["velocity"] = vel
        config_dict.append(obj)

    return config_dict


def gen_galaxy() -> List[dict]:
    ...


def gen_file(type: int) -> None:
    config_dict: List[dict]
    if type == BINARY:
        config_dict = gen_binary()
    elif type == GALAXY:
        config_dict = gen_galaxy()
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
