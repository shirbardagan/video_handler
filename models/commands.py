from enum import Enum

class CommandsEnum(str, Enum):
    BIT = "bit"
    PLAY = "play"
    KEEPALIVE = "keepalive"
    VERSION = "version"
    STOP = "stop"