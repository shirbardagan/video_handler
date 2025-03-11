from typing_extensions import Union
from pydantic import Field, field_validator
from pydantic_settings import BaseSettings, SettingsConfigDict


class SystemSettingsConfig(BaseSettings):
    model_config = SettingsConfigDict(env_file='.env', env_prefix='SYSTEM_', extra='ignore')
    vh_max_users: Union[int,str] = Field(-1, description="Maximum number of users allowed")
    default_value: int = -1

    @field_validator("vh_max_users", mode="before")
    @classmethod
    def parse_vh_max_users(cls, value):
        if value is None or not value:
            return -1
        return int(value)