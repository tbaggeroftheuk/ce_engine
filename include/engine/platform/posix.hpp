#include <dlfcn.h>
#define LOAD_LIB(name) dlopen(name, RTLD_LAZY)
#define GET_SYM(lib, name) dlsym(lib, name)
#define CLOSE_LIB(lib) dlclose(lib)
typedef void* lib_handle;