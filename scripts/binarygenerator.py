from typing import List

from vec import Vec3


def gen_binary() -> List[dict]:
    config_dict: List[dict] = list()
    for i in range(2):
        obj = dict()
        pos = Vec3()
        vel = Vec3()

        print(f"Body {i}: type mass:")
        obj["mass"] = float(input("> "))

        print(f"Body {i}: type pos.x:")
        pos.x = float(input("> "))
        print(f"Body {i}: type pos.y:")
        pos.y = float(input("> "))
        print(f"Body {i}: type pos.z:")
        pos.z = float(input("> "))

        print(f"Body {i}: type velocity.x:")
        vel.x = float(input("> "))
        print(f"Body {i}: type velocity.y:")
        vel.y = float(input("> "))
        print(f"Body {i}: type velocity.z:")
        vel.z = float(input("> "))

        obj["pos"] = pos.to_dict()
        obj["velocity"] = vel.to_dict()
        config_dict.append(obj)

    return config_dict
