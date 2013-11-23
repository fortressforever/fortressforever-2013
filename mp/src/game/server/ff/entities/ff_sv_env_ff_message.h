#pragma once

#ifndef FF_SV_ENV_FF_MESSAGE_H
#define FF_SV_ENV_FF_MESSAGE_H


class CFF_SV_EnvFFMessage : public CBaseEntity
{
public:
	DECLARE_CLASS( CFF_SV_EnvFFMessage, CBaseEntity );
	DECLARE_DATADESC();

	CFF_SV_EnvFFMessage()
	{
		m_clrMesssage.r = 200;
		m_clrMesssage.g = 200;
		m_clrMesssage.b = 200;
		m_clrMesssage.a = 255;
	}

private:
	color32 m_clrMesssage;

	string_t m_iszMessageToAll;
	string_t m_iszMessageToActivator;
	string_t m_iszMessageToNonActivator;
	string_t m_iszMessageToActivatorsTeams;
	string_t m_iszMessageToNonActivatorsTeams;
	string_t m_iszMessageToOwnerTeams;
	string_t m_iszMessageToNonOwnerTeams;

	void Input_ShowMsgAll( inputdata_t &inputdata );
	void Input_ShowMsgActivator( inputdata_t &inputdata );
	void Input_ShowMsgNonActivator( inputdata_t &inputdata );
	void Input_ShowMsgActivatorTeams( inputdata_t &inputdata );
	void Input_ShowMsgNonActivatorTeams( inputdata_t &inputdata );
	void Input_ShowMsgOwnerTeams( inputdata_t &inputdata );
	void Input_ShowMsgNonOwnerTeams( inputdata_t &inputdata );

	void Input_ShowMessages( inputdata_t &inputdata );
};


#endif // FF_SV_ENV_FF_MESSAGE_H
