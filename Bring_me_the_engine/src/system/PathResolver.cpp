#include "system/PathResolver.h"

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
#else
    #include <unistd.h>
#endif

std::filesystem::path PathResolver::getExecutableDir() {
    char buffer[4096];

#if defined(_WIN32)
    GetModuleFileNameA(NULL, buffer, sizeof(buffer));
#elif defined(__APPLE__)
    uint32_t size = sizeof(buffer);
    _NSGetExecutablePath(buffer, &size);
#else // Linux
    ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer));
    if (count == -1) throw std::runtime_error("Impossible de récupérer le chemin de l'exécutable.");
    buffer[count] = '\0';
#endif

    return std::filesystem::path(buffer).parent_path();
}

// Chemin relatif à partir de la racine du projet
const std::string PathResolver::getResourcePath(const std::string& relative) {
    return (std::filesystem::path(PROJECT_ROOT_DIR) / relative).string();
}
