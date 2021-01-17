import libpyxpcs as libpx
from scipy.sparse import lil_matrix

def multitau(array: lil_matrix, config):
    res = libpx.multitau(lil_matrix.rows, lil_matrix.cols, config)
    pass
    