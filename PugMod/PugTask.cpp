#include "precompiled.h"

CPugTask gPugTask;

void CPugTask::ServerActivate()
{
	this->m_Task.clear();
}

void CPugTask::ServerDeactivate()
{
	this->m_Task.clear();
}

void CPugTask::Create(int Index, float Delay, bool Loop, int Parameter)
{
	this->m_Task[Index] = {Delay, (gpGlobals->time + Delay), Loop, Parameter};
}

void CPugTask::Remove(int Index)
{
	if (this->m_Task.find(Index) != this->m_Task.end())
	{
		this->m_Task[Index] = {0.0f, 0.0f, false, 0};
	}
}

void CPugTask::Execute(int Index)
{
	if (this->m_Task.find(Index) != this->m_Task.end())
	{
		switch (Index)
		{
			case E_TASK::SET_STATE:
			{
				gPugMod.SetState(this->m_Task[Index].Parameter);
				break;
			}
			case E_TASK::MAP_CHANGE:
			{
				gPugMapList.ChangeMap(this->m_Task[Index].Parameter);
				break;
			}
			case E_TASK::LEADER_MENU:
			{
				gPugLeader.GetRandomPlayer(this->m_Task[Index].Parameter);
				break;
			}
			case E_TASK::VOTE_SWAP_TEAM:
			{
				gPugVoteSwapTeam.Init(this->m_Task[Index].Parameter);
				break;
			}
			case E_TASK::LO3_RESTART:
			{
				gPugLO3.Init(this->m_Task[Index].Parameter);
				break;
			}
			case E_TASK::VOTE_OVERTIME:
			{
				gPugVoteOvertime.Init();
				break;
			}
			case E_TASK::ROUND_MESSAGE:
			{
				gPugMod.RoundMessage();
				break;
			}
			case E_TASK::PAUSE_MATCH:
			{
				gPugPause.Timer(this->m_Task[Index].Parameter);
				break;
			}
			case E_TASK::RAGE_BAN_CHECK:
			{
				gPugRageBan.CheckList();
				break;
			}
		}
	}
}

void CPugTask::StartFrame()
{
	if (!this->m_Task.empty())
	{
		for (auto Task = this->m_Task.begin(); Task != this->m_Task.end(); Task++)
		{
			if  ((Task->second.Time > 0.0f) && (gpGlobals->time >= Task->second.Time))
			{
				if (Task->second.Loop)
				{
					Task->second.Time = (gpGlobals->time + Task->second.Delay);
				}
				else
				{
					Task->second.Time = 0.0f;
				}

				this->Execute(Task->first);
			}
		}
	}
}