from pydantic_settings import BaseSettings, SettingsConfigDict


class CapsConfig(BaseSettings):
    model_config = SettingsConfigDict(env_file='.env', env_prefix='CAPS_', extra='ignore')
    h264: str
    rtp: str
