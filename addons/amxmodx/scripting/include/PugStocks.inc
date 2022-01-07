#if defined _PugStocks_included
	#endinput
#endif
#define _PugStocks_included

/**
 * Get Pug Mod configuration directory
 *
 * @note This is useful for get configs directory based on Amx Mod X.
 *
 * @param szPatch	Buffer to copy patch to
 * @param iLen		Maximum buffer size
 *
 * @noreturn
 */
stock PUG_GetFilePath(const szFile[],szPath[],iLen)
{
	get_localinfo("amxx_configsdir",szPath,iLen);
	formatex(szPath,iLen,"%s/pug/%s",szPath,szFile);
}

/**
 * Register new client command
 *
 * @note This is useful for register command using translation system,
 * 	 If command have admin flags, will be registred as !%s, if not as .%s
 *
 * @param szCommand		Command to register
 * @param szFunction		Function callback
 * @param iFlags		Admin flags needed
 * @param szInfo		Information string
 * @param bTranslate		Translate information string
 *
 * @noreturn
 */
stock PUG_RegCommand(const szCommand[],const szFunction[],iFlags = ADMIN_ALL,const szInfo[] = "",bool:bTranslate = true)
{
	return register_clcmd(fmt((iFlags == ADMIN_ALL ? ".%s" : "!%s"),szCommand),szFunction,iFlags,szInfo,-1,bTranslate);
}

/**
 * Execute a PUG function with client as parameter and display a result in chat
 *
 * @note This is only for execute functions that return an value
 *
 * @param id			Caller index (Usually an admin)
 * @param szLang		Lang string to translate
 * @param iClient		Target client index
 * @param bCheck		The function to call or it's result
 *
 * @noreturn
 */
stock PUG_CommandClient(id,szLang[],iClient,bCheck)
{
	new szCommand[32];
	read_argv(0,szCommand,charsmax(szCommand));
	
	if(bCheck)
	{
		client_print_color(0,iClient,"%s %L",PUG_MOD_HEADER,LANG_SERVER,szLang,iClient);
	}
	
	client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_PLAYER,bCheck ? "PUG_CMD_OK" : "PUG_CMD_FAIL",szCommand);
}

/**
 * Execute a PUG function and display a result in chat
 *
 * @note This is only for execute functions that return an value
 *
 * @param id			Caller index (Usually an admin)
 * @param szLang		Lang string in language system
 * @param bCheck		The function to call or it's result
 *
 * @noreturn
 */
stock PUG_ExecuteCommand(id,szLang[],bCheck)
{
	new szCommand[32];
	read_argv(0,szCommand,charsmax(szCommand));
	
	if(bCheck)
	{
		client_print_color(0,id,"%s %L",PUG_MOD_HEADER,LANG_SERVER,szLang,id,szCommand);
	}
	
	client_print_color(id,id,"%s %L",PUG_MOD_HEADER,LANG_PLAYER,bCheck ? "PUG_CMD_OK" : "PUG_CMD_FAIL",szCommand);
}

/**
 * Get Pug Map List
 *
 * @note This is used to build maps menu for votes
 *
 * @param szData		2D data array returned from function
 * @param iLen			Size of Data Array
 *
 * @return              Maps Loaded count
 */
stock PUG_GetMapList(szData[][],iLen)
{
	new szPath[PLATFORM_MAX_PATH];
	PUG_GetFilePath("maps.rc",szPath,charsmax(szPath));
	
	if(file_exists(szPath))
	{
		new iFile = fopen(szPath,"rb");
		new szLine[MAX_NAME_LENGTH],iCount;
	
		while(!feof(iFile) && (iCount < iLen))
		{
			fgets(iFile,szLine,charsmax(szLine));
			trim(szLine);
			
			if(is_map_valid(szLine) && !equali(MapName,szLine))
			{
				copy(szData[iCount++],iLen,szLine);
			}
		}
		
		fclose(iFile);

		return iCount;
	}
	
	return 0;
}

/**
 * Build the help file (.html) for commands at config dir
 *
 * @note This is used to automatically build a file with registred commands
 *
 * @param iLevel			Admin level constant (See amxconst for flags)
 *
 * @noreturn
 */
stock PUG_BuildHelpFile(iLevel)
{
	new szPath[PLATFORM_MAX_PATH];	
	PUG_GetFilePath((iLevel == ADMIN_ALL) ? "help.htm" : "admin.htm",szPath,charsmax(szPath));
	
	if(!file_exists(szPath))
	{
		new iCommands = get_concmdsnum(iLevel);
		
		if(iCommands)
		{
			new iFile = fopen(szPath,"wt");

			fputs(iFile,"<style type='text/css'>body{background:#000;margin:2px;color:#FFB000;font-family:Tahoma}</style><table width='100%%'>");
			
			new szName[MAX_NAME_LENGTH],szInfo[128];

			new iFlag,bool:bTranslate;
			
			for(new iCommand;iCommand < iCommands;iCommand++)
			{
				get_concmd(iCommand,szName,charsmax(szName),iFlag,szInfo,charsmax(szInfo),iLevel,-1,bTranslate);

				if(szInfo[0] && (szName[0] == ((iLevel == ADMIN_ALL) ? '.' : '!')))
				{
					if(bTranslate)
					{
						format(szInfo,charsmax(szInfo),"%L",LANG_SERVER,szInfo);
					}

					replace_string(szInfo,sizeof(szInfo),"<","&#60;");
					replace_string(szInfo,sizeof(szInfo),">","&#62;");

					fprintf(iFile,"<tr><td>%s</td><td>%s</td></tr>",szName,szInfo);
				}
			}

			fclose(iFile);
		}
	}
}

/**
 * Build the pugmod.rc file (.rc) for cvars in PUG config dir
 *
 * @note This is used to automatically build a file with registred cvars
 *
 * @param Name			Name of the file to check or build
 * @param Help			Build file as html to help admins
 * @noreturn
 */
stock PUG_BuildCvarsFile(szFile[],bool:bHelp)
{
	new szPath[PLATFORM_MAX_PATH];	
	PUG_GetFilePath(szFile,szPath,charsmax(szPath));
	
	if(!file_exists(szPath))
	{
		new iNum = get_plugins_cvarsnum();
		
		if(iNum)
		{
			new iFile = fopen(szPath,"wt");
			
			if(bHelp)
			{
				fputs(iFile,"<style type='text/css'>body{background:#000;margin:2px;color:#FFB000;font-family:Tahoma}</style><table width='100%%'>");
			}
			
			new szName[MAX_NAME_LENGTH],iFlag,iPlugin,iHandle,szInfo[128],szValue[MAX_NAME_LENGTH];
			
			for(new id;id < iNum;id++)
			{
				if(get_plugins_cvar(id,szName,charsmax(szName),iFlag,iPlugin,iHandle,szInfo,charsmax(szInfo)))
				{
					if((szName[0] == 'p') && (szName[1] == 'u') && (szName[2] == 'g'))
					{
						if(bHelp)
						{
							replace_string(szInfo,sizeof(szInfo),"<","&#60;");
							replace_string(szInfo,sizeof(szInfo),">","&#62;");
							
							fprintf(iFile,"<tr><td>%s</td><td>%s</td></tr>",szName,szInfo);
						}
						else
						{
							get_pcvar_string(iHandle,szValue,charsmax(szValue));
							
							fprintf(iFile,"%s		^"%s^"			// %s^n",szName,szValue,szInfo);
						}
					}
				}
			}
			
			fclose(iFile);
		}
	}
}
