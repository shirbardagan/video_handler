from typing_extensions import Optional
from pydantic import BaseModel

from models.play_command.request.mp2t import KLVModel


class PlayResponseModel(BaseModel):
    status: bool
    ws_port: int
    host_ip: str
    endpoint: str
    active_ws_port: int
    klv: Optional[KLVModel]

    class Config:
        extra = "ignore"