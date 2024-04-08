import json
from typing import List
from random import random

BINARY = 0
GALAXY = 1
G = 6.67 * 10 ** -11


def save_config(config_dict: List[dict]) -> None:
    print("type the filename:")
    filename = input("> ")
    with open(f"./config/auto_gen_{filename}.json", "w") as file:
        file.write(json.dumps(config_dict, indent=4))


def orbital_speed(mass: float, distance: float) -> float:
    return (G * mass * (1/distance)) ** 0.5


def normalize(coord: float, distance: float) -> float:
    return coord / distance


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
    config_dict: List[dict] = list()
    print("How many galaxies?")
    galaxies_amount = int(input("> "))
    for i in range(galaxies_amount):
        print(f"Galaxy number {i} pos.x:")
        pos_x = int(input("> "))
        print(f"Galaxy number {i} pos.y:")
        pos_y = int(input("> "))
        print(f"Galaxy number {i} pos.z:")
        pos_z = int(input("> "))

        possible_answers = ["x", "y", "z"]
        galaxy_axis = ""
        while galaxy_axis not in possible_answers:
            print(f"Which axis should the galaxy number {i} be pointed to? [x/y/z]")
            galaxy_axis = str(input("> ").strip().lower())

        print(f"Galaxy number {i} central massive body mass:")
        central_massive_body_mass = float(input("> "))
        print(f"Galaxy number {i} velocity.x:")
        velocity_x = int(input("> "))
        print(f"Galaxy number {i} velocity.y:")
        velocity_y = int(input("> "))
        print(f"Galaxy number {i} velocity.z:")
        velocity_z = int(input("> "))

        obj = dict()
        obj["mass"] = central_massive_body_mass
        obj["pos"] = {"x": pos_x, "y": pos_y, "z": pos_z}
        obj["velocity"] = {"x": velocity_x, "y": velocity_y, "z": velocity_z}
        config_dict.append(obj)

        print(f"galaxy number {i} max orbital objects radius")
        radius = int(input("> "))
        print(f"How many bodies will orbit galaxy number {i}?")
        amount = int(input("> "))
        same_mass = False
        common_mass = 0
        if amount > 0:
            print(f"These bodies will have the same mass?")
            same_mass = bool(input("> "))

        obj_count = 1
        counter = 0
        step = radius / amount
        while counter < radius:
            offset = step * obj_count
            pos_x_offset = pos_x + offset * (1 if random() < 0.5 else -1)
            pos_y_offset = pos_y + offset * (1 if random() < 0.5 else -1)
            pos_z_offset = pos_z + offset * (1 if random() < 0.5 else -1)
            if galaxy_axis == "x":
                pos_x_offset = pos_x
            elif galaxy_axis == "y":
                pos_y_offset = pos_y
            else:
                pos_z_offset = pos_z

            orbit_obj_mass = 0
            if same_mass and common_mass != 0:
                orbit_obj_mass = common_mass
            elif same_mass and common_mass == 0:
                print("Object mass")
                common_mass = float(input("> "))
            elif not same_mass:
                print(f"Object {obj_count} mass")
                orbit_obj_mass = float(input("> "))

            obj = dict()
            obj["mass"] = orbit_obj_mass
            obj["pos"] = {"x": pos_x_offset, "y": pos_y_offset, "z": pos_z_offset}

            vec_x = pos_x - pos_x_offset
            vec_y = pos_y - pos_y_offset
            vec_z = pos_z - pos_z_offset
            distance = (vec_x ** 2 + vec_y ** 2 + vec_z ** 2) ** 0.5
            obj_orbital_speed = orbital_speed(central_massive_body_mass, distance)
            print(obj_orbital_speed)
            if vec_x == 0:
                aux = vec_y
                vec_y = vec_z
                vec_z = -aux
            if vec_y == 0:
                aux = vec_x
                vec_x = vec_z
                vec_z = -aux
            else:
                aux = vec_x
                vec_x = vec_y
                vec_y = -aux

            vec_x = normalize(vec_x, distance)
            vec_y = normalize(vec_y, distance)
            vec_z = normalize(vec_z, distance)
            obj["velocity"] = {"x": vec_x * obj_orbital_speed, "y": vec_y * obj_orbital_speed, "z": vec_z * obj_orbital_speed}

            config_dict.append(obj)
            counter += step
            obj_count += 1

    return config_dict


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
