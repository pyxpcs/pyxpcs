import libpyxpcs as libpx
from pyxpcs.structs import PyXPCSArray


def multitau(array: PyXPCSArray):
    lil = array.get_lil()
    res = libpx.multitau(lil.frames, lil.indices, lil.values)
    print(res)