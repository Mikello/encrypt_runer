"""Тест коррекности импорта из зашифрованного файла
"""

import os
import sys
sys.path.insert(0, os.getcwd())

import src.encrypted_import.encrypted_import as encrypted_import
from src.encrypt.encrypt import Encrypt

# пропишем ключ шифрования
key = b'\x01'

# Установим загрузчик шифрованных скриптов
library_path = 'src/encrypt/libencrypt.so'
encrypted_import.encrypt_loader = Encrypt(library_path, key)
encrypted_import.MyMetaFinder.default_path = os.getcwd()
encrypted_import.install()

# Загрузим шифрованный скрипт
from tests.res.test_encrypted_script import test_fn

assert test_fn() == 'test string'
print("Import test passed")