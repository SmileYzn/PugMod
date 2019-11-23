#include <amxmodx>
#include <fakemeta>

#include <PugCore>

new g_FM_Voice;

public plugin_init()
{
	register_plugin("Pug Mod (Dead Talk)",PUG_VERSION,PUG_AUTHOR);
	
	hook_cvar_change(get_cvar_pointer("sv_alltalk"),"OnChange");
}

public OnChange(pCvar,const Old[],const New[])
{
	if(New[0] == '0')
	{
		g_FM_Voice = register_forward(FM_Voice_SetClientListening,"FMVoiceSetClientListening",false);
	}
	else
	{
		unregister_forward(FM_Voice_SetClientListening,g_FM_Voice,false);
	}
}

public FMVoiceSetClientListening(Recv,Sender,bool:Listen)
{
	if(Recv != Sender)
	{
		if(is_user_connected(Recv) && is_user_connected(Sender))
		{
			if(get_ent_data(Recv,"CBasePlayer","m_iTeam") == get_ent_data(Sender,"CBasePlayer","m_iTeam"))
			{
				engfunc(EngFunc_SetClientListening,Recv,Sender,true);
				return FMRES_SUPERCEDE;
			}
		}
	}
	
	return FMRES_IGNORED;
}