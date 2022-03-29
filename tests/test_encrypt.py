"""Тест коррекности шифровки расшифровки с доступом к либе из Python
"""

import sys
sys.path.append('./src/encrypt')
from encrypt import Encrypt
 
# пропишем ключ шифрования
key = b'\x01'
# Пропишем путь до библиотеки с функциями шифровки/расшифровки
library_path = 'src/encrypt/libencrypt.so'

# Инициализируем библиотеку шифровки.расшифровки
enc = Encrypt(library_path, key)

def assert_encrypt (test_str):
    ret_enc = enc.encode(test_str)
    ret_dec = enc.decode(ret_enc)
    status = test_str == ret_dec
    assert status, f"Encrypt error. {test_str} = {ret_dec}"

# проверим что текст остался тем же после шифровки-расшифровки
assert_encrypt('Это тестовая строка')
assert_encrypt('Тест чисел 1,2,3,4,5,6,7,8,9,0')
assert_encrypt('Тест спецсимволов Ё!"№;%:?*()_+~!@#$%^&*()_+}{')
print("Encrypt test passed")