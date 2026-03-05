#include "precompiled.h"

CPugUtil gPugUtil;

const char *CPugUtil::GetPath() {
  if (this->m_Path.empty()) {
    std::string GameDir = gpMetaUtilFuncs->pfnGetGameInfo(PLID, GINFO_GAMEDIR);

    if (!GameDir.empty()) {
      this->m_Path = gpMetaUtilFuncs->pfnGetPluginPath(PLID);

      if (!this->m_Path.empty()) {
        this->m_Path.erase(0, GameDir.length() + 1U);

        std::replace(this->m_Path.begin(), this->m_Path.end(), (char)(92),
                     (char)(47));

        this->m_Path.erase(this->m_Path.find_last_of((char)(47)),
                           this->m_Path.length());

        while (std::count(this->m_Path.begin(), this->m_Path.end(),
                          (char)(47)) > 1) {
          this->m_Path.erase(this->m_Path.find_last_of((char)(47)),
                             this->m_Path.length());
        }
      }
    }
  }

  return this->m_Path.c_str();
}

const char *CPugUtil::GetFullPath() {
  if (this->m_FullPath.empty()) {
    std::string GameDir = gpMetaUtilFuncs->pfnGetGameInfo(PLID, GINFO_GAMEDIR);

    if (!GameDir.empty()) {
      this->m_FullPath = gpMetaUtilFuncs->pfnGetPluginPath(PLID);

      if (!this->m_FullPath.empty()) {
        std::replace(this->m_FullPath.begin(), this->m_FullPath.end(),
                     (char)(92), (char)(47));

        this->m_FullPath.erase(this->m_FullPath.find_last_of((char)(47)),
                               this->m_FullPath.length());

        this->m_FullPath.erase(this->m_FullPath.find_last_of((char)(47)),
                               this->m_FullPath.length());
      }
    }
  }

  return this->m_FullPath.c_str();
}

int CPugUtil::MakeDirectory(const char *Path) {
  if (Path) {
    if (Path[0u] != '\0') {
      struct stat st = {};

      if (stat(Path, &st) == -1) {
#if defined(_WIN32)
        return _mkdir(Path);
#else
        return mkdir(Path, S_IRWXU | S_IRGRP | S_IROTH);
#endif
      }
    }
  }

  return 0;
}

void CPugUtil::ServerCommand(const char *Format, ...) {
  static char Command[255] = {0};

  va_list ArgList;

  va_start(ArgList, Format);

  vsnprintf(Command, sizeof(Command), Format, ArgList);

  va_end(ArgList);

  strcat(Command, "\n");

  SERVER_COMMAND(Command);
}

bool CPugUtil::IsNetClient(edict_t *pEntity) {
  if (!FNullEnt(pEntity)) {
    if (pEntity->free == TRUE) {
      return false;
    }

    if (pEntity->pvPrivateData == NULL) {
      return false;
    }

    if ((pEntity->v.flags & FL_DORMANT) == FL_DORMANT) {
      return false;
    }

    if ((pEntity->v.flags & FL_PROXY) == FL_PROXY) {
      return false;
    }

    if ((pEntity->v.flags & FL_FAKECLIENT) == FL_FAKECLIENT) {
      return false;
    }

    return true;
  }

  return false;
}

void CPugUtil::PrintColor(edict_t *pEntity, int Sender, const char *Format,
                          ...) {
  static int iMsgSayText;

  if (iMsgSayText ||
      (iMsgSayText = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "SayText", NULL))) {
    char szText[191] = {0};

    va_list vArgList;

    va_start(vArgList, Format);

    vsnprintf(szText, sizeof(szText), Format, vArgList);

    va_end(vArgList);

    if (Sender < E_PRINT_TEAM::BLUE || Sender > gpGlobals->maxClients) {
      Sender = E_PRINT_TEAM::DEFAULT;
    } else if (Sender < E_PRINT_TEAM::DEFAULT) {
      Sender = abs(Sender) + MAX_CLIENTS;
    }

    this->ParseColor(szText);

    if (!FNullEnt(pEntity)) {
      if (!(pEntity->v.flags & FL_FAKECLIENT)) {
        MESSAGE_BEGIN(MSG_ONE, iMsgSayText, nullptr, pEntity);
        WRITE_BYTE(Sender ? Sender : ENTINDEX(pEntity));
        WRITE_STRING("%s");
        WRITE_STRING(szText);
        MESSAGE_END();
      }
    } else {
      for (int i = 1; i <= gpGlobals->maxClients; ++i) {
        pEntity = INDEXENT(i);

        if (this->IsNetClient(pEntity)) {
          MESSAGE_BEGIN(MSG_ONE, iMsgSayText, nullptr, pEntity);
          WRITE_BYTE(Sender ? Sender : i);
          WRITE_STRING("%s");
          WRITE_STRING(szText);
          MESSAGE_END();
        }
      }
    }
  }
}

void CPugUtil::TeamInfo(edict_t *pEntity, int playerIndex,
                        const char *pszTeamName) {
  if (!FNullEnt(pEntity)) {
    static int iMsgTeamInfo;

    if (iMsgTeamInfo || (iMsgTeamInfo = gpMetaUtilFuncs->pfnGetUserMsgID(
                             PLID, "TeamInfo", NULL))) {
      MESSAGE_BEGIN(MSG_ONE, iMsgTeamInfo, nullptr, pEntity);
      WRITE_BYTE(playerIndex);
      WRITE_STRING(pszTeamName);
      MESSAGE_END();
    }
  }
}

void CPugUtil::ParseColor(char *Buffer) {
  size_t Length = strlen(Buffer);

  int Offset = 0;

  if (Length > 0) {
    int Character;

    for (size_t i = 0; i < Length; i++) {
      Character = Buffer[i];

      if (Character == '^' && (i != Length - 1)) {
        Character = Buffer[++i];

        if (Character == 'n' || Character == 't' ||
            (Character >= '1' && Character <= '4')) {
          switch (Character) {
          case '1':
            Character = '\x01';
            break;
          case '2':
            Character = '\x02';
            break;
          case '3':
            Character = '\x03';
            break;
          case '4':
            Character = '\x04';
            break;
          case 'n':
            Character = '\n';
            break;
          case 't':
            Character = '\t';
            break;
          }

          Offset++;
        }
      }

      Buffer[i - Offset] = Character;
    }

    Buffer[Length - Offset] = '\0';
  }
}

void CPugUtil::ClientPrint(edict_t *pEntity, int iMsgDest, const char *Format,
                           ...) {
  va_list argList;

  va_start(argList, Format);

  char Buffer[188] = {0};

  int Length = vsnprintf(Buffer, sizeof(Buffer), Format, argList);

  va_end(argList);

  if (iMsgDest == E_PRINT::CONSOLE) {
    if (Length > 125) {
      Length = 125;
    }

    Buffer[Length++] = '\n';
    Buffer[Length++] = '\n';
    Buffer[Length] = 0;
  }

  static int iMsgTextMsg;

  if (iMsgTextMsg ||
      (iMsgTextMsg = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "TextMsg", NULL))) {
    if (!FNullEnt(pEntity)) {
      MESSAGE_BEGIN(MSG_ONE, iMsgTextMsg, nullptr, pEntity);
    } else {
      MESSAGE_BEGIN(MSG_BROADCAST, iMsgTextMsg);
    }

    WRITE_BYTE(iMsgDest);
    WRITE_STRING("%s");
    WRITE_STRING(Buffer);
    MESSAGE_END();
  }
}

void CPugUtil::ClientCommand(edict_t *pEntity, const char *Format, ...) {
  static char Command[256] = {0};

  va_list ArgList;

  va_start(ArgList, Format);

  vsnprintf(Command, sizeof(Command), Format, ArgList);

  va_end(ArgList);

  strcat(Command, "\n");

  if (!FNullEnt(pEntity)) {
    CLIENT_COMMAND(pEntity, Command);
  } else {
    for (int i = 1; i <= gpGlobals->maxClients; ++i) {
      pEntity = INDEXENT(i);

      if (this->IsNetClient(pEntity)) {
        CLIENT_COMMAND(pEntity, Command);
      }
    }
  }
}

void CPugUtil::ClientDrop(int EntityIndex, const char *Format, ...) {
  va_list argList;

  va_start(argList, Format);

  char Buffer[255] = {0};

  vsnprintf(Buffer, sizeof(Buffer), Format, argList);

  va_end(argList);

  if (g_RehldsSvs && g_RehldsFuncs) {
    auto Gameclient = g_RehldsSvs->GetClient(EntityIndex - 1);

    if (Gameclient) {
      g_RehldsFuncs->DropClient(Gameclient, false, "%s", Buffer);
    }
  } else {
    auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

    if (Player) {
      int UserIndex = GETPLAYERUSERID(Player->edict());

      if (!FNullEnt(Player->edict()) && UserIndex > 0) {
        if (strlen(Buffer) > 0) {
          this->ServerCommand("kick #%d \"%s\"", UserIndex, Buffer);
        } else {
          this->ServerCommand("kick #%d", UserIndex);
        }
      }
    }
  }
}

void CPugUtil::ClientSlap(edict_t *pEntity, float Damage,
                          bool RandomDirection) {
  if (pEntity->v.deadflag == DEAD_NO && pEntity->v.health > 0.0f) {
    if (pEntity->v.health > Damage) {
      if (RandomDirection) {
        pEntity->v.velocity.x += RANDOM_LONG(-600, 600);
        pEntity->v.velocity.y += RANDOM_LONG(-180, 180);
        pEntity->v.velocity.z += RANDOM_LONG(100, 200);
      } else {
        Vector v_forward, v_right;
        Vector vang = pEntity->v.angles;

        float fang[3] = {0.0f, 0.0f, 0.0f};

        fang[0] = vang.x;
        fang[1] = vang.y;
        fang[2] = vang.z;

        UTIL_MakeVectorsPrivate(fang, v_forward, v_right, NULL);

        pEntity->v.velocity =
            pEntity->v.velocity + v_forward * 220 + Vector(0.0f, 0.0f, 200.0f);
      }

      pEntity->v.punchangle.x = static_cast<vec_t>(RANDOM_LONG(-10, 10));
      pEntity->v.punchangle.y = static_cast<vec_t>(RANDOM_LONG(-10, 10));

      pEntity->v.health -= Damage;

      float armor = pEntity->v.armorvalue;

      armor -= Damage;

      if (armor < 0) {
        armor = 0;
      }

      pEntity->v.armorvalue = armor;

      pEntity->v.dmg_inflictor = pEntity;

      static const char *cs_sound[6] = {
          "player/pl_pain2.wav", "player/pl_pain4.wav", "player/pl_pain5.wav",
          "player/pl_pain6.wav", "player/pl_pain7.wav"};

      EMIT_SOUND_DYN2(pEntity, CHAN_VOICE, cs_sound[RANDOM_LONG(0, 4)], 1.0,
                      ATTN_NORM, 0, PITCH_NORM);
    } else {
      float Backup = pEntity->v.frags;

      MDLL_ClientKill(pEntity);

      pEntity->v.frags = Backup;
    }
  }
}

unsigned short CPugUtil::FixedUnsigned16(float Value, float Scale) {
  int Result = Value * Scale;

  if (Result < 0) {
    Result = 0;
  }

  if (Result > USHRT_MAX) {
    Result = USHRT_MAX;
  }

  return (unsigned short)Result;
}

short CPugUtil::FixedSigned16(float Value, float Scale) {
  int Result = Value * Scale;

  if (Result > SHRT_MAX) {
    Result = SHRT_MAX;
  }

  if (Result < SHRT_MIN) {
    Result = SHRT_MIN;
  }

  return (short)Result;
}

hudtextparms_t CPugUtil::SetHud(vec3_t FromColor, vec3_t ToColor, float X,
                                float Y, int Effect, float FxTime,
                                float HoldTime, float FadeInTime,
                                float FadeOutTime, int Channel) {
  hudtextparms_t TextParams;

  TextParams.r1 = static_cast<byte>(ToColor.x);
  TextParams.g1 = static_cast<byte>(ToColor.y);
  TextParams.b1 = static_cast<byte>(ToColor.z);
  TextParams.a1 = 0xFF;

  TextParams.r2 = static_cast<byte>(FromColor.x);
  TextParams.g2 = static_cast<byte>(FromColor.y);
  TextParams.b2 = static_cast<byte>(FromColor.z);
  TextParams.a2 = 0xFF;

  TextParams.x = X;
  TextParams.y = Y;

  TextParams.effect = Effect;
  TextParams.fxTime = FxTime;
  TextParams.holdTime = HoldTime;
  TextParams.fadeinTime = FadeInTime;
  TextParams.fadeoutTime = FadeOutTime;
  TextParams.channel = Channel;

  return TextParams;
}

void CPugUtil::SendHud(edict_t *pEntity, const hudtextparms_t &TextParams,
                       const char *Format, ...) {
  static char szString[511] = {0};

  va_list ArgPtr;

  va_start(ArgPtr, Format);

  Q_vsnprintf(szString, sizeof(szString), Format, ArgPtr);

  va_end(ArgPtr);

  this->ParseColor(szString);

  if (!FNullEnt(pEntity)) {
    MESSAGE_BEGIN(MSG_ONE_UNRELIABLE, SVC_TEMPENTITY, nullptr, pEntity);
  } else {
    MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
  }

  WRITE_BYTE(TE_TEXTMESSAGE);
  WRITE_BYTE(TextParams.channel & 0xFF);

  WRITE_SHORT(this->FixedSigned16(TextParams.x, BIT(13)));
  WRITE_SHORT(this->FixedSigned16(TextParams.y, BIT(13)));

  WRITE_BYTE(TextParams.effect);

  WRITE_BYTE(TextParams.r1);
  WRITE_BYTE(TextParams.g1);
  WRITE_BYTE(TextParams.b1);
  WRITE_BYTE(TextParams.a1);

  WRITE_BYTE(TextParams.r2);
  WRITE_BYTE(TextParams.g2);
  WRITE_BYTE(TextParams.b2);
  WRITE_BYTE(TextParams.a2);

  WRITE_SHORT(this->FixedUnsigned16(TextParams.fadeinTime, BIT(8)));
  WRITE_SHORT(this->FixedUnsigned16(TextParams.fadeoutTime, BIT(8)));
  WRITE_SHORT(this->FixedUnsigned16(TextParams.holdTime, BIT(8)));

  if (TextParams.effect == 2) {
    WRITE_SHORT(this->FixedUnsigned16(TextParams.fxTime, BIT(8)));
  }

  WRITE_STRING(szString);
  MESSAGE_END();
}

void CPugUtil::SendDHud(edict_t *pEntity, const hudtextparms_t &TextParams,
                        const char *Format, ...) {
#define DRC_CMD_MESSAGE 6

  static char Text[128];

  va_list ArgPtr;

  va_start(ArgPtr, Format);

  Q_vsnprintf(Text, sizeof(Text), Format, ArgPtr);

  va_end(ArgPtr);

  this->ParseColor(Text);

  int Length = strlen(Text);

  if (Length > 127) {
    Length = 127;

    if (((Text[Length - 1] & 0xFF) >= 0xC2) &&
        ((Text[Length - 1] & 0xFF) <= 0xEF)) {
      Length--;
    }

    Text[Length] = 0;
  }

  if (!FNullEnt(pEntity)) {
    MESSAGE_BEGIN(MSG_ONE_UNRELIABLE, SVC_DIRECTOR, nullptr, pEntity);
  } else {
    MESSAGE_BEGIN(MSG_BROADCAST, SVC_DIRECTOR);
  }

  WRITE_BYTE(Length + 31);
  WRITE_BYTE(DRC_CMD_MESSAGE);
  WRITE_BYTE(TextParams.effect);
  WRITE_LONG(TextParams.b1 + (TextParams.g1 << 8) + (TextParams.r1 << 16));
  WRITE_LONG((*((int32_t *)&TextParams.x)));
  WRITE_LONG((*((int32_t *)&TextParams.y)));
  WRITE_LONG((*((int32_t *)&TextParams.fadeinTime)));
  WRITE_LONG((*((int32_t *)&TextParams.fadeoutTime)));
  WRITE_LONG((*((int32_t *)&TextParams.holdTime)));
  WRITE_LONG((*((int32_t *)&TextParams.fxTime)));
  WRITE_STRING(Text);
  MESSAGE_END();
}

void CPugUtil::SendDeathMsg(edict_t *pEntity, CBaseEntity *pKiller,
                            CBasePlayer *pVictim, CBasePlayer *pAssister,
                            entvars_t *pevInflictor,
                            const char *killerWeaponName,
                            int iDeathMessageFlags, int iRarityOfKill) {
  static int iDeathMsg;

  if (iDeathMsg ||
      (iDeathMsg = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "DeathMsg", NULL))) {
    if (!FNullEnt(pEntity)) {
      MESSAGE_BEGIN(MSG_ONE, iDeathMsg, nullptr, pEntity);
    } else {
      MESSAGE_BEGIN(MSG_ALL, iDeathMsg);
    }

    WRITE_BYTE((pKiller && pKiller->IsPlayer()) ? pKiller->entindex() : 0);
    WRITE_BYTE(pVictim->entindex());
    WRITE_BYTE((iRarityOfKill & 0x001));
    WRITE_STRING(killerWeaponName);

    if (iDeathMessageFlags > 0) {
      WRITE_LONG(iDeathMessageFlags);

      if (iDeathMessageFlags & 0x001) {
        WRITE_COORD(pVictim->pev->origin.x);
        WRITE_COORD(pVictim->pev->origin.y);
        WRITE_COORD(pVictim->pev->origin.z);
      }

      if (iDeathMessageFlags & 0x002) {
        WRITE_BYTE((pAssister && pAssister->IsPlayer()) ? pAssister->entindex()
                                                        : 0);
      }

      if (iDeathMessageFlags & 0x004) {
        WRITE_BYTE(iRarityOfKill);
      }
    }

    MESSAGE_END();
  }
}

void CPugUtil::ScreenFade(edict_t *pEntity, float Duration, float HoldTime,
                          int FadeFlags, int Red, int Green, int Blue,
                          int Alpha) {
  if (!FNullEnt(pEntity)) {
    static int iMsgScreenFade;

    if (iMsgScreenFade || (iMsgScreenFade = gpMetaUtilFuncs->pfnGetUserMsgID(
                               PLID, "ScreenFade", nullptr))) {
      MESSAGE_BEGIN(MSG_ONE, iMsgScreenFade, nullptr, pEntity);
      WRITE_SHORT(gPugUtil.FixedUnsigned16(Duration, BIT(12)));
      WRITE_SHORT(gPugUtil.FixedUnsigned16(HoldTime, BIT(12)));
      WRITE_SHORT(FadeFlags);
      WRITE_BYTE(Red);
      WRITE_BYTE(Green);
      WRITE_BYTE(Blue);
      WRITE_BYTE(Alpha);
      MESSAGE_END();
    }
  } else {
    for (int i = 1; i <= gpGlobals->maxClients; ++i) {
      pEntity = INDEXENT(i);

      if (this->IsNetClient(pEntity)) {
        this->ScreenFade(pEntity, Duration, HoldTime, FadeFlags, Red, Green,
                         Blue, Alpha);
      }
    }
  }
}

void CPugUtil::ScreenShake(edict_t *pEntity, float Amplitude, float Duration,
                           float Frequency) {
  if (!FNullEnt(pEntity)) {
    static int iMsgScreenShake;

    if (iMsgScreenShake || (iMsgScreenShake = gpMetaUtilFuncs->pfnGetUserMsgID(
                                PLID, "ScreenShake", nullptr))) {
      MESSAGE_BEGIN(MSG_ONE, iMsgScreenShake, nullptr, pEntity);
      WRITE_SHORT(gPugUtil.FixedUnsigned16(Amplitude, BIT(12)));
      WRITE_SHORT(gPugUtil.FixedUnsigned16(Duration, BIT(12)));
      WRITE_SHORT(gPugUtil.FixedUnsigned16(Frequency, BIT(8)));
      MESSAGE_END();
    }
  } else {
    for (int i = 1; i <= gpGlobals->maxClients; ++i) {
      pEntity = INDEXENT(i);

      if (this->IsNetClient(pEntity)) {
        this->ScreenShake(pEntity, Amplitude, Duration, Frequency);
      }
    }
  }
}

void CPugUtil::DrawTracer(Vector Start, Vector End) {
  MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, Start);
  WRITE_BYTE(TE_TRACER);
  WRITE_COORD(Start.x);
  WRITE_COORD(Start.y);
  WRITE_COORD(Start.z);
  WRITE_COORD(End.x);
  WRITE_COORD(End.y);
  WRITE_COORD(End.z);
  MESSAGE_END();
}

void CPugUtil::DrawLine(Vector Start, Vector End, int Life, int Red, int Green,
                        int Blue) {
  MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, Start);
  WRITE_BYTE(TE_LINE);
  WRITE_COORD(Start.x);
  WRITE_COORD(Start.y);
  WRITE_COORD(Start.z);
  WRITE_COORD(End.x);
  WRITE_COORD(End.y);
  WRITE_COORD(End.z);
  WRITE_SHORT(Life);
  WRITE_BYTE(Red);
  WRITE_BYTE(Green);
  WRITE_BYTE(Blue);
  MESSAGE_END();
}

std::vector<CBasePlayer *> CPugUtil::GetPlayers(bool InGame, bool Bots) {
  std::vector<CBasePlayer *> Players = {};

  for (int i = 1; i <= gpGlobals->maxClients; ++i) {
    auto Player = UTIL_PlayerByIndexSafe(i);

    if (Player) {
      if (!Player->IsDormant()) {
        if ((Player->edict()->v.flags & FL_PROXY) == FL_PROXY) {
          continue;
        }

        if (InGame) {
          if ((Player->m_iTeam != TERRORIST) && (Player->m_iTeam != CT)) {
            continue;
          }
        }

        if (Bots == false) {
          if (Player->IsBot()) {
            continue;
          }
        }

        Players.push_back(Player);
      }
    }
  }

  return Players;
}

std::array<std::vector<CBasePlayer *>, SPECTATOR + 1U> CPugUtil::GetPlayers() {
  std::array<std::vector<CBasePlayer *>, SPECTATOR + 1U> Players;

  for (int i = 1; i <= gpGlobals->maxClients; ++i) {
    auto Player = UTIL_PlayerByIndexSafe(i);

    if (Player) {
      if (!Player->IsDormant()) {
        Players[Player->m_iTeam].push_back(Player);
      }
    }
  }

  return Players;
}

void CPugUtil::ShowMotd(edict_t *pEntity, char *Motd, unsigned int MotdLength) {
  static int iMsgMOTD;

  if (iMsgMOTD || (iMsgMOTD = GET_USER_MSG_ID(PLID, "MOTD", NULL))) {
    if (MotdLength < 128) {
      struct stat FileBuffer;

      if (stat(Motd, &FileBuffer) == 0) {
        int FileLength = 0;

        char *FileContent =
            reinterpret_cast<char *>(LOAD_FILE_FOR_ME(Motd, &FileLength));

        if (FileLength) {
          this->ShowMotd(pEntity, FileContent, FileLength);
        }

        FREE_FILE(FileContent);

        return;
      }
    }

    char *Buffer = Motd;

    char Character = 0;

    int Size = 0;

    while (*Buffer) {
      Size = MotdLength;

      if (Size > 175) {
        Size = 175;
      }

      MotdLength -= Size;

      Character = *(Buffer += Size);

      *Buffer = 0;

      MESSAGE_BEGIN(MSG_ONE, iMsgMOTD, NULL, pEntity);
      WRITE_BYTE(Character ? FALSE : TRUE);
      WRITE_STRING(Motd);
      MESSAGE_END();

      *Buffer = Character;

      Motd = Buffer;
    }
  }
}

bool CPugUtil::IsPlayerVisible(CBasePlayer *Player, CBasePlayer *Target) {
  if (Player && Target) {
    edict_t *pEntity = Player->edict();
    edict_t *pTarget = Target->edict();

    if (pEntity->v.flags & FL_NOTARGET) {
      return false;
    }

    Vector vLooker = (pEntity->v.origin + pEntity->v.view_ofs);
    Vector vTarget = (pTarget->v.origin + pTarget->v.view_ofs);

    TraceResult tr;

    auto oldSolid = pTarget->v.solid;

    pTarget->v.solid = SOLID_NOT;

    TRACE_LINE(vLooker, vTarget, FALSE, pEntity, &tr);

    pTarget->v.solid = oldSolid;

    if (tr.fInOpen && tr.fInWater) {
      return false;
    } else if ((tr.flFraction == 1.0) ||
               (ENTINDEX(tr.pHit) == Target->entindex())) {
      return true;
    }
  }

  return false;
}