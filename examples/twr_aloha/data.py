import json
from dataclasses import asdict, dataclass
from typing import Dict, Optional, Union, Tuple, List
from dacite import from_dict
from riotctrl.shell import ShellInteraction
import math


@dataclass
class TWRData:
    t: int
    src: str
    dst: str
    d_cm: Optional[Union[int, float]]
    d_m: Optional[Union[int, float]]
    tof: Optional[float]
    los: Optional[float]
    rssi: Optional[int]

    def __post_init__(self):
        if self.d_cm:
            self.d_m = self.d_cm / 100.0
        elif self.d_m:
            self.d_cm = self.d_m * 100.0

    @staticmethod
    def from_json_str(json_string: str):
        json_dict = json.loads(json_string)
        return from_dict(data_class=TWRData, data=json_dict)

    def euid(self) -> str:
        eui = self.src
        return "{:0>4X}".format(((eui << 8) | (eui >> 8)) & 0xFFFF)


@dataclass
class ExperimentNode:
    network_address: str  # iot-lab format, example: dwm1001-5.saclay.iot-lab.info
    position: Union[List[float], Tuple[float, ...]]  # (x, y, z)
    hwaddr: Optional[str]
    shell: Optional[Union[ShellInteraction, str]]

    @property
    def node_id(self) -> str:
        # iot-lab node id, example: dwm1001-5
        return self.network_address.split(".")[0]

    def distance(self, neighbor) -> Union[float, int]:
        return math.sqrt(
            math.pow(self.position[0] - neighbor.position[0], 2)
            + math.pow(self.position[1] - neighbor.position[1], 2)
            + math.pow(self.position[2] - neighbor.position[2], 2)
        )

    def to_json_str(self, indent=None) -> str:
        json_dict = asdict(self)
        return json.dumps(json_dict, indent=indent)


@dataclass
class MeasurementData:
    distances: List[Union[float, int]]
    distance: Union[float, int]
    neighbor: str

    def to_json_str(self, indent=None) -> str:
        json_dict = asdict(self)
        return json.dumps(json_dict, indent=indent)

    def __lt__(self, other):
        return self.distance < other.distance


@dataclass
class NodeData:
    node: ExperimentNode
    data: List[MeasurementData]

    def to_json_str(self, indent=None) -> str:
        json_dict = asdict(self)
        return json.dumps(json_dict, indent=indent)


@dataclass
class ExperimentData:
    data: List[NodeData]

    def to_json_str(self, indent=None) -> str:
        json_dict = asdict(self)
        return json.dumps(json_dict, indent=indent)

    def sort(self):
        for data in self.data:
            data.data.sort()
        return self

    @staticmethod
    def from_json_str(json_string: str):
        json_dict = json.loads(json_string)
        return from_dict(data_class=ExperimentData, data=json_dict)
