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

t0 = time.time()
reader = IMMReader8ID(IMM_FILE, no_of_frames=frames_to_process, skip_frames=config['frame_start_limit'])
indices, values = reader.data()
sd = libpx.call(indices, values, config['pixels'])
print(type(sd))
libpx.test(sd)

print(f"Reading IMM file took {time.time() - t0} seconds")
