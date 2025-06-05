from .base_parser import BaseParser

class RvtParser(BaseParser):
    def __init__(self, tag: int):
        self.tag = tag

    def parse(self, value: bytes) -> tuple[str, any]:
        if self.tag == 73:
            return self._parse_rvt_local_set(value)
        elif self.tag == 74:
            return self._parse_rvt_vmti(value)
        else:
            raise ValueError(f"Unsupported RVT tag: {self.tag}")

    def _parse_rvt_local_set(self, buff: bytes) -> tuple[str, any]:
        pass

    def _parse_rvt_vmti(self, buff: bytes) -> tuple[str, any]:
        pass