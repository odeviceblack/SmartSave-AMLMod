#include "AutoSave.hpp"
#include "../main.hpp"

AutoSaveManager* AutoSaveManager::instance = nullptr;

float AutoSaveManager::GetMonotonicSeconds() const
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (float)t.tv_sec + (float)t.tv_nsec * 1e-9f;
}

AutoSaveManager* AutoSaveManager::getInstance()
{
	return instance;
}

void AutoSaveManager::Initialize(int optionIndex)
{
	instance = this;
	intervalOption = optionIndex;
	lastSaveTime = GetMonotonicSeconds();

	SETSYM_TO(pcSaveHelper, g_libGTASA, "PcSaveHelper");
	SETSYM_TO(fnSaveSlot,   g_libGTASA, "_ZN8C_PcSave8SaveSlotEab");

	if(!pcSaveHelper || !fnSaveSlot)
	{
		logger->Error("Failed to resolve save symbols!");
		return;
	}

	InstallHooks();
	logger->Info("AutoSave active: %d seconds.", kValues[intervalOption]);
}

DECL_HOOKv(OnRender2D)
{
	OnRender2D();

	AutoSaveManager* inst = AutoSaveManager::getInstance();
	if(!inst) return;

	int interval = AutoSaveManager::kValues[inst->getInterval()];
	if(interval <= 0) return;

	float now = inst->getNow();
	float dt = now - inst->getLastSaveTime();

	if(dt >= interval)
	{
		inst->resetLastSaveTime(now);
		inst->doSave();
	}
}

DECL_HOOKv(OnMenuUpdate, void* self, float delta)
{
	OnMenuUpdate(self, delta);

	AutoSaveManager* inst = AutoSaveManager::getInstance();
	if(!inst) return;

	static bool added = false;
	if(added) return;
	added = true;

	g_saUtils->AddClickableItem(
		SetType_Mods,
		"AutoSave Interval",
		inst->getInterval(),
		0, 7,
		(const char**)AutoSaveManager::kLabels,
		&AutoSaveManager::OnSettingChanged
	);
}

void AutoSaveManager::InstallHooks()
{
	HOOKSYM(OnRender2D,		g_libGTASA,	"_Z13Render2dStuffv");
	HOOKSYM(OnMenuUpdate,	g_libGTASA,	"_ZN14MainMenuScreen6UpdateEf");
}

void AutoSaveManager::PerformSave()
{
	if(fnSaveSlot)
	{
		fnSaveSlot(pcSaveHelper, 8, true); // GTASAsf9.b (default autosave slot)
		logger->Info("AutoSave executed (%d s).", kValues[intervalOption]);
	}
}

void AutoSaveManager::OnSettingChanged(int oldIndex, int newIndex, void* pData)
{
	if(!instance) return;

	instance->intervalOption = newIndex;
	g_cfgInterval->SetInt(newIndex);
	cfg->Save();

	instance->lastSaveTime = instance->GetMonotonicSeconds();
	logger->Info("New interval: %d seconds.", kValues[newIndex]);
}
