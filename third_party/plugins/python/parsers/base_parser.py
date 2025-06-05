from abc import ABC, abstractmethod

class BaseParser(ABC):
    @abstractmethod
    def parse(self, buff: bytes) -> tuple[str, float | str]:
        pass

def decode_ascii(self, buff: bytes) -> str:
    return buff.decode('ascii', errors='replace')