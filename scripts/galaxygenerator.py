import math
from typing import List
from random import random

from vec import Vec3


G = 6.67 * 10 ** -11


def normalize(coord: float, distance: float) -> float:
    return coord / distance


def orbital_speed(mass: float, distance: float) -> float:
    return (G * mass * (1/distance)) ** 0.5


def gen_galaxy() -> List[dict]:
    config_dict: List[dict] = list()
    print("How many galaxies?")
    galaxies_amount = int(input("> "))
    for i in range(galaxies_amount):
        pos = Vec3()
        print(f"Galaxy number {i} pos.x:")
        pos.x = int(input("> "))
        print(f"Galaxy number {i} pos.y:")
        pos.y = int(input("> "))
        print(f"Galaxy number {i} pos.z:")
        pos.z = int(input("> "))

        possible_answers = ["x", "y", "z"]
        galaxy_axis = ""
        while galaxy_axis not in possible_answers:
            print(f"Which axis should the galaxy number {
                  i} be pointed to? [x/y/z]")
            galaxy_axis = str(input("> ").strip().lower())

        print(f"Galaxy number {i} central massive body mass:")
        central_massive_body_mass = float(input("> "))

        velocity = Vec3()
        print(f"Galaxy number {i} velocity.x:")
        velocity.x = float(input("> "))
        print(f"Galaxy number {i} velocity.y:")
        velocity.y = float(input("> "))
        print(f"Galaxy number {i} velocity.z:")
        velocity.z = float(input("> "))

        obj = dict()
        obj["mass"] = central_massive_body_mass
        obj["pos"] = pos.to_dict()
        obj["velocity"] = velocity.to_dict()
        config_dict.append(obj)

        print(f"galaxy number {i} max orbital objects radius")
        radius = int(input("> "))
        print(f"How many layers of bodies will the galaxy number {i} have?")
        layers_amount = int(input("> "))
        print(f"How many bodies per layer?")
        bodies_per_layer = int(input("> "))

        same_mass = False
        aligned_bodies = True
        common_mass = 0
        if layers_amount > 0:
            print(f"These bodies will have the same mass? [Y/n]")
            same_mass_str = input("> ").strip().lower()
            same_mass = same_mass_str == "y" or same_mass_str == ""
            print(
                f"These bodies will be aligned at the start of the simulation? [Y/n]")
            aligned_bodies_str = input("> ").strip().lower()
            aligned_bodies = aligned_bodies_str == "y" or aligned_bodies_str == ""

        obj_count = 1
        counter = 0
        step = radius / layers_amount
        while counter < radius:
            offset = step * obj_count
            for i in range(bodies_per_layer):
                pos_offset = Vec3()
                if not aligned_bodies:
                    pos_offset.x = pos.x + offset * math.sin(random()) * \
                        (1 if random() < 0.5 else -1)
                    pos_offset.y = pos.y + offset * math.sin(random()) * \
                        (1 if random() < 0.5 else -1)
                    pos_offset.z = pos.z + offset * math.sin(random()) * \
                        (1 if random() < 0.5 else -1)
                else:
                    pos_offset.x = pos.x + offset * \
                        (1 if random() < 0.5 else -1)
                    pos_offset.y = pos.y + offset * \
                        (1 if random() < 0.5 else -1)
                    pos_offset.z = pos.z + offset * \
                        (1 if random() < 0.5 else -1)

                if galaxy_axis == "x":
                    pos_offset.x = pos.x
                elif galaxy_axis == "y":
                    pos_offset.y = pos.y
                else:
                    pos_offset.z = pos.z

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
                obj["pos"] = pos_offset.to_dict()

                obj_vel = Vec3()
                obj_vel.x = pos.x - pos_offset.x
                obj_vel.y = pos.y - pos_offset.y
                obj_vel.z = pos.z - pos_offset.z
                distance = (obj_vel.x ** 2 + obj_vel.y **
                            2 + obj_vel.z ** 2) ** 0.5
                obj_orbital_speed = orbital_speed(
                    central_massive_body_mass, distance)
                if obj_vel.x == 0:
                    aux = obj_vel.y
                    obj_vel.y = obj_vel.z
                    obj_vel.z = -aux
                elif obj_vel.y == 0:
                    aux = obj_vel.x
                    obj_vel.x = obj_vel.z
                    obj_vel.z = -aux
                else:
                    aux = obj_vel.x
                    obj_vel.x = obj_vel.y
                    obj_vel.y = -aux

                obj_vel.normalize()
                final_obj_vel = Vec3(obj_vel.x * obj_orbital_speed + velocity.x,
                                     obj_vel.y * obj_orbital_speed + velocity.y,
                                     obj_vel.z * obj_orbital_speed + velocity.z)
                obj["velocity"] = final_obj_vel.to_dict()

                config_dict.append(obj)

            counter += step
            obj_count += 1

    return config_dict
