import numpy as np

from collections import defaultdict
from numpy.core.numeric import indices
from scipy import sparse

class PyXPCSArray:
    def __init__(self, dims:tuple, compressed=True, compression_format="lil"):
        self.compressed = compressed
        self.format = compression_format
        self.frames, self.width, self.height = dims

        self.rows = [[] for _ in range(self.width*self.height)] 
        self.data = [[] for _ in range(self.width*self.height)] 
        self.valid_pixels = np.zeros(self.width*self.height)
        
    def add_frame(self, frame: int, indicies: np.ndarray, values: np.ndarray):
        for idx, val in zip(indicies, values):
            self.rows[idx].append(frame)
            self.data[idx].append(val)
            self.valid_pixels[idx] = 1
    
    def finalize(self):
        for i in range(len(self.rows)):
            self.rows[i] = np.array(self.rows[i])
            self.data[i] = np.array(self.data[i], dtype=np.float32)

    def get_lil(self):
        return [np.nonzero(self.valid_pixels)[0], self.rows, self.data]

    def pixel_sum(self):
        sums = np.zeros((self.width*self.height))
        for i in range(len(self.rows)):
            sums[i] = np.sum(self.data[i])
        
        return np.reshape(sums, (self.width, self.height))

    def frame_sum(self, axis:int):
        pass
        # sums = None
        # if axis == 0:
        #     # Summing up original frames
        #     sums =  np.zeros(len(self.frames))
        #     for idx, pos in enumerate(self.frames):
        #         sums[idx] = np.sum(self.values[idx])
        # elif axis == 1:
        #     sums = np.zeros((self.rows*self.cols))
        #     for index_np, value_np in zip(self.pixels, self.values):
        #         # print(index_np.shape, value_np.shape)
        #         sums[index_np] += value_np
        #     sums = np.reshape(sums, (self.rows, self.cols))
        # return sums
        
    def from_numpy(self, np_array: np.ndarray, compression_format="lil"):
        pass

    def __build_lil(self):
        self.lil = LiL(
            np.array(self.frames),
            self.pixels,
            self.values
        )