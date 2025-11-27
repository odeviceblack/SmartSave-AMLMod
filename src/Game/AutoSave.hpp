#pragma once
#include <time.h>

class AutoSaveManager
{
public:
	void Initialize(int optionIndex);
	static AutoSaveManager* getInstance();

	int  getInterval() const { return intervalOption; }
	void setInterval(int v)  { intervalOption = v; }

	float getLastSaveTime() const { return lastSaveTime; }
	void  resetLastSaveTime(float v) { lastSaveTime = v; }

	float getNow() const { return GetMonotonicSeconds(); }

	void doSave() { PerformSave(); }

	static constexpr int kValues[8] = {
		0,   10,  30,  60,
		120, 180, 240, 300
	};

	static constexpr const char* kLabels[8] = {
		"off", "10s",
		"30s", "1m",
		"2m", "3m",
		"4m", "5m"
	};

	static void OnSettingChanged(int oldValue, int newValue, void* pData);

private:
	int intervalOption = 0;

	void* pcSaveHelper = nullptr;
	void (*fnSaveSlot)(void*, int8_t, bool) = nullptr;

	float GetMonotonicSeconds() const;
	float lastSaveTime = 0.0f;

	void InstallHooks();
	void PerformSave();

	static AutoSaveManager* instance;
};
