from typing import Optional

from pydantic import Field
from pydantic_settings import BaseSettings
from models.base_stream import BaseStreamModel


class KLVModel(BaseSettings):
    enable: bool = Field(default=True)
    datachannel: bool = Field(default=True)
    websocket: bool = Field(default=False)
    parse: bool = Field(default=False)

class MP2TStreamModel(BaseStreamModel):
    stream_type: str = "mp2t"
    is_rtp: Optional[bool] = Field(default=False, description="Is RTP enabled")
    klv: Optional[KLVModel] = None