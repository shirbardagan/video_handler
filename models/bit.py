from pydantic import BaseModel


class BitResponseModel(BaseModel):
    ip: str
    port: int
    nic: str
    connected_users: int
    status: bool
    iframe_interval: int
