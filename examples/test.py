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

a = []
for i in range(10):
    a.append(
        (np.meshgrid(
            np.arange(np.random.randint(20)),
            np.arange(np.random.randint(10))
        )[0])
    )

# px.call(a);
a = px.callback(5, 5, 5);
print(a.shape)
print(a)


