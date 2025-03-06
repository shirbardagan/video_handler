from enum import Enum

from typing_extensions import Optional
from pydantic import Field, BaseModel, model_validator


class MulticastIn(BaseModel):
    ip: Optional[str] = Field(description="Multicast IP")
    port: int
    nic: Optional[str] = Field(default=None, description="Network interface")

    class Config:
        extra = "ignore"


class RTPConfig(BaseModel):
    enable: bool = Field(default=False, description="Enable RTP extension")
    datachannel: bool = Field(default=True, description="Enable datachannel for RTP")
    websocket: bool = Field(default=False, description="Enable websocket for RTP")

    class Config:
        extra = "ignore"


class StreamType(str, Enum):
    UDP = "udp"
    TEST = "test"
    RTSP = "rtsp"
    V4L2 = "v4l2"
    MPEG4I = "mpeg4i"
    MP2T = "mp2t"


class StreamSettings(BaseModel):
    bitrate: Optional[int] = 2048
    v4l2_srcDevice: Optional[str] = None
    rtsp_settings: Optional[str] = None

    class Config:
        extra = "ignore"

class BaseStreamModel(BaseModel):
    command: str
    stream_type: str
    settings: Optional[StreamSettings] = None
    multicast_in: Optional[MulticastIn] = None

    class Config:
        extra = "ignore"

    @classmethod
    @model_validator(mode="before")
    def validate_settings(cls, values):
        stream_type = values.get("stream_type")
        settings = values.get("settings")

        if stream_type == StreamType.RTSP:
            if not settings or not settings.rtsp_settings:
                raise ValueError("When stream_type is 'rtsp', settings must have 'rtsp_settings' with rtsp path.")
        return values

