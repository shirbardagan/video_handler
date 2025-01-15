from typing_extensions import Optional, Union

from models.base_stream import BaseStreamModel
from pydantic import Field, root_validator

reinterlace_options = [None, False, True, 1, 0, 2, "reinterlace", "crop"]


class MPEG4IStreamConfig(BaseStreamModel):
    stream_type: str = "mpeg4i"
    reinterlace: Optional[Union[bool, int, str]] = Field(default=0, description="Reinterlace option")

# @root_validator(pre=True)
# def validate_reinterlace(values):
#     reinterlace = values.get("reinterlace")
#     reinterlace_options = [None, False, True, 1, 0, 2, "reinterlace", "crop"]
#     if reinterlace not in reinterlace_options:
#         raise ValueError(f"Invalid value for reinterlace_option. Allowed values are {reinterlace_options}.")
#     return values
