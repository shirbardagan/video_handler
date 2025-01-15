from typing_extensions import Optional
from pydantic import Field, BaseModel


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


class BaseStreamModel(BaseModel):
    command: str
    stream_type: str
    is_rtp: Optional[bool] = False
    encoding: Optional[str] = None
    uri: Optional[str] = Field(default=None, description="Stream URI")
    multicast_in: Optional[MulticastIn] = None
    dedupe: Optional[bool] = False
    rtp_ext: Optional[RTPConfig] = None

    class Config:
        extra = "ignore"
