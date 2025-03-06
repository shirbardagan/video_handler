from typing_extensions import Optional

from pydantic import Field, BaseModel
from .base_stream import BaseStreamModel


class KLVModel(BaseModel):
    enable: bool = Field(default=True)
    datachannel: bool = Field(default=True)
    websocket: bool = Field(default=False)
    parse: bool = Field(default=False)

    class Config:
        extra = "ignore"


class MP2TStreamModel(BaseStreamModel):
    stream_type: str = "mp2t"
    klv: Optional[KLVModel] = None

    class Config:
        arbitrary_types_allowed = True
        extra = "ignore"
