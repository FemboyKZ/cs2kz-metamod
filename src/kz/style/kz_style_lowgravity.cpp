#include "kz_style_lowgravity.h"

#include "utils/addresses.h"
#include "utils/interfaces.h"
#include "utils/gameconfig.h"
#include "version.h"

KZLowGravityStylePlugin g_KZLowGravityStylePlugin;

CGameConfig *g_pGameConfig = NULL;
KZUtils *g_pKZUtils = NULL;
KZStyleManager *g_pStyleManager = NULL;
StyleServiceFactory g_StyleFactory = [](KZPlayer *player) -> KZStyleService * { return new KZLowGravityStyleService(player); };
PLUGIN_EXPOSE(KZLowGravityStylePlugin, g_KZLowGravityStylePlugin);

bool KZLowGravityStylePlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();
	// Load mode
	int success;
	g_pStyleManager = (KZStyleManager *)g_SMAPI->MetaFactory(KZ_STYLE_MANAGER_INTERFACE, &success, 0);
	if (success == META_IFACE_FAILED)
	{
		V_snprintf(error, maxlen, "Failed to find %s interface", KZ_STYLE_MANAGER_INTERFACE);
		return false;
	}
	g_pKZUtils = (KZUtils *)g_SMAPI->MetaFactory(KZ_UTILS_INTERFACE, &success, 0);
	if (success == META_IFACE_FAILED)
	{
		V_snprintf(error, maxlen, "Failed to find %s interface", KZ_UTILS_INTERFACE);
		return false;
	}
	modules::Initialize();
	if (!interfaces::Initialize(ismm, error, maxlen))
	{
		V_snprintf(error, maxlen, "Failed to initialize interfaces");
		return false;
	}

	if (nullptr == (g_pGameConfig = g_pKZUtils->GetGameConfig()))
	{
		V_snprintf(error, maxlen, "Failed to get game config");
		return false;
	}

	if (!g_pStyleManager->RegisterStyle(g_PLID, STYLE_NAME_SHORT, STYLE_NAME, g_StyleFactory))
	{
		V_snprintf(error, maxlen, "Failed to register style");
		return false;
	}
	ConVar_Register();

	return true;
}

bool KZLowGravityStylePlugin::Unload(char *error, size_t maxlen)
{
	g_pStyleManager->UnregisterStyle(g_PLID);
	return true;
}

bool KZLowGravityStylePlugin::Pause(char *error, size_t maxlen)
{
	g_pStyleManager->UnregisterStyle(g_PLID);
	return true;
}

bool KZLowGravityStylePlugin::Unpause(char *error, size_t maxlen)
{
	if (!g_pStyleManager->RegisterStyle(g_PLID, STYLE_NAME_SHORT, STYLE_NAME, g_StyleFactory))
	{
		return false;
	}
	return true;
}

const char *KZLowGravityStylePlugin::GetLicense()
{
	return "GPLv3";
}

const char *KZLowGravityStylePlugin::GetVersion()
{
	return VERSION_STRING;
}

const char *KZLowGravityStylePlugin::GetDate()
{
	return __DATE__;
}

const char *KZLowGravityStylePlugin::GetLogTag()
{
	return "KZ-Style-LowGravity";
}

const char *KZLowGravityStylePlugin::GetAuthor()
{
	return "zer0.k, jvnipers";
}

const char *KZLowGravityStylePlugin::GetDescription()
{
	return "LowGravity style plugin for CS2KZ";
}

const char *KZLowGravityStylePlugin::GetName()
{
	return "CS2KZ-Style-LowGravity";
}

const char *KZLowGravityStylePlugin::GetURL()
{
	return "https://github.com/KZGlobalTeam/cs2kz-metamod";
}

CGameEntitySystem *GameEntitySystem()
{
	return g_pKZUtils->GetGameEntitySystem();
}

void KZLowGravityStyleService::Init()
{
	this->player->GetPlayerPawn()->m_flGravityScale(0.5);
}

void KZLowGravityStyleService::Cleanup()
{
	this->player->GetPlayerPawn()->m_flGravityScale(1);
}

void KZLowGravityStyleService::OnProcessMovement()
{
	this->player->GetPlayerPawn()->m_flGravityScale(0.5);
}

