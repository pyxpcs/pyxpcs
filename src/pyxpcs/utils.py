import h5py
import numpy as np
from IPython import embed


from pyxpcs.config import PyXPCSDict

def config_from_8idhdf5(file_name, entry_group="/xpcs"):
    f = h5py.File(file_name, 'r')
    paths = f[entry_group].keys()
    print(paths)

    pxdict = PyXPCSDict()
    pxdict['rows'] = f['/measurement/instrument/detector/x_dimension'][0][0]
    pxdict['cols'] = f['/measurement/instrument/detector/y_dimension'][0][0]
    pxdict['frame_start'] = f[f"{entry_group}/data_begin"][0][0]
    pxdict['frame_end'] = f[f"{entry_group}/data_end"][0][0]
    pxdict['frame_start_limit'] = f[f"{entry_group}/data_begin_todo"][0][0]
    pxdict['frame_end_limit'] = f[f"{entry_group}/data_end_todo"][0][0]
    pxdict['delays_per_level'] = f[f"{entry_group}/delays_per_level"][0][0]
    pxdict['dqmap'] = np.squeeze(f[f"{entry_group}/dqmap"][:])
    pxdict['sqmap'] = np.squeeze(f[f"{entry_group}/sqmap"][:])
    pxdict['roi_1d'] = build_roi_1d(pxdict['dqmap'], pxdict['sqmap'])
    # pxdict['roi_2d'] = build_roi_2d(dqmap, sqmap)
    pxdict['beam_center_x'] = f['/measurement/instrument/acquisition/beam_center_x'][0][0]
    pxdict['beam_center_y'] = f['/measurement/instrument/acquisition/beam_center_y'][0][0]
    pxdict['detector_distance'] = f['/measurement/instrument/detector/distance'][0][0]
    pxdict['x_energy'] = f['/measurement/instrument/source_begin/energy'][0][0]
    pxdict['pixel_size'] = f['/measurement/instrument/detector/x_pixel_size'][0][0]

    f.close()

    return pxdict

def build_roi_1d(dqmap, sqmap):
    mask = []
    dq_nonzeros = np.nonzero(dqmap)[1]
    sq_nonzeros = np.nonzero(sqmap)[1]

    mask = dq_nonzeros + sq_nonzeros

    return mask

def buld_roi_2d(dqmap, sqmap):
    pass

if __name__ == '__main__':
    file = "/home/faisal/Development/xpcs-eigen/data/pyxpcs/A002_MJ_PMA_47g61kDa_att2_160C_Lq0_001_0002-0512.hdf"
    pxdict = config_from_8idhdf5(file)
    print(pxdict)
    
    embed()


