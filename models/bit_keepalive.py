from enum import Enum

from pydantic import BaseModel
from typing_extensions import Optional

from models.play_command.request.mp2t import KLVModel


class GstStateEnum(str, Enum):
    NULL = "null"
    READY = "ready"
    PAUSED = "paused"
    PLAYING = "playing"
    STOPPED = "stopped"


class TranscodeSettings(BaseModel):
    ip: str
    port: int
    nic: str
    iframe_interval: int


class BitData(BaseModel):
    klv: Optional[KLVModel]
    transcode: TranscodeSettings


class BitResponseModel(BaseModel):
    status: bool
    connected_users: int
    state: Optional[GstStateEnum]
    bit: BitData


class BitKeepAliveCommandsModel(BaseModel):
    command: str