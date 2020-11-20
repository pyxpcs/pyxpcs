from pyxpcs.structs import PyXPCSArray
from pyxpcs.config import PyXPCSDict

import numpy as np
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm


def plot_pix_sum(sums: np.ndarray, config: PyXPCSDict):
    fig, ax = plt.subplots(1, 1, figsize=(15, 10))
    colormap = plt.cm.jet
    colormap.set_under(color='w')
    
    det_dist = config['detector_distance']
    ccd_x0 = config['beam_center_x']
    ccd_y0 = config['beam_center_y']
    pixel_size = config['pixel_size']
    x_energy = config['x_energy']

    pix2q = pixel_size/det_dist*(2*3.1416/(12.4/x_energy))
    y_min = ((0-ccd_x0)*pix2q).item()
    y_max = ((sums.shape[1]-ccd_x0)*pix2q).item()
    x_min = (0-ccd_y0)*pix2q.item()
    x_max = (sums.shape[0]-ccd_y0)*pix2q.item()

    im = ax.imshow(sums, 
                 cmap=colormap, 
                 norm=LogNorm(vmin=1e-6, vmax=3e-1),
                 interpolation='none', 
                 extent=([y_min, y_max, x_min, x_max]))
    fig.colorbar(im, ax=ax)
    plt.rc('font', size=20)

    return ax
