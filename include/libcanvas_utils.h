#ifndef LIBCANVAS_DEBUG_H_
#define LIBCANVAS_DEBUG_H_

#ifdef WIN32
#include <synchapi.h>
// Include only syncapi.h to have the defition of Sleep function
// without all the windows.h stuff.
#else
#include <unistd.h>
#endif

namespace elp {
namespace utils {

#ifdef WIN32
inline void Sleep(int s) {
    ::Sleep(s * 1000);
}
#else
inline void Sleep(int s) {
    sleep(s);
}
#endif

} }
#endif
