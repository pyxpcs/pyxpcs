import numpy as np

from pyxpcs.reader import IMMReader8ID

from pyxpcs.structs import PyXPCSArray
from pyxpcs.config import PyXPCSDict
from pyxpcs.utils import config_from_8idhdf5
from pyxpcs.viz import plot_pix_sum
# from pyxpcs.algorithms import multitau
from pprint import pprint

# import libpyxpcs as libpx

# IMM_FILE = "/home/faisal/Development/xpcs-eigen/data/pyxpcs/A002_MJ_PMA_47g61kDa_att2_160C_Lq0_001_00001-00512.imm"

# HDF5_FILE = "/home/faisal/Development/xpcs-eigen/data/pyxpcs/A002_MJ_PMA_47g61kDa_att2_160C_Lq0_001_0002-0512.hdf"

# config = config_from_8idhdf5(HDF5_FILE)
# # pprint(config)
# frames_to_process = 25

# reader = IMMReader8ID(IMM_FILE, no_of_frames=frames_to_process, skip_frames=config['frame_start_limit'])
# data = reader.array()
# data.finalize()

# valid_pixels, rows, values = data.get_lil()

# print(type(valid_pixels))
# print(type(rows))
# print(type(values))

# print(valid_pixels.shape)
# print(rows.shape)
# print(values.shape)

# config = {
#     'frames': frames_to_process,
#     'pixels': config['cols'] * config['rows'],
#     'delays_per_level': 4,
#     'rows': config['rows'],
#     'cols': config['cols'],  
# }


# ret = libpx.multitau(valid_pixels, rows, values, config)
# print(type(ret))
# print(ret.shape)

# print(ret[0, 452]) # 0.041667, 


# print(config['rows'], config['cols'])
# # xs = np.arange(10).astype("double")
# print(rows[5])
# libpx.twice(valid_pixels, rows)

import libpyxpcs as libpx

row = libpx.Row()
row.indxPtr.push_back(5)
row.indxPtr.push_back(7)

libpx.test(row)

