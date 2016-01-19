#ifdef _WIN32
    #include "libbmp_win.h"
#elif __linux__
    #include "libbmp_linux.h"
#else
    #warning "Unsported operating system"
#endif
