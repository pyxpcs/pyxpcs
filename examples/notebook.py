import numpy as np

from pyxpcs.reader import IMMReader8ID

from pyxpcs.structs import PyXPCSArray
from pyxpcs.config import PyXPCSDict
from pyxpcs.utils import config_from_8idhdf5
from pyxpcs.viz import plot_pix_sum
# from pyxpcs.algorithms import multitau
from pprint import pprint

import libpyxpcs as libpx

IMM_FILE = "/home/faisal/Development/xpcs-eigen/data/pyxpcs/A002_MJ_PMA_47g61kDa_att2_160C_Lq0_001_00001-00512.imm"

HDF5_FILE = "/home/faisal/Development/xpcs-eigen/data/pyxpcs/A002_MJ_PMA_47g61kDa_att2_160C_Lq0_001_0002-0512.hdf"

config = config_from_8idhdf5(HDF5_FILE)
# pprint(config)
frames_to_process = 25

reader = IMMReader8ID(IMM_FILE, no_of_frames=frames_to_process, skip_frames=config['frame_start_limit'])
data = reader.array()
data.finalize()

valid_pixels, rows, values = data.get_lil()

config = {
    'frames': frames_to_process,
    'pixels': config['cols'] * config['rows'],
    'delays_per_level': 4,
    'rows': config['rows'],
    'cols': config['cols'],  
}


ret = libpx.multitau(valid_pixels, rows, values, config)
print(type(ret))


