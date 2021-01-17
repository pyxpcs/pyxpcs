import numpy as np

from collections import defaultdict
from numpy.core.numeric import indices
from scipy import sparse

class PyXPCSArray:
    def __init__(self, dims:tuple, compressed=True, compression_format="lil"):
        self.compressed = compressed
        self.format = compression_format
        self.frames, self.rows, self.cols = dims
        self.data = sparse.lil_matrix((self.frames, self.rows*self.cols))
    
    def update(self, frame: int, indicies: np.ndarray, values: np.ndarray):
        self.data[frame, indices] = values

    def get_lil(self):
        return self.data

    def sum(self, axis:int):
        return np.sum(self.data, axis=axis)
        
    def from_numpy(self, np_array: np.ndarray, compression_format="lil"):
        pass

    def __build_lil(self):
        self.lil = LiL(
            np.array(self.frames),
            self.pixels,
            self.values
        )