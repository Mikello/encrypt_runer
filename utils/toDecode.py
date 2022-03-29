import sys
from pathlib import Path

sys.path.append('/home/evgeny/work/project/GTP/cpp_runer/src/encrypt')
from src.encrypt import Encrypt

input_str = Path("/home/evgeny/work/project/GTP/cpp_runer/tests/res/test_main_key_q.ypy")
output_str = input_str.parent / (input_str.stem + '.py')


# пропишем ключ шифрования
key = b'\x01'
# Пропишем путь до библиотеки с функциями шифровки/расшифровки
library_path = '/home/evgeny/work/project/GTP/cpp_runer/src/encrypt/libencrypt.so'
enc = Encrypt(library_path, key)

with open(str(input_str), 'rb') as f:
    data = f.read()

data = enc.decode(data)

with open(str(output_str), 'w') as f:
    f.write(data)