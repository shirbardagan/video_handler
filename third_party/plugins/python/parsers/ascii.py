from .base_parser import BaseParser
import logging

logger = logging.getLogger(__name__)
class AsciiParser(BaseParser):
    def __init__(self, key: str):
        self.key = key

    def parse(self, value: bytes) -> tuple[str, str]:
        try:
            return self.key, value.decode('utf-8', errors='replace')
        except Exception as e:
            logger.error("While parsing from ascii: %s", e)
            return self.key, ''