#include "precompiled.h"

CPugRestrictItem gPugRestrictItem;

bool CPugRestrictItem::HasRestrictItem(CBasePlayer *Player, ItemID Item, ItemRestType Type)
{
	auto State = gPugMod.GetState();

	if (State != STATE_DEAD)
	{
		if (Item >= ITEM_SHIELDGUN && Item <= ITEM_BATTERY)
		{
			auto Restricted = false;

			if (gPugCvar.m_RestrictItem->string)
			{
				// Slots beyond the end of the string are not restricted
				if (static_cast<size_t>(Item) < strlen(gPugCvar.m_RestrictItem->string))
				{
					Restricted = (gPugCvar.m_RestrictItem->string[Item] == '1');
				}
			}

			if ((State == STATE_KNIFE_ROUND) && (Item != ITEM_KEVLAR) && (Item != ITEM_ASSAULT) && (Item != ITEM_KNIFE))
			{
				Restricted = true;
			}

			if (Restricted)
			{
				// Only warn when buying; touching and spawn equipment are checked every frame / spawn
				if (Type == ITEM_TYPE_BUYING)
				{
					gPugUtil.ClientPrint(Player->edict(), E_PRINT::CENTER, "#Cstrike_TitlesTXT_Weapon_Not_Available");
				}

				return true;
			}
		}
	}

	return false;
}