import numpy as np

class PyXPCSArray:
    def __init__(self, dims:tuple, compressed=True, compression_format="lil"):
        self.compressed = compressed
        self.format = compression_format
        self.rows, self.cols = dims
        self.locations = []
        self.pixles = []
        self.values = []
        self.indices = []
        self.lil = None
        self.coo = None
        self.dirty = False
    
    def append(self, location: int, indicies: np.ndarray, values: np.ndarray):
        self.locations.append(location)
        self.indices.append(indicies)
        self.values.append(values)
        self.dirty = True

    def get_lil(self):
        if not self.lil or self.dirty:
            self.__build_lil()
        
        return self.lil

    def get_coo(self):
        pass
    
    def sum(self, axis:int, roi:[] = None):
        assert(axis == 0 or axis == 1 )
        sums = None
        if axis == 0:
            # Summing up original frames
            sums =  np.zeros(len(self.locations))
            for idx, pos in enumerate(self.locations):
                sums[idx] = np.sum(self.values[idx])
        elif axis == 1:
            sums = np.zeros((self.rows*self.cols))
            for index_np, value_np in zip(self.indices, self.values):
                # print(index_np.shape, value_np.shape)
                sums[index_np] += value_np
            sums = np.reshape(sums, (self.rows, self.cols))
        return sums

    def from_numpy(self, np_array: np.ndarray, compression_format="lil"):
        pass

    def __build_lil(self):
        self.lil = [
            np.array(self.locations),
            self.indices,
            self.values
        ]