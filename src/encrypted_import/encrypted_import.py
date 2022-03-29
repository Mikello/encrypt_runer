import sys
import os.path

from importlib.abc import Loader, MetaPathFinder
from importlib.util import spec_from_file_location

EXPS = ['.ypy']

class MyMetaFinder(MetaPathFinder):
    default_path = os.getcwd()
    def check_file(self, filename, extention = ['.py']):
        for ext in extention:
            if os.path.exists(filename + ext):
                break
        return filename + ext
        
    def find_spec(self, fullname, path, target=None):
        if path is None or path == "":
            path = [MyMetaFinder.default_path] # top level import -- 

        name = fullname.split(".")[-1]
        
        for entry in path:
            if os.path.isdir(os.path.join(entry, name)):
                filename = self.check_file(os.path.join(entry, name, "__init__"), EXPS)
                submodule_locations = [os.path.join(entry, name)]
            else:
                submodule_locations = None
                filename = self.check_file(os.path.join(entry, name), EXPS)

            if  not os.path.exists(filename):
                    continue

            return spec_from_file_location(fullname, filename, loader=MyLoader(filename),
                submodule_search_locations=submodule_locations)

        return None # we don't know how to import this

class MyLoader(Loader):
    def __init__(self, filename):
        self.filename = filename
        try:
            self.encrypt_loader = encrypt_loader
        except Exception as e:
            self.decode = lambda x: x.decode("utf-8")

    def create_module(self, spec):
        return None # use default module creation semantics

    def exec_module(self, module):
        with open(self.filename, 'rb') as f:
            data = f.read()
        
        data = self.encrypt_loader.decode(data)

        exec(data, vars(module))

def install():
    sys.meta_path.insert(0, MyMetaFinder())