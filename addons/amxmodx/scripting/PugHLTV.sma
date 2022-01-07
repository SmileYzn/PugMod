#include <PugCore>
#include <sockets>

new g_iHLTV_Port;
new g_szHLTV_Host[64];
new g_szHLTV_Pass[64];
new g_szHLTV_Self[64];
new g_szHLTV_Name[64];

new g_iHLTV_Socket;
new g_szHLTV_Command[128];

public plugin_init()
{
	register_plugin("Pug Mod (HLTV)",PUG_MOD_VERSION,PUG_MOD_AUTHOR,PUG_MOD_LINK,"HLTV Server Control");

	bind_pcvar_num(create_cvar("pug_hltv_port","",FCVAR_NONE,"HLTV Proxy Remote Port"),g_iHLTV_Port);

	bind_pcvar_string(create_cvar("pug_hltv_host","",FCVAR_NONE,"HLTV Proxy Remote Adress"),g_szHLTV_Host,charsmax(g_szHLTV_Host));

	bind_pcvar_string(create_cvar("pug_hltv_pass","",FCVAR_NONE,"HLTV Proxy Rcon Password"),g_szHLTV_Pass,charsmax(g_szHLTV_Pass));

	bind_pcvar_string(create_cvar("pug_hltv_prefix","pug",FCVAR_NONE,"HLTV Proxy Demo Name Prefix"),g_szHLTV_Name,charsmax(g_szHLTV_Name));

	bind_pcvar_string(get_cvar_pointer("net_address"),g_szHLTV_Self,sizeof(g_szHLTV_Self));
}

public client_putinserver(id)
{
	if(is_user_hltv(id))
	{
		RCON_SendCommand("record %s",g_szHLTV_Name);
	}
}

public PUG_Event(iState)
{
	if(iState == STATE_FIRST_HALF)
	{
		RCON_SendCommand("connect %s",g_szHLTV_Self);
	}
	else if(iState == STATE_END)
	{
		RCON_SendCommand("stop");
	}
}

RCON_SendCommand(const szCommand[],any:...)
{
	new iErrorCode;

	g_iHLTV_Socket = socket_open(g_szHLTV_Host,g_iHLTV_Port,SOCKET_UDP,iErrorCode,SOCK_NON_BLOCKING);

	if(!iErrorCode)
	{
		vformat(g_szHLTV_Command,charsmax(g_szHLTV_Command),szCommand,2);

		new szSend[256] = {0};

		formatex(szSend,charsmax(szSend),"%c%c%c%cchallenge rcon",0xFF,0xFF,0xFF,0xFF);

		socket_send2(g_iHLTV_Socket,szSend,charsmax(szSend));

		RequestFrame("RCON_FrameGetChallenge");
	}
	else
	{
		socket_close(g_iHLTV_Socket);
	}
}

public RCON_FrameGetChallenge()
{
	if(socket_is_readable(g_iHLTV_Socket))
	{
		new szData[256];

		socket_recv(g_iHLTV_Socket,szData,charsmax(szData));

		new szNone[64];

		new szCode[32];

		parse(szData,szNone,charsmax(szNone),szNone,charsmax(szNone),szCode,charsmax(szCode));

		formatex(szData,charsmax(szData),"%c%c%c%crcon %s ^"%s^" %s",0xFF,0xFF,0xFF,0xFF,szCode,g_szHLTV_Pass,g_szHLTV_Command);

		socket_send2(g_iHLTV_Socket,szData,charsmax(szData));

		socket_close(g_iHLTV_Socket);
	}
	else
	{
		RequestFrame("RCON_FrameGetChallenge");
	}
}