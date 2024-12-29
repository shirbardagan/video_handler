from typing import Optional, Union

from models.base_stream import BaseStreamModel
from pydantic import Field, model_validator

reinterlace_options = [None, False, True, 1, 0, 2, "reinterlace", "crop"]


class MPEG4IStreamConfig(BaseStreamModel):
    stream_type: str = "mpeg4i"
    reinterlace: Optional[Union[bool, int, str]] = Field(default=0, description="Reinterlace option")

    @model_validator(mode='before')
    def validate_reinterlace(cls, values):
        value = values.get("reinterlace")
        if value not in reinterlace_options:
            raise ValueError(f"Invalid value for reinterlace_option. Allowed values are {reinterlace_options}.")
        return values