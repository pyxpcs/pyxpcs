import struct
import numpy as np

# from scipy import sparse
from pyxpcs.structs import PyXPCSArray

class Reader:
    """
    Base class for File readers
    """

    def get_sparse_lil(self):
        pass

class IMMReader8ID(Reader):
    def __init__(self, filename:str, no_of_frames:int, skip_frames:int = 0):
        self.filename = filename
        self.no_of_frames = no_of_frames
        self.skip_frames = skip_frames
        assert (self.skip_frames <= self.no_of_frames)
        self.lil_mtx = None
        self.__load__()

    def __load__(self):
        with open(self.filename, "rb") as file:
            if self.skip_frames > 0:
                self.__skip__(file)

            header = self.__read_imm_header(file)
            self.rows, self.cols = header['rows'], header['cols']
            self.lil_mtx = PyXPCSArray(dims=(self.no_of_frames, self.rows, self.cols))
            self.is_compressed = bool(header['compression'] == 6)
            num_pixels = header['dlen']
            frame_index = 0

            while True:
                try:
                    num_pixels = header['dlen']
                    if self.is_compressed:
                        indexes = np.fromfile(file, dtype=np.uint32, count=num_pixels)
                        values = np.fromfile(file, dtype=np.uint16, count=num_pixels)
                        self.lil_mtx.add_frame(frame_index, indexes, values)
                    else:
                        values = np.fromfile(file, dtype=np.uint16, count=num_pixels)
                        # self.lil_mtx[frame_index, :] = values

                    # Check for end of file.
                    if not file.peek(1):
                        break
                    header = self.__read_imm_header(file)
                    frame_index += 1
                    if frame_index > self.no_of_frames:
                        break
                except Exception as err:
                    raise IOError("IMM file doesn't seems to be of right type") from err

        print(frame_index)
        
    def __skip__(self, file):
        for _ in range(self.skip_frames):
            header = self.__read_imm_header(file)
            is_compressed = bool(header['compression'] == 6)
            num_pixels = header['dlen']
            payload_size = num_pixels * (6 if is_compressed else 2)
            file.read(payload_size)

    def array(self):
        return self.lil_mtx

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


if __name__ == "__main__":
    IMM_FILE = "/home/faisal/Development/xpcs-eigen/data/pyxpcs/A002_MJ_PMA_47g61kDa_att2_160C_Lq0_001_00001-00512.imm"
    reader = IMMReader8ID(IMM_FILE)
    reader.load()

    from pyxpcs.structs import PyXPCSArray
    data = reader.array
    data.sum(1)

    pos, indicies, values = data.get_lil()
    print(indicies[0][:30])