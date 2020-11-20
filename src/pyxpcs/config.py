import h5py
from collections.abc import MutableMapping
import numpy as np

class PyXPCSDict(MutableMapping):
    def __init__(self, configs={}):
        self.mapping = {}
        self.update(configs)
    
    def __getitem__(self, key):
        return self.mapping[key]
    
    def __delitem__(self, key):
        del self.mapping[key]
        
    def __setitem__(self, key, value):
        if key in self:
            del self[self[key]]
        self.mapping[key] = value
        
    def __iter__(self):
        return iter(self.mapping)

    def __len__(self):
        return len(self.mapping)

    def __repr__(self):
        return f"{type(self).__name__}({self.mapping})"

if __name__ == '__main__':
    c = ConfigDict()
    c['rows'] = 30

    from pprint import pprint
    pprint(c)