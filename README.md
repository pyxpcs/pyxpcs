## Python XPCS Toolkit

### Dependencies

* CMake version 3+
* OpenMP
* HDF5

### Fetch the code

```
git clone --recurse-submodules https://github.com/pyxpcs/pyxpcs.git
```

### Install

To build a python package including the C/C++ extension. In the root directory:
```
python setup.py install
```

To build only the C/C++ code.

```
mkdir build
cd build
cmake ../
make -j
```

### Running

```
import pyxpcs as px
import numpy as np


config = {
    'frames': 200,
    'pixels': 100,
    'dpl': 4,
    'height': 10,
    'width': 10
}

pixels = np.arange(100)
times = np.meshgrid(np.arange(200), np.arange(100))[0]
values = np.random.rand(100, 200)

px.multitau(pixels, times, values, **config);
```




