from enum import Enum

from pydantic import BaseModel
from typing_extensions import Optional, Literal, Union

from models.commands import CommandsEnum
from models.play_command.request.mp2t import KLVModel


class GstStateEnum(str, Enum):
    NULL = "null"
    READY = "ready"
    PAUSED = "paused"
    PLAYING = "playing"
    STOPPED = "stopped"
    IDLE = "idle"


class TranscodeSettings(BaseModel):
    ip: str
    port: int
    nic: Optional[Union[str, None]]
    iframe_interval: int


class BitData(BaseModel):
    klv: Optional[KLVModel]
    transcode: TranscodeSettings


class BitResponseModel(BaseModel):
    status: bool
    connected_users: int
    state: Optional[GstStateEnum]
    bit: BitData


class BitCommandModel(BaseModel):
    """Model for command 'bit'."""
    command: Literal[CommandsEnum.BIT]


class KeepaliveCommandModel(BaseModel):
    """Model for command 'keepalive'."""
    command: Literal[CommandsEnum.KEEPALIVE]


class VersionCommandModel(BaseModel):
    """Model for command 'version'."""
    command: Literal[CommandsEnum.VERSION]


class StopCommandModel(BaseModel):
    """Model for command 'stop'."""
    command: Literal[CommandsEnum.STOP]


class Liveness(BaseModel):
    """Model for command 'stop'."""
    liveness : bool