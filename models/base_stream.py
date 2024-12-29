from pydantic_settings import BaseSettings
from typing import Optional, Union, List, Dict
from pydantic import Field


class MulticastIn(BaseSettings):
    ip: Optional[str] = Field(default="127.0.0.1", description="Multicast IP")
    port: int
    nic: Optional[str] = Field(default=None, description="Network interface")


class RTPConfig(BaseSettings):
    enable: bool = Field(default=False, description="Enable RTP extension")
    datachannel: bool = Field(default=True, description="Enable datachannel for RTP")
    websocket: bool = Field(default=False, description="Enable websocket for RTP")


class BaseStreamModel(BaseSettings):
    command: str = "play"
    stream_type: str
    is_rtp: Optional[bool] = False
    encoding: Optional[str]
    uri: Optional[str] = Field(default=None, description="Stream URI")
    multicast_in: Optional[MulticastIn] = None
    dedupe: Optional[bool] = False
    rtp_ext: Optional[RTPConfig] = None
