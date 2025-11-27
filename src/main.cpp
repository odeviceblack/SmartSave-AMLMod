#include "main.hpp"
#include "Game/AutoSave.hpp"

MYMODCFGNAME(com.deviceblack.gtasa.SmartSave, SmartSave, 1.0, DeviceBlack, SmartSave)
NEEDGAME(com.rockstargames.gtasa)

void*			g_libGTASA		= nullptr;
ConfigEntry*	g_cfgInterval	= nullptr;
ISAUtils*		g_saUtils		= nullptr;

static AutoSaveManager g_autoSave;

ON_MOD_PRELOAD()
{
	logger->SetTag("SmartSave");

#ifdef LOG_IN_FILE
	logger->SetFile(aml->GetConfigPath(), "../mods/SmartSave.txt");
	logger->EnableFileLogging(true);
#endif

	g_libGTASA = aml->GetLibHandle("libGTASA.so");
	logger->Info("GTASA Handle: 0x%" PRIXPTR, (uintptr_t)g_libGTASA);
}

ON_MOD_LOAD()
{
	if(!g_libGTASA)
	{
		logger->Error("Error loading libGTASA!");
		return;
	}

	g_saUtils = (ISAUtils*)GetInterface("SAUtils");

	g_cfgInterval = cfg->Bind("SaveInterval", 2, "SmartSave");
	int initialValue = g_cfgInterval->GetInt();

	g_autoSave.Initialize(clampint(0, 7, initialValue));
}
