#include <PugCore>
#include <PugStocks>

public plugin_init()
{
	register_plugin("Pug Mod (Admin)",PUG_MOD_VERSION,PUG_MOD_AUTHOR);
	
	register_dictionary("PugAdmin.txt");
	
	PUG_RegCommand("kick","PUG_Kick",ADMIN_LEVEL_A,"PUG_DESC_KICK");
	PUG_RegCommand("kill","PUG_Kill",ADMIN_LEVEL_A,"PUG_DESC_KILL");
	PUG_RegCommand("rcon","PUG_Rcon",ADMIN_LEVEL_A,"PUG_DESC_RCON");
	PUG_RegCommand("map","PUG_Map",ADMIN_LEVEL_A,"PUG_DESC_MAP");
	PUG_RegCommand("msg","PUG_Msg",ADMIN_LEVEL_A,"PUG_DESC_MSG");
	PUG_RegCommand("ban","PUG_Ban",ADMIN_LEVEL_A,"PUG_DESC_BAN");
}

public plugin_cfg()
{
	remove_user_flags(0,read_flags("z"));
	
	new szPath[PLATFORM_MAX_PATH];
	PUG_GetFilePath("admin.rc",szPath,charsmax(szPath));
	
	if(file_exists(szPath))
	{
		new SMCParser:hHandle = SMC_CreateParser();
		
		if(hHandle != Invalid_SMCParser)
		{
			SMC_SetReaders(hHandle,"SMC_OnKeyValue");
			SMC_ParseFile(hHandle,szPath);
		}
		
		SMC_DestroyParser(hHandle);
	}
}

public SMCResult:SMC_OnKeyValue(SMCParser:hHandle,const szAuth[],const szFlags[])
{
	admins_push(szAuth,"",read_flags(szFlags),FLAG_AUTHID|FLAG_NOPASS);	
	return SMCParse_Continue;
}

public client_authorized(id)
{
	new szAuth[2][MAX_AUTHID_LENGTH];
	get_user_authid(id,szAuth[0],charsmax(szAuth[]));
	
	for(new i;i < admins_num();i++)
	{
		admins_lookup(i,AdminProp_Auth,szAuth[1],charsmax(szAuth[]));
		
		if(equali(szAuth[0],szAuth[1]))
		{
			set_user_flags(id,admins_lookup(i,AdminProp_Access));
			return PLUGIN_CONTINUE;
		}
	}
	
	set_user_flags(id,ADMIN_USER);	
	return PLUGIN_CONTINUE;
}


public PUG_Kick(id,iLevel)
{
	if(access(id,iLevel))
	{
		new szName[MAX_NAME_LENGTH];
		read_argv(1,szName,charsmax(szName));
		
		new iPlayer = cmd_target(id,szName,CMDTARGET_OBEY_IMMUNITY);
		
		if(iPlayer)
		{		
			new szReason[32];
			read_argv(2,szReason,charsmax(szReason));
			remove_quotes(szReason);
			
			server_cmd("kick #%i ^"%s^"",get_user_userid(iPlayer),szReason);
		}
		
		PUG_CommandClient(id,"!kick","PUG_KICK",iPlayer,iPlayer);
	}
	
	return PLUGIN_HANDLED;
}

public PUG_Kill(id,iLevel)
{
	if(access(id,iLevel))
	{
		new szName[MAX_NAME_LENGTH];
		read_argv(1,szName,charsmax(szName));
		
		new iPlayer = cmd_target(id,szName,CMDTARGET_OBEY_IMMUNITY);
		
		if(iPlayer)
		{
			user_kill(iPlayer,1)
		}

		PUG_CommandClient(id,"!kill","PUG_KILL",iPlayer,iPlayer);
	}
	
	return PLUGIN_HANDLED;
}

public PUG_Rcon(id,iLevel)
{
	if(access(id,iLevel))
	{
		new szCommand[256];
		read_args(szCommand,charsmax(szCommand));
		remove_quotes(szCommand);
		
		if(szCommand[0])
		{
			server_cmd(szCommand);
		}
		
		PUG_ExecuteCommand(id,"!rcon","PUG_RCON",szCommand[0]);
	}
	
	return PLUGIN_HANDLED;
}

public PUG_Map(id,iLevel)
{
	if(access(id,iLevel))
	{
		new szMap[MAX_NAME_LENGTH];
		read_args(szMap,charsmax(szMap));
		remove_quotes(szMap);
		
		new bIsMapValid = is_map_valid(szMap);
		
		if(bIsMapValid)
		{
			server_cmd("changelevel %s",szMap);
		}
		
		PUG_ExecuteCommand(id,"!map","PUG_MAP",bIsMapValid);
	}
	
	return PLUGIN_HANDLED;
}

public PUG_Msg(id,iLevel)
{
	if(access(id,iLevel))
	{
		new szMessage[256];
		read_args(szMessage,charsmax(szMessage));
		remove_quotes(szMessage);
		
		if(szMessage[0])
		{
			new szName[MAX_NAME_LENGTH];
			get_user_name(id,szName,charsmax(szName));
			
			client_print_color(0,id,"%s (^3%s^1) %s",PUG_MOD_HEADER,szName,szMessage);
		}
	}
	
	return PLUGIN_HANDLED;
}

public PUG_Ban(id,iLevel)
{
	if(access(id,iLevel))
	{
		new szName[MAX_NAME_LENGTH];
		read_argv(1,szName,charsmax(szName));
		
		new iPlayer = cmd_target(id,szName,CMDTARGET_OBEY_IMMUNITY);
		
		if(iPlayer)
		{
			new iTime = read_argv_int(2);
			
			server_cmd("banid %i #%i kick;writeid",iTime,get_user_userid(iPlayer));
		}
		
		PUG_CommandClient(id,"!ban","PUG_BAN",iPlayer,iPlayer);
	}
	
	return PLUGIN_HANDLED;
}
