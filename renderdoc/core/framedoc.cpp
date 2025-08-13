#include "core/framedoc.h"
#include "core/core.h"
#include "hooks/hooks.h"
#include "os/os_specific.h"
#include "api/replay/version.h"
#include <dlfcn.h>
#include "strings/string_utils.h"

namespace FrameDoc
{
LauncherConf launcher_conf = LauncherConf();
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

void ParseLauncherConf()
{
  rdcstr confstr;
  if(ReadLauncherConf(confstr))
  {
    RDCLOG("LauncherConf: %s", confstr.c_str());
    rdcarray<rdcstr> lines;
    split(confstr, lines, '\n');
    for(size_t i = 0; i < lines.size(); ++i)
    {
      rdcstr line = lines[i].trimmed();
      if(line.beginsWith("#"))
      {
        continue;
      }
      int32_t index = line.find('=');
      if(index > 0 && index < line.length() - 1)
      {
        rdcstr key = strlower(line.substr(0, index).trimmed());
        rdcstr value = strlower(line.substr(index+1, line.length() - index - 1).trimmed());

        if(key == "version")
        {
          rdcarray<rdcstr> version;
          split(value, version, '.');
          if(version.size() >= 2)
          {
            launcher_conf.major_version = atoi(version[0].c_str());
            launcher_conf.minor_version = atoi(version[1].c_str());
          }
        }
      }
    }
  }
  else
  {
    RDCLOG("LauncherConf: not found");
  }
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

rdcstr LoadLauncherConf()
{
  
}

bool ShouldInitLibOnLoad(rdcstr& OutDesc)
{
  OutDesc = "";
  if(FRAME_DOC_LAUNCHER_VERSION_SUPPORT(1, 0))
  {
    if(CheckProp("IGNORE_LAYERS"))
    {
      OutDesc = "property IGNORE_LAYERS";
      return true;
    }
    if(CheckPackageConf("/data/local/tmp/FrameDoc/"))
    {
      OutDesc = "tmp PackageConf";
      return true;
    }
  }
  else // fallback
  {
    if(CheckPackageConf("sdcard/FrameDoc/"))
    {
      OutDesc = "sdcard PackageConf";
      return true;
    }
  }

  return false;
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
  ParseLauncherConf();

  rdcstr shouldInitDesc;
  if(ShouldInitLibOnLoad(shouldInitDesc))
  {
    InitLib("OnLoad " + shouldInitDesc);
  }
  else
  {
    PreventLibUnloaded();
    // LibraryHooks::PreventInit();
    RDCLOG("FrameDoc v%s (%s) not initialized on load.", FRAMEDOC_MAJOR_MINOR_VERSION_STRING, GitVersionHash);
  }

}
}

