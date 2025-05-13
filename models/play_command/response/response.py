from typing_extensions import Optional
from pydantic import BaseModel, Field, ValidationError

from models.play_command.request.mp2t import KLVModel


class PlayResponseModel(BaseModel):
    status: bool
    ws_port: int = 8080
    host_ip: str
    endpoint: str
    klv: Optional[KLVModel] = None

    class Config:
        extra = "ignore"


try:
    PlayResponseModel(ws_port=None)
except ValidationError as exc:
    print(repr(exc.errors()[0]['type']))
