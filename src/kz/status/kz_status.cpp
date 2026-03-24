#include "kz_status.h"
#include "kz/timer/kz_timer.h"
#include "kz/checkpoint/kz_checkpoint.h"
#include "kz/mode/kz_mode.h"
#include "kz/style/kz_style.h"
#include "kz/mappingapi/kz_mappingapi.h"
#include "utils/utils.h"

static_global CUtlVector<KZStatusEventListener *> g_statusEventListeners;

class KZStatusInterfaceImpl : public KZStatusInterface
{
public:
	bool GetPlayerStatus(i32 playerSlot, KZPlayerStatus &out) override;
	i32 GetAllPlayerStatus(KZPlayerStatus *out, i32 maxCount) override;
	bool RegisterEventListener(KZStatusEventListener *listener) override;
	bool UnregisterEventListener(KZStatusEventListener *listener) override;
};

static_global KZStatusInterfaceImpl g_kzStatusImpl;
KZStatusInterface *g_pKZStatus = &g_kzStatusImpl;

static void FillPlayerStatus(KZPlayer *kzPlayer, KZPlayerStatus &out)
{
	memset(&out, 0, sizeof(out));

	out.connected = kzPlayer->IsConnected();
	out.inGame = kzPlayer->IsInGame();
	out.fakeClient = kzPlayer->IsFakeClient();

	if (!out.connected)
	{
		return;
	}

	out.steamId64 = 0;
	CSteamID sid = kzPlayer->GetSteamId(false);
	if (sid.IsValid())
	{
		out.steamId64 = sid.ConvertToUint64();
	}

	V_strncpy(out.name, kzPlayer->GetName(), sizeof(out.name));

	if (!out.inGame || out.fakeClient)
	{
		return;
	}

	if (kzPlayer->timerService)
	{
		out.timerRunning = kzPlayer->timerService->GetTimerRunning();
		out.paused = kzPlayer->timerService->GetPaused();
		out.validTime = kzPlayer->timerService->GetValidTimer();
		out.currentTime = kzPlayer->timerService->GetTime();

		const KZCourseDescriptor *course = kzPlayer->timerService->GetCourse();
		if (course)
		{
			V_strncpy(out.courseName, course->GetName().Get(), sizeof(out.courseName));
			out.courseId = course->id;
		}
	}

	if (kzPlayer->checkpointService)
	{
		out.teleportCount = kzPlayer->checkpointService->GetTeleportCount();
		out.checkpointCount = kzPlayer->checkpointService->GetCheckpointCount();
	}

	if (kzPlayer->modeService)
	{
		V_strncpy(out.modeName, kzPlayer->modeService->GetModeName(), sizeof(out.modeName));
		V_strncpy(out.modeShortName, kzPlayer->modeService->GetModeShortName(), sizeof(out.modeShortName));
	}

	out.styleCount = 0;
	for (i32 s = 0; s < kzPlayer->styleServices.Count() && out.styleCount < KZPlayerStatus::MAX_STYLES; s++)
	{
		if (kzPlayer->styleServices[s])
		{
			V_strncpy(out.styleNames[out.styleCount], kzPlayer->styleServices[s]->GetStyleName(), sizeof(out.styleNames[0]));
			out.styleCount++;
		}
	}

	// Position / velocity / angles
	Vector origin, velocity;
	QAngle angles;
	kzPlayer->GetOrigin(&origin);
	kzPlayer->GetVelocity(&velocity);
	kzPlayer->GetAngles(&angles);

	out.origin[0] = origin.x;
	out.origin[1] = origin.y;
	out.origin[2] = origin.z;
	out.velocity[0] = velocity.x;
	out.velocity[1] = velocity.y;
	out.velocity[2] = velocity.z;
	out.angles[0] = angles.x;
	out.angles[1] = angles.y;
	out.angles[2] = angles.z;
}

bool KZStatusInterfaceImpl::GetPlayerStatus(i32 playerSlot, KZPlayerStatus &out)
{
	if (playerSlot < 0 || playerSlot >= MAXPLAYERS)
	{
		memset(&out, 0, sizeof(out));
		return false;
	}

	// Player manager uses entity indices (slot + 1)
	KZPlayer *kzPlayer = g_pKZPlayerManager->ToKZPlayer(static_cast<Player *>(g_pKZPlayerManager->ToPlayer(CPlayerSlot(playerSlot))));
	if (!kzPlayer || !kzPlayer->IsConnected())
	{
		memset(&out, 0, sizeof(out));
		return false;
	}

	FillPlayerStatus(kzPlayer, out);
	return true;
}

i32 KZStatusInterfaceImpl::GetAllPlayerStatus(KZPlayerStatus *out, i32 maxCount)
{
	i32 written = 0;
	i32 limit = (maxCount < MAXPLAYERS) ? maxCount : MAXPLAYERS;

	for (i32 slot = 0; slot < limit; slot++)
	{
		KZPlayer *kzPlayer = g_pKZPlayerManager->ToKZPlayer(static_cast<Player *>(g_pKZPlayerManager->ToPlayer(CPlayerSlot(slot))));
		if (!kzPlayer || !kzPlayer->IsConnected() || kzPlayer->IsFakeClient())
		{
			continue;
		}

		FillPlayerStatus(kzPlayer, out[written]);
		written++;
	}

	return written;
}

bool KZStatusInterfaceImpl::RegisterEventListener(KZStatusEventListener *listener)
{
	if (g_statusEventListeners.Find(listener) >= 0)
	{
		return false;
	}
	g_statusEventListeners.AddToTail(listener);
	return true;
}

bool KZStatusInterfaceImpl::UnregisterEventListener(KZStatusEventListener *listener)
{
	return g_statusEventListeners.FindAndRemove(listener);
}

static void ResolveCourse(u32 courseGUID, const char *&name, i32 &id)
{
	const KZCourseDescriptor *course = KZ::course::GetCourse(courseGUID);
	name = course ? course->GetName().Get() : "";
	id = course ? course->id : -1;
}

class KZStatusTimerBridge : public KZTimerServiceEventListener
{
public:
	bool OnTimerStart(KZPlayer *player, u32 courseGUID) override
	{
		const char *name;
		i32 id;
		ResolveCourse(courseGUID, name, id);
		i32 slot = player->GetPlayerSlot().Get();
		CALL_FORWARD(g_statusEventListeners, OnTimerStart, slot, name, id);
		return true;
	}

	void OnTimerStartPost(KZPlayer *player, u32 courseGUID) override
	{
		const char *name;
		i32 id;
		ResolveCourse(courseGUID, name, id);
		i32 slot = player->GetPlayerSlot().Get();
		CALL_FORWARD(g_statusEventListeners, OnTimerStartPost, slot, name, id);
	}

	bool OnTimerEnd(KZPlayer *player, u32 courseGUID, f32 time, u32 teleportsUsed) override
	{
		const char *name;
		i32 id;
		ResolveCourse(courseGUID, name, id);
		i32 slot = player->GetPlayerSlot().Get();
		CALL_FORWARD(g_statusEventListeners, OnTimerEnd, slot, name, id, (f64)time, teleportsUsed);
		return true;
	}

	void OnTimerEndPost(KZPlayer *player, u32 courseGUID, f32 time, u32 teleportsUsed) override
	{
		const char *name;
		i32 id;
		ResolveCourse(courseGUID, name, id);
		i32 slot = player->GetPlayerSlot().Get();
		CALL_FORWARD(g_statusEventListeners, OnTimerEndPost, slot, name, id, (f64)time, teleportsUsed);
	}

	void OnTimerStopped(KZPlayer *player, u32 courseGUID) override
	{
		const char *name;
		i32 id;
		ResolveCourse(courseGUID, name, id);
		i32 slot = player->GetPlayerSlot().Get();
		CALL_FORWARD(g_statusEventListeners, OnTimerStopped, slot, name, id);
	}

	void OnTimerInvalidated(KZPlayer *player) override
	{
		i32 slot = player->GetPlayerSlot().Get();
		CALL_FORWARD(g_statusEventListeners, OnTimerInvalidated, slot);
	}

	bool OnPause(KZPlayer *player) override
	{
		i32 slot = player->GetPlayerSlot().Get();
		CALL_FORWARD(g_statusEventListeners, OnPause, slot);
		return true;
	}

	void OnPausePost(KZPlayer *player) override
	{
		i32 slot = player->GetPlayerSlot().Get();
		CALL_FORWARD(g_statusEventListeners, OnPausePost, slot);
	}

	bool OnResume(KZPlayer *player) override
	{
		i32 slot = player->GetPlayerSlot().Get();
		CALL_FORWARD(g_statusEventListeners, OnResume, slot);
		return true;
	}

	void OnResumePost(KZPlayer *player) override
	{
		i32 slot = player->GetPlayerSlot().Get();
		CALL_FORWARD(g_statusEventListeners, OnResumePost, slot);
	}
};

static_global KZStatusTimerBridge g_statusTimerBridge;

namespace KZ::status
{
	void Init()
	{
		KZTimerService::RegisterEventListener(&g_statusTimerBridge);
	}

	void Cleanup()
	{
		KZTimerService::UnregisterEventListener(&g_statusTimerBridge);
	}
} // namespace KZ::status
