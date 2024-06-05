from typing import Dict


class Vec3:
    def __init__(self, x: float = 0.0, y: float = 0.0, z: float = 0.0) -> None:
        self.x = x
        self.y = y
        self.z = z

    def normalize(self) -> None:
        distance = (self.x ** 2 + self.y ** 2 + self.z ** 2) ** 0.5
        if distance != 0:
            self.x /= distance
            self.y /= distance
            self.z /= distance

    def to_dict(self) -> Dict[str, float]:
        return {"x": self.x, "y": self.y, "z": self.z}

    def __str__(self) -> str:
        return f"({self.x}, {self.y}, {self.z})"
