import numpy as np
import h5py

from pyxpcs.structs import PyXPCSArray
from pyxpcs.config import PyXPCSDict
from pyxpcs.utils import config_from_8idhdf5, load_results
from pyxpcs.viz import plot_pix_sum, plot_as_image

import matplotlib.pyplot as plt

import libpyxpcs as libpx

from pprint import pprint

HDF5_FILE = "/home/faisal/Development/xpcs-eigen/data/hdf5_test/A003_Cu3Au_att0_001_multitau.hdf"
HDF5_DATA_FILE = "/home/faisal/Development/xpcs-eigen/data/hdf5_test/A003_Cu3Au_att0_001.h5"


config = config_from_8idhdf5(HDF5_FILE)

f = h5py.File(HDF5_DATA_FILE, 'r')
paths = f["/"].keys()


data = f['/entry/data/data'][:]


libpx.sparse_lil_value(data)


# a = np.zeros((1000,500, 500))
# b = np.ones((1000,500, 500)) * 3 

# c = libpx.add_arrays(a, b)
# print(c.shape)