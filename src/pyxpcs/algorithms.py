import libpyxpcs as libpx
from pyxpcs.structs import PyXPCSArray


def multitau(array: PyXPCSArray, config):
    lil = array.get_lil()
    res = libpx.multitau(lil.frames, lil.pixels, lil.values, **config)
    