#include <glad/gl.h>
#include <cstddef>

#if defined(SB_OS_WIN)
#include <windows.h>
#include <glad/wgl.h>
#elif defined(SB_OS_MACOS)
#include <dlfcn.h>
#elif defined(SB_OS_LINUX)
#include <glad/glx.h>
#include <dlfcn.h>
#else
#error "SoGL has not been ported to this OS"
#endif

namespace {
void *
SoGlGetProcAddress(const char *name) {
#if defined(SB_OS_WIN)
    return wglGetProcAddress((LPCSTR)name);
#else
    static void *handle = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL);

    if (handle) {
        return dlsym(handle, (const char *)name);
    }
    return NULL;
#endif
}
} // namespace

void
SoGlInit() {
    gladSetGLOnDemandLoader((GLADloadfunc)SoGlGetProcAddress);
#if defined(SB_OS_WIN)
    gladSetWGLOnDemandLoader((GLADloadfunc)SoGlGetProcAddress);
#endif
#if defined(SB_OS_LINUX)
    gladSetGLXOnDemandLoader((GLADloadfunc)SoGlGetProcAddress);
#endif
}
