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
    def __init__(self, filename:str, no_of_frames:int=-1, skip_frames:int = 0):
        self.filename = filename
        self.no_of_frames = no_of_frames
        self.index_data = []
        self.value_data = []
        self.skip_frames = skip_frames
        self.frames_read = 0
        self.lil_mtx = None
        self.__load__()

    def __load__(self):
        with open(self.filename, "rb") as file:
            if self.skip_frames > 0:
                self.__skip__(file)

            header = self.__read_imm_header(file)
            self.rows, self.cols = header['rows'], header['cols']
            self.is_compressed = bool(header['compression'] == 6)
            num_pixels = header['dlen']
            frame_index = 0

            while True:
                try:
                    num_pixels = header['dlen']
                    if self.is_compressed:
                        indexes = np.fromfile(file, dtype=np.uint32, count=num_pixels)
                        values = np.fromfile(file, dtype=np.uint16, count=num_pixels)
                        self.index_data.append(indexes)
                        self.value_data.append(values)
                    else:
                        values = np.fromfile(file, dtype=np.uint16, count=num_pixels)
                        #TODO

                    # Check for end of file.
                    if not file.peek(1):
                        break
                    header = self.__read_imm_header(file)
                    frame_index += 1
                    self.frames_read += 1
                    if self.no_of_frames != -1 and frame_index > self.no_of_frames:
                        break
                except Exception as err:
                    raise IOError("IMM file doesn't seems to be of right type") from err
        
    def __skip__(self, file):
        for _ in range(self.skip_frames):
            header = self.__read_imm_header(file)
            is_compressed = bool(header['compression'] == 6)
            num_pixels = header['dlen']
            payload_size = num_pixels * (6 if is_compressed else 2)
            file.read(payload_size)

    def data(self):
        return self.index_data, self.value_data

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

class RigakuReader(Reader):
    def __init__(self, file):
        self.file = file
    
    def __load(self):
        with open(self.file, 'r') as f:
            a = np.fromfile(f, dtype=np.uint64)

            b = (a >> 5+11)
            pix_ind = (b & 2**21-1).astype(int)
            pix_count = (a & 2**12-1).astype(int)
            pix_frame = (a >> 64-24).astype(int)



if __name__ == "__main__":
    IMM_FILE = "/home/faisal/Development/xpcs-eigen/data/pyxpcs/A002_MJ_PMA_47g61kDa_att2_160C_Lq0_001_00001-00512.imm"
    reader = IMMReader8ID(IMM_FILE)
    reader.load()

    from pyxpcs.structs import PyXPCSArray
    data = reader.array
    data.sum(1)

    pos, indicies, values = data.get_lil()
    print(indicies[0][:30])