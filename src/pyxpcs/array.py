import numpy as np

class Array:
    def __init__(self, compressed=True, compression_format="lil"):
        self.compressed = compressed
        self.format = compression_format
        self.positions = []
        self.pixles = []
        self.values = []
    
    def append(self, pos: int, values: np.ndarray, index: np.ndarray):
        self.positions.append(pos)
        self.values.append(values)
        self.index.append(index)

    def get_lil(self):
        pass

    def get_coo(self):
        pass
    
    def from_numpy(self, np_array: np.ndarray, compression_format="lil"):
        pass