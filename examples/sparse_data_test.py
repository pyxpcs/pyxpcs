import pyxpcs as px
import libpyxpcs as libpx

sparse_data = libpx.SparseData(10, 10);
row = sparse_data.Pixel(0)
row.push(5, 10)
row.push(6, 12)

libpx.test(sparse_data)

print(len(sparse_data.ValidPixels()))