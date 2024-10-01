#ifdef _WIN
#include "vst/hosting/module_win32.cpp"
#include "common/threadchecker_win32.cpp"

#else
#include "vst/hosting/module_linux.cpp"
#include "common/threadchecker_linux.cpp"

#endif

// other includes
#include "common/commonstringconvert.cpp"
#include "vst/utility/stringconvert.cpp"
#include "vst/hosting/plugprovider.cpp"
#include "vst/hosting/module.cpp"
#include "vst/hosting/connectionproxy.cpp"
#include "pluginterfaces/base/funknown.cpp"
#include "vst/hosting/parameterchanges.cpp"
