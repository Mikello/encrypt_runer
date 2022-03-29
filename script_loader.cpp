#include <Python.h>
#include <fstream>
#include <experimental/filesystem>
#include <iostream>
#include <cctype>
#include <algorithm>

#include <iomanip>
#include <sstream>


#include <stdio.h>
#include <dlfcn.h>
#include "script_loader.hpp"

using namespace std;
namespace fs = std::experimental::filesystem;


int main(int argc, char **argv) {
    if (argc < 2)
    {
        std::cout << "Укажите скрипт для запуска " << "\n";
        return 0;
    }
    std::string path2encryptedlib = "src/encrypt/libencrypt.so";
    std::string path2encrypted_import = "src/encrypted_import/encrypted_import.ypy";
    std::string path2encrypt = "src/encrypt/encrypt.ypy";

    std::string encrypt_extension = ".ypy";
    char key = '\x01'; // Ключ для шифровки/расшифровки

    // Рабочая директория
    fs::path working_dir = get_exe_path(argv[0]);

    // Загрузим библиотеку шифровки/расшифровки
    fs::path lib_name = working_dir / path2encryptedlib.c_str();
    void *lib = dlopen(lib_name.c_str(), RTLD_LAZY);
    if (!lib) {
        printf("cannot open library '%s'\n", "libencrypt.so");
        return 0;
    }

    typedef char * (*encode_decode_t)(char *, char, size_t); 
    encode_decode_t encode = (encode_decode_t)dlsym(lib, "encode");
    encode_decode_t decode = (encode_decode_t)dlsym(lib, "decode");

    // Загрузим и расшифруем код шифровальщика
    fs::path encrypt_path = working_dir / path2encrypt.c_str();
    char* encrypt_src_encrypted = load_source(encrypt_path.c_str());
    char* encrypt_src = decode(encrypt_src_encrypted, key, strlen(encrypt_src_encrypted));
    free(encrypt_src_encrypted);

    // Загрузим и расшифруем код импортера
    fs::path importer_path = working_dir / path2encrypted_import.c_str();
    char* importer_src_encrypted = load_source(importer_path.c_str());
    char* importer_src = decode(importer_src_encrypted, key, strlen(importer_src_encrypted));
    free(importer_src_encrypted);
    
    Py_Initialize();
    std::string working_dir_str = std::string(working_dir.c_str());
    std::string exec_str = "import os\nimport sys\nsys.path.insert(0, \'" + working_dir_str + "\')";
    PyRun_SimpleString(exec_str.c_str());

    // загрузим код шифровальщика
    PyRun_SimpleString(encrypt_src);
    free(encrypt_src);

    //Инстанцируем шифровальщика
    exec_str = "encrypt_loader = Encrypt(";
    exec_str += "\'" + std::string(lib_name.c_str()) + "\',";
    exec_str += "b\'" + std::string(1, key) + "\')";
    PyRun_SimpleString(exec_str.c_str());


    // загрузим код импортера
    PyRun_SimpleString(importer_src);
    free(importer_src);

    //инсталируем импортера
    exec_str = "MyMetaFinder.default_path = \'" + working_dir_str + "\'\n";
    exec_str += "EXPS = [\'" + encrypt_extension + "\']\n";
    exec_str += "install()";
    PyRun_SimpleString(exec_str.c_str());


    // подготовим путь запуска python скрипта
    fs::path script_file = get_script_path(working_dir, argv[1]);
    if (! fs::is_regular_file(script_file))
        return 0;

    std::string extension = script_file.extension();
    transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    if (extension != encrypt_extension)
    {
        std::cout << "Неправильное расширение файла. ";
        std::cout << "Должно быть: \'" << encrypt_extension << "\'\n";
        return 0;
    }

    // Инициализируем аргументы командной строки
    exec_str = "sys.argv=" + build_argv(argc, argv);
    PyRun_SimpleString(exec_str.c_str());

    // Загрузим и расшифруем код python скрипта
    char* script_file_src_encrypted = load_source(script_file.c_str());
    char* script_file_src = decode(script_file_src_encrypted, key, strlen(script_file_src_encrypted));
    free(script_file_src_encrypted);

    // Запустим код 
    PyRun_SimpleString("__name__ = \'__main__\'");
    PyRun_SimpleString(script_file_src);

    // PyObject *pcode, *presult, *pdict_global;

    // /* make new namespace dictionary */
    // pdict_global = PyDict_New();
    // if (pdict_global == NULL) 
    //     return -2;
    // PyDict_SetItemString(pdict_global, "__builtins__", PyEval_GetBuiltins());

    // pcode = Py_CompileString(script_file_src, "python_script", Py_file_input);
    // free(script_file_src);

    // PyDict_SetItemString(pdict_global, "__name__", PyUnicode_FromString("__main__"));
    // /* run compiled bytecode in namespace dict */
    // if (pcode) {
    //     presult = PyEval_EvalCode(pcode, pdict_global, pdict_global);
    //     Py_XDECREF(pcode);
    // }
    // else
    //     return -1;
    
    // /* free referenced objects */
    // Py_XDECREF(pdict_global);
    // if (presult == NULL)
    //     return -2;
    // Py_DECREF(presult);

    Py_Finalize();

    return 0;
}



std::string build_argv(int argc, char **argv) {
    /*
    Подготовим аргументы командной строки в виде list
    */
    std::string argv_s = "[\"";
    for (size_t i = 1; i < argc-1; i++){
        argv_s += argv[i];
        argv_s += "\",\"";
    }
    argv_s += argv[argc-1];
    argv_s += "\"]";
    return argv_s;
}

char * load_source (const char* name) {
    /*
    Загрузить код загрузчика
    */
    int size;
    char * memblock;

    ifstream file (name, ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
    size = file.tellg();
    char * memblock = (char *)malloc( (size + 1 ) * sizeof( char ) );
    file.seekg (0, ios::beg);
    file.read (memblock, size);
    file.close();

    memblock[size] = 0;
    return memblock;
    }
    return nullptr;
}

std::experimental::filesystem::path get_exe_path(char *argv) {
    /*
    Получить полный путь к запускаемому Python скрипту
    */
    fs::path workdir = fs::current_path();
    fs::path workfile = fs::path(argv);
    
    if (!workfile.is_absolute())
    {
        workdir /= &workfile.c_str()[1];
    workdir = workdir.remove_filename();
    }
    else
    {
        workdir = workfile.remove_filename();
    }
    return workdir;
}

fs::path get_script_path(fs::path working_dir, char *argv) {
    /*
    Подготовим путь запуска python скрипта
    */
    fs::path script_file;

    if ( argv[0] == '.')
    {
        script_file = working_dir / &argv[1];
    }
    else
    {
        script_file = (fs::path) argv;
        if ( ! fs::is_regular_file(script_file))
            script_file = working_dir / argv;

        if ( ! fs::is_regular_file(script_file))
            script_file.clear();
    }
    return script_file;
}