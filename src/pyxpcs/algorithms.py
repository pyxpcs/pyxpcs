import libpyxpcs as libpx

def to_sparse_matrix(indices, values, config):
    return libpx.sparse_lil(indices, values, config['pixels'])

def multitau(sparse_matrix, config):
    _config = {
        'frames': config['frames'],
        'pixels': config['cols'] * config['rows'],
        'delays_per_level': config['delays_per_level'],
        'rows': config['rows'],
        'cols': config['cols'],  
    }

    return libpx.multitau(sparse_matrix, _config)

    