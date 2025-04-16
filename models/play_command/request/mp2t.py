from typing_extensions import Optional, Literal

from pydantic import Field, BaseModel, model_validator
from .base_stream import BaseStreamModel, StreamType


class KLVModel(BaseModel):
    enable: bool = Field(default=True, description="Enable/Disable KLV parsing")
    datachannel: bool = Field(default=True, description="Pass the data in WebRTC datachannel")
    websocket: bool = Field(default=False, description="Pass the data in websocket")
    parse: bool = Field(default=False, description="Parse KLV data or send raw data")

    class Config:
        extra = "ignore"


class MP2TStreamModel(BaseStreamModel):
    stream_type: Literal[StreamType.MP2T]
    klv: Optional[KLVModel] = Field(default=None, description="KLV pipeline settings")

    class Config:
        arbitrary_types_allowed = True
        extra = "ignore"


    @model_validator(mode="before")
    @classmethod
    def validate_mp2t_settings(cls, values):
        klv = values.get("klv")
        if not klv:
            values["klv"] = KLVModel(**{field: True for field in KLVModel.__fields__})
        return values