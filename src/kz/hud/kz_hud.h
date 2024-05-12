#pragma once
#include "../kz.h"
#include "../timer/kz_timer.h"

#define KZ_HUD_TIMER_STOPPED_GRACE_TIME 3.0f

class KZHUDService : public KZBaseService
{
	using KZBaseService::KZBaseService;

private:
	bool showPanel {};

public:
	static_global void RegisterCommands();
	void DrawPanels(KZPlayer *target);

	void TogglePanel();

	bool IsShowingPanel()
	{
		return this->showPanel;
	}

private:
	std::string GetSpeedText(const char *language = KZ_DEFAULT_LANGUAGE);
	std::string GetKeyText(const char *language = KZ_DEFAULT_LANGUAGE);
};
