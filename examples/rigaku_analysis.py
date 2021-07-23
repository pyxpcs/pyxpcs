import numpy as np

from pyxpcs.config import PyXPCSDict

from pyxpcs.utils import config_from_8idhdf5, load_results
from pyxpcs.viz import plot_pix_sum, plot_as_image
import libpyxpcs as libpx


Rigaku_FILE = "/home/faisal/Development/xpcs-eigen/data/rigaku/B873_Thermalgel20pct_att2_7C_Rq0_00005.bin"
HDF5_FILE = "/home/faisal/Development/xpcs-eigen/data/rigaku/B873_Thermalgel20pct_att2_7C_Rq0_00005_0001-100000.hdf"

config = config_from_8idhdf5(HDF5_FILE)

with open(Rigaku_FILE) as f:
    a = np.fromfile(f, dtype=np.uint64)
    
    b = (a >> 5+11)
    pix_ind = (b & 2**21-1).astype(int)
    pix_count = (a & 2**12-1).astype(int)
    pix_frame = (a >> 64-24).astype(int)

sparse_data = libpx.sparse_lil2(pix_ind, pix_frame, pix_count, np.max(pix_ind)+1)

mini_config = {
    'frames': 10000, #config['frames'],
    'pixels': config['cols'] * config['rows'],
    'delays_per_level': config['delays_per_level'],
    'rows': config['rows'],
    'cols': config['cols'],  
}

G2, IP, IF = libpx.multitau(sparse_data, mini_config)

print (G2.shape)