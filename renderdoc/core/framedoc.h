#ifndef FRAME_DOC_H_
#define FRAME_DOC_H_ 1
#include <atomic>

class rdcstr;

namespace FrameDoc
{
typedef void (*PFN_LibLoad)(void);

struct LauncherConf
{
  int32_t major_version = 0;
  int32_t minor_version = 0;
};

extern LauncherConf launcher_conf;

extern PFN_LibLoad g_LibInitFunction;

extern std::atomic_bool g_nLibInitialized;

extern std::atomic_bool g_nLibUnloadedPrevented;

extern std::atomic_bool g_nLogFileInitialized;

#define FRAME_DOC_LAUNCHER_VERSION_SUPPORT(major, minor) \
  (FrameDoc::launcher_conf.major_version > major ||      \
   (FrameDoc::launcher_conf.major_version == major && FrameDoc::launcher_conf.minor_version >= minor))

void InitLogFile();
/** try to prevent the library from being unloaded, increment our dlopen refcount (might not work
 * on android, but we'll try!)
 * we use RTLD_NOLOAD to prevent a second copy being loaded if this path doesn't refer to
 * ourselves or otherwise breaks because of android's terrible library handling.
 */
void PreventLibUnloaded();
void RegisterLibLoadFunction(PFN_LibLoad Func);
void InitLib(const rdcstr& init_from);
void InitLibOnLoad();
}

#endif

