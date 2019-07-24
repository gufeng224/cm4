#include "ms_version.h"

#if defined(MVXV_MKFILE)
#define MVXV_V2 "MVX#" MVXV_CHANGELIST MVXV_EXT MVXV_MKFILE "#XVM"
#else
#define MVXV_V2 "MVX#" MVXV_CHANGELIST MVXV_EXT "#XVM"
#endif
const char* version=MVXV_V2;
