import pyxpcs as px
import numpy as np
import libpyxpcs as libpx


config = {
    'frames': 10,
    'pixels': 5,
    'delays_per_level': 4,
    'rows': 3,
    'cols': 3,
    'pixels': 9,
}

frames = np.arange(config['frames'])
pixels = [ np.arange(config['pixels']) for _ in range(config['frames'])]
values = [ np.random.rand(config['pixels']) for _ in range(config['frames'])]

libpx.multitau(frames, pixels, values, config)

# libpx.multitau2(3, frames)

# a = []
# for i in range(10):
#     a.append(
#         (np.meshgrid(
#             np.arange(np.random.randint(20)),
#             np.arange(np.random.randint(10))
#         )[0])
#     )

# # px.call(a);
# a = px.callback(5, 5, 5);
# print(a.shape)
# print(a)

# from pyxpcs.reader import IMMReader8ID
# from pyxpcs.structs import PyXPCSArray
# from pyxpcs.config import PyXPCSDict
# from pyxpcs.utils import config_from_8idhdf5
# from pyxpcs.viz import plot_pix_sum
# from pyxpcs.algorithms import multitau

# import libpyxpcs as libpx

# from pprint import pprint

# IMM_FILE = "/home/faisal/Development/xpcs-eigen/data/pyxpcs/A002_MJ_PMA_47g61kDa_att2_160C_Lq0_001_00001-00512.imm"
# HDF5_FILE = "/home/faisal/Development/xpcs-eigen/data/pyxpcs/A002_MJ_PMA_47g61kDa_att2_160C_Lq0_001_0002-0512.hdf"

# reader = IMMReader8ID(IMM_FILE)
# reader.load()

# config = config_from_8idhdf5(HDF5_FILE)

# data = reader.array
# pixel_sum = data.sum(axis=1)
# pixel_sum.shape

# # multitau(data, config)
# ret = libpx.callback(2, 3, 4);
# print(ret);
