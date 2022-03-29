import ctypes 

class Encrypt:
    # key = b'\00'
    # library_path = './libencrypt.so'
    def __init__(self, library_path, key):
        assert isinstance(key, bytes)

        self.library_path = library_path
        self.key = key
        self.lib = ctypes.CDLL(self.library_path)

        # Указываем, что функции принимает char * и char, а возвращает char *
        self.lib.encode.restype = ctypes.c_char_p
        self.lib.encode.argtypes = [ctypes.POINTER(ctypes.c_char), ctypes.c_char, ctypes.c_ulong]

        self.lib.decode.restype = ctypes.c_char_p
        self.lib.decode.argtypes = [ctypes.POINTER(ctypes.c_char), ctypes.c_char, ctypes.c_ulong]
    
    def encode(self, str2bytes:str) -> bytes:
        """Шифрует входящую строку
        """
        try:
            if isinstance(str2bytes, str) and isinstance(self.key, bytes):
                str2bytes = str2bytes.encode('utf-8')
                return self.lib.encode(str2bytes, self.key, len(str2bytes))
        except:
            print("Encode error")
        return None
    
    def decode(self, bytes2str:bytes) -> str:
        """Расшифровывает входящую строку
        """
        try:
            if isinstance(bytes2str, bytes) and isinstance(self.key, bytes):
                out_str = self.lib.decode(bytes2str, self.key, len(bytes2str)).decode("utf-8")
                return out_str
        except:
            print("Decode error")
        return None