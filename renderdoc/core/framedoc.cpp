#include "core/framedoc.h"
#include "core/core.h"
#include "hooks/hooks.h"
#include "os/os_specific.h"
#include "api/replay/version.h"
#include <dlfcn.h>

namespace FrameDoc
{
PFN_LibLoad g_LibInitFunction = nullptr;
std::atomic_bool g_nLibInitialized{false};
std::atomic_bool g_nLibUnloadedPrevented{false};
std::atomic_bool g_nLogFileInitialized{false};

bool framedoc_library_enabled()
{
  bool shouldEnabled = false;
  rdcstr package_name;
  FileIO::GetExecutableFilename(package_name);
  rdcstr confFile = rdcstr("sdcard/FrameDoc/") + package_name + rdcstr(".conf");
  bool file_exsists = FileIO::exists(confFile);
  if(file_exsists)
  {
    shouldEnabled = true;
  }

  return shouldEnabled;
}

bool ReadLauncherConf(rdcstr& OutContent)
{
  rdcstr confFile = rdcstr("/data/local/tmp/FrameDoc/FrameDocLauncher.conf");
  bool file_exsists = FileIO::exists(confFile);
  if(file_exsists)
  {
    FileIO::ReadAll(confFile, OutContent);
    return true;
  }

  return false;
}

bool CheckPackageConf(const rdcstr &dir_path, const rdcstr &file_extension = "conf")
{
  rdcstr package_name;
  FileIO::GetExecutableFilename(package_name);
  rdcstr path = dir_path.endsWith("/") ? dir_path : dir_path + "/";
  rdcstr conf_file = path + package_name + "." + file_extension;
  return FileIO::exists(conf_file);
}

bool CheckProp(const rdcstr &prop_name)
{
  rdcstr prop_value = Process::GetEnvVariable(prop_name);
  if(!prop_value.empty() && prop_value[0] == '1')
  {
    return true;
  }

  return false;
}

int ShouldInitLibOnLoad()
{
  // check property
  if(CheckProp("IGNORE_LAYERS"))
  {
    return 1;
  }
  if(CheckProp("FRAMEDOC_IGNORE_LAYERS"))
  {
    return 2;
  }
  // check files
  if(CheckPackageConf("sdcard/FrameDoc/"))
  {
    return 3;
  }
  if(CheckPackageConf("/data/local/tmp/FrameDoc/"))
  {
    return 4;
  }

  return 0;
}

void InitLogFile()
{
  bool initialzied = false;
  if(g_nLogFileInitialized.compare_exchange_strong(initialzied, true))
  {
    RenderDoc::Inst().InitLogFile();
  }
}

void PreventLibUnloaded()
{
  bool prevented = false;
  if(g_nLibUnloadedPrevented.compare_exchange_strong(prevented, true))
  {
    rdcstr selfLib;
    FileIO::GetLibraryFilename(selfLib);
    if(FileIO::exists(selfLib))
    {
      void *handle = dlopen(selfLib.c_str(), RTLD_NOW | RTLD_NOLOAD | RTLD_LOCAL);
      if(handle)
        RDCLOG("Dummy-loaded %s with dlopen to prevent library unload", selfLib.c_str());
      else
        RDCLOG("Failed to dummy-loaded %s with dlopen", selfLib.c_str());
    }
    else
    {
      RDCLOG("Couldn't dummy-load %s because it doesn't exist", selfLib.c_str());
    }
  }
}

void RegisterLibLoadFunction(PFN_LibLoad Func)
{
  g_LibInitFunction = Func;
}

void InitLib(const rdcstr& init_from)
{
  bool initialized = false;
  if(g_nLibInitialized.compare_exchange_strong(initialized, true))
  {
    if(g_LibInitFunction)
    {
      g_LibInitFunction();
      RDCLOG("FrameDoc v%s (%s) initialised from %s.", FRAMEDOC_MAJOR_MINOR_VERSION_STRING, GitVersionHash, init_from.c_str());
    }
  }
}

void InitLibOnLoad()
{
  InitLogFile();
  // check launcher conf
  rdcstr LauncherConf;
  if(ReadLauncherConf(LauncherConf))
  {
    RDCLOG("FrameDocLauncher.conf: %s", LauncherConf.c_str());
  }
  
  if(int shouldInit = ShouldInitLibOnLoad())
  {
    InitLib("OnLoad_" + ToStr(shouldInit));
  }
  else
  {
    PreventLibUnloaded();
    // LibraryHooks::PreventInit();
    RDCLOG("FrameDoc v%s (%s) not initialized on load.", FRAMEDOC_MAJOR_MINOR_VERSION_STRING, GitVersionHash);
  }

}
}

