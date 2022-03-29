#include <fstream>
// #include <iostream>

namespace fs = std::experimental::filesystem;

std::string build_argv(int argc, char **argv);

char * load_source (const char* name);

fs::path get_exe_path(char *argv);

fs::path get_script_path(fs::path working_dir, char *argv);
