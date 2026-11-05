#ifndef PUGMOD_PRECOMPILED_H
#define PUGMOD_PRECOMPILED_H

#ifndef _WIN32
#define _GLIBCXX_USE_CXX11_ABI 0
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define _strdup strdup
#define _unlink unlink
#define _vsnprintf vsnprintf
#define _write write
#define _close close
#define _acces access
#define _vsnwprintf vswprintf
#else
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wldap32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"crypt32.lib")
#pragma comment(lib,"normaliz.lib")
#endif

#ifdef _WIN32
#pragma warning(disable: 4100)
#pragma warning(disable: 4996)
#endif

// System
#include <osconfig.h>

// direct.h
#ifdef _WIN32
#include <direct.h>
#endif

// C++
#include <array>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

// cURL sources
#ifndef CURL_STATICLIB
#define CURL_STATICLIB
#endif

// cURL sources
#include <curl/curl.h>

// CSSDK
#include <extdll.h>
#include <eiface.h>

// MetaMod SDK
#include <meta_api.h>

// ReHLDS Api
#include <rehlds_api.h>

// ReGameDLL Api
#include <regamedll_api.h>

// Metamod Includes
#include "MetaMod.h"

// ReAPI & ReGameDLL
#include "ReAPI.h"
#include "ReGameDLL.h"

// JSON
#include "include/json.hpp"

// Time Format
#include "include/TimeFormat.h"

// Pickup Game Mod
#include "PugMenu.h"
#include "PugAdmin.h"
#include "PugAdminMenu.h"
#include "PugClientCmd.h"
#include "PugCurl.h"
#include "PugCvar.h"
#include "PugCvarControl.h"
#include "PugDemoRecord.h"
#include "PugDM.h"
#include "PugEngine.h"
#include "PugGameDesc.h"
#include "PugLang.h"
#include "PugLeader.h"
#include "PugLO3.h"
#include "PugMapList.h"
#include "PugMod.h"
#include "PugPause.h"
#include "PugRageBan.h"
#include "PugReady.h"
#include "PugRestrictItem.h"
#include "PugServerCmd.h"
#include "PugSpawnEdit.h"
#include "PugRoundStats.h"
#include "PugSmokeGrenade.h"
#include "PugSpread.h"
#include "PugStats.h"
#include "PugStatus.h"
#include "PugTask.h"
#include "PugTeamManager.h"
#include "PugTimer.h"
#include "PugUtil.h"
#include "PugVoteEnd.h"
#include "PugVoteMap.h"
#include "PugVoteMenu.h"
#include "PugVoteOvertime.h"
#include "PugVoteTeam.h"
#include "PugVoteSwapTeam.h"

#endif // PUGMOD_PRECOMPILED_H
