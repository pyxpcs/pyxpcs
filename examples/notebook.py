import numpy as np
import time

from pyxpcs.reader import IMMReader8ID

from pyxpcs.structs import PyXPCSArray
from pyxpcs.config import PyXPCSDict
from pyxpcs.utils import config_from_8idhdf5
from pyxpcs.viz import plot_pix_sum
from pprint import pprint

import libpyxpcs as libpx

IMM_FILE = "/home/faisal/Development/xpcs-eigen/data/pyxpcs/A002_MJ_PMA_47g61kDa_att2_160C_Lq0_001_00001-00512.imm"
HDF5_FILE = "/home/faisal/Development/xpcs-eigen/data/pyxpcs/A002_MJ_PMA_47g61kDa_att2_160C_Lq0_001_0002-0512.hdf"

config = config_from_8idhdf5(HDF5_FILE)
pprint(config)
frames_to_process = config['frames']

t00 = time.time()
t0 = time.time()
reader = IMMReader8ID(IMM_FILE, no_of_frames=frames_to_process, skip_frames=config['frame_start_limit'])
indices, values = reader.data()
print(f"Reading IMM file took {time.time() - t0} seconds")

t0 = time.time()
sparse_data = libpx.sparse_lil(indices, values, config['pixels'])

# sparse_data = libpx.SparseData(config['pixels'], 10);
# found_rows = {}
# for fno in range(frames_to_process):
#   for idx, value in zip(indices[fno], values[fno]):
#     if idx in found_rows:
#       row = found_rows[idx]
#     else:
#       row = sparse_data.Pixel(idx)
#       found_rows[idx] = row

#     row.push(fno, value)
print(f"Constructing sparse matrix took {time.time() - t0} seconds")

config = {
    'frames': frames_to_process,
    'pixels': config['cols'] * config['rows'],
    'delays_per_level': 4,
    'rows': config['rows'],
    'cols': config['cols'],  
}


pixel_sum = np.zeros((config['cols'], config['rows']))
validPixels = sparse_data.ValidPixels()
for pixno in validPixels:
    row = sparse_data.Pixel(pixno)
    for val in row.valPtr:
        pixel_sum[pixno] += val

# t0 = time.time()
# libpx.multitau(sparse_data, config)
# print(f"Running multitau took {time.time() - t0} seconds")
# print(f"Total = {time.time() - t00} seconds")
