#pragma once
#include "../kz.h"
#include "../timer/kz_timer.h"

class KZHUDServiceTimerEventListener : public KZTimerServiceEventListener
{
	virtual void OnTimerStopped(KZPlayer *player) override;
	virtual void OnTimerEndPost(KZPlayer *player, const char *courseName, f32 time, u32 teleportsUsed) override;
};

#define KZ_HUD_TIMER_STOPPED_GRACE_TIME 3.0f

class KZHUDService : public KZBaseService
{
	using KZBaseService::KZBaseService;

private:
	bool showPanel {};
	f64 timerStoppedTime {};
	f64 currentTimeWhenTimerStopped {};

public:
	virtual void Reset() override;
	static_global void Init();
	static_global void RegisterCommands();
	void DrawPanels(KZPlayer *target);

	void TogglePanel();

	bool IsShowingPanel()
	{
		return this->showPanel;
	}

	void OnTimerStopped(f64 currentTimeWhenTimerStopped);

	bool ShouldShowTimerAfterStop()
	{
		return g_pKZUtils->GetServerGlobals()->curtime > KZ_HUD_TIMER_STOPPED_GRACE_TIME
			   && g_pKZUtils->GetServerGlobals()->curtime - timerStoppedTime < KZ_HUD_TIMER_STOPPED_GRACE_TIME;
	}

private:
	std::string GetSpeedText(const char *language = KZ_DEFAULT_LANGUAGE);
	std::string GetKeyText(const char *language = KZ_DEFAULT_LANGUAGE);
	std::string GetCheckpointText(const char *language = KZ_DEFAULT_LANGUAGE);
	std::string GetTimerText(const char *language = KZ_DEFAULT_LANGUAGE);
};
