from .base_parser import BaseParser

class LinearScalarParser(BaseParser):
    def __init__(self, name, scale, offset=0, signed=False):
        self.name = name
        self.scale = scale
        self.offset = offset
        self.signed = signed

    def parse(self, buff):
        int_val = int.from_bytes(buff, byteorder='big', signed=self.signed)
        return self.name, self.scale * int_val + self.offset