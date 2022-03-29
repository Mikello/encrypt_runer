import sys
from pathlib import Path

sys.path.append('/home/evgeny/work/project/GTP/cpp_runer/src/encrypt')
from encrypt import Encrypt

input_str = Path("tests/res/test_main.py")
output_str = input_str.parent / (input_str.stem + '.ypy')


# пропишем ключ шифрования
key = b'\x01'

# Пропишем путь до библиотеки с функциями шифровки/расшифровки
library_path = '/home/evgeny/work/project/GTP/cpp_runer/src/encrypt/libencrypt.so'
enc = Encrypt(library_path, key)

with open(str(input_str), 'r') as f:
    data_in = f.read()

data = enc.encode(data_in)

with open(str(output_str), 'wb') as f:
    f.write(data)