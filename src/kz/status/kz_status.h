#pragma once

#include "../kz.h"

#define KZ_STATUS_INTERFACE "KZStatusInterface"

struct KZPlayerStatus
{
	// Identity
	bool connected;
	bool inGame;
	bool fakeClient;
	u64 steamId64;
	char name[128];

	// KZ timer
	bool timerRunning;
	bool paused;
	bool validTime;
	f64 currentTime; // seconds elapsed
	u32 teleportCount;
	u32 checkpointCount;

	// Course
	char courseName[65];
	i32 courseId;

	// Mode / Style
	char modeName[128];
	char modeShortName[32];

	static constexpr i32 MAX_STYLES = 16;
	i32 styleCount;
	char styleNames[MAX_STYLES][128];

	// Position / Velocity (world space)
	f32 origin[3];
	f32 velocity[3];
	f32 angles[3]; // pitch, yaw, roll
};

class KZStatusEventListener
{
public:
	virtual ~KZStatusEventListener() = default;

	virtual void OnTimerStart(i32 playerSlot, const char *courseName, i32 courseId) {}
	virtual void OnTimerStartPost(i32 playerSlot, const char *courseName, i32 courseId) {}

	virtual void OnTimerEnd(i32 playerSlot, const char *courseName, i32 courseId, f64 time, u32 teleportsUsed) {}
	virtual void OnTimerEndPost(i32 playerSlot, const char *courseName, i32 courseId, f64 time, u32 teleportsUsed) {}

	virtual void OnTimerStopped(i32 playerSlot, const char *courseName, i32 courseId) {}

	virtual void OnTimerInvalidated(i32 playerSlot) {}

	virtual void OnPause(i32 playerSlot) {}
	virtual void OnPausePost(i32 playerSlot) {}

	virtual void OnResume(i32 playerSlot) {}
	virtual void OnResumePost(i32 playerSlot) {}

};

class KZStatusInterface
{
public:
	virtual bool GetPlayerStatus(i32 playerSlot, KZPlayerStatus &out) = 0;

	virtual i32 GetAllPlayerStatus(KZPlayerStatus *out, i32 maxCount) = 0;

	virtual bool RegisterEventListener(KZStatusEventListener *listener) = 0;
	virtual bool UnregisterEventListener(KZStatusEventListener *listener) = 0;
};

extern KZStatusInterface *g_pKZStatus;

namespace KZ::status
{
	void Init();
	void Cleanup();
} // namespace KZ::status
