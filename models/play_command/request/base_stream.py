from enum import Enum

from fastapi import HTTPException
from typing_extensions import Optional
from pydantic import Field, BaseModel, model_validator


class MulticastIn(BaseModel):
    ip: str = Field(description="Multicast IP")
    port: int = Field(description="Multicast port")
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
    RTSP = "rtsp"
    TEST = "test"
    RTP = "rtp"
    V4L2 = "v4l2"
    MPEG4I = "mpeg4i"
    MP2T = "mp2t"

    @classmethod
    def list(cls):
        """Return a list of all valid stream types."""
        return [stream.value for stream in cls]


class StreamSettings(BaseModel):
    bitrate: Optional[int] = 2048

    class Config:
        extra = "ignore"


class BaseStreamModel(BaseModel):
    command: str
    stream_type: str
    settings: StreamSettings = None
    multicast_in: MulticastIn = None

    class Config:
        extra = "ignore"

    @model_validator(mode="before")
    @classmethod
    def validate_settings(cls, values):
        stream_type = values.get("stream_type")
        settings = values.get("settings")

        if stream_type == StreamType.RTSP:
            if not settings or not settings.rtsp_settings:
                raise HTTPException(
                    status_code=422,
                    detail={
                        "status": "error",
                        "message": "When stream_type is 'rtsp', settings must have 'rtsp_settings' with rtsp path.",
                        "missing_field": "rtsp_settings",
                    }
                )
        return values
