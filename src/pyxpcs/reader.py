import struct
import numpy as np

from .array import Array

class Reader:
    """
    Base class for File readers
    """
    def read(self, nframes: int):
        pass

    def specs(self):
        pass

    def array(self):
        pass

class IMMReader8ID(Reader):
    def __init__(self, filename, **kwargs):
        self.filename = filename
        self.array = Array()

    def read(self, nframes=-1):
         with open(filename, "rb") as file:
            header = self.__read_imm_header()
            self.rows, self.cols = header['rows'], header['cols']
            self.is_compressed = bool(header['compression'] == 6)
            default_indices = list(range(0, self.rows * self.cols))
            num_pixels = header['dlen']
            payload_size = num_pixels * (6 if self.is_compressed else 2)
            pos = 0

            while True:
                try:
                    frame = None
                    if self.is_compressed:
                        indexes = np.fromfile(file, dtype=np.uint32, count=num_pixels)
                        values = np.fromfile(file, dtype=np.uint16, count=num_pixels)
                        self.array.append(pos, values, indexes)
                    else:
                        values = np.fromfile(file, dtype=np.uint16, count=num_pixels)
                        self.array.append(pos, values, default_indices)

                    header = self.__read_imm_header(file)
                    # Check for end of file.
                    if not file.peek(4):
                        break
                except Exception as err:
                    raise IOError("IMM file doesn't seems to be of right type") from err
    def array(self):
        return self.array

    def __read_imm_header(self, file):
        imm_headformat = "ii32s16si16siiiiiiiiiiiiiddiiIiiI40sf40sf40sf40sf40sf40sf40sf40sf40sf40sfffiiifc295s84s12s"
        imm_fieldnames = [
            'mode',
            'compression',
            'date',
            'prefix',
            'number',
            'suffix',
            'monitor',
            'shutter',
            'row_beg',
            'row_end',
            'col_beg',
            'col_end',
            'row_bin',
            'col_bin',
            'rows',
            'cols',
            'bytes',
            'kinetics',
            'kinwinsize',
            'elapsed',
            'preset',
            'topup',
            'inject',
            'dlen',
            'roi_number',
            'buffer_number',
            'systick',
            'pv1',
            'pv1VAL',
            'pv2',
            'pv2VAL',
            'pv3',
            'pv3VAL',
            'pv4',
            'pv4VAL',
            'pv5',
            'pv5VAL',
            'pv6',
            'pv6VAL',
            'pv7',
            'pv7VAL',
            'pv8',
            'pv8VAL',
            'pv9',
            'pv9VAL',
            'pv10',
            'pv10VAL',
            'imageserver',
            'CPUspeed',
            'immversion',
            'corecotick',
            'cameratype',
            'threshhold',
            'byte632',
            'empty_space',
            'ZZZZ',
            'FFFF'
        ]
        bindata = file.read(1024)
        imm_headerdat = struct.unpack(imm_headformat, bindata)
        imm_header = dict(zip(imm_fieldnames, imm_headerdat))

        return imm_header

    