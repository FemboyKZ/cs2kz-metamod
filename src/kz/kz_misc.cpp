#include "common.h"
#include "utils/utils.h"
#include "kz.h"
#include "utils/simplecmds.h"

#include "checkpoint/kz_checkpoint.h"
#include "jumpstats/kz_jumpstats.h"
#include "quiet/kz_quiet.h"
#include "mode/kz_mode.h"
#include "language/kz_language.h"
#include "style/kz_style.h"
#include "noclip/kz_noclip.h"
#include "hud/kz_hud.h"
#include "spec/kz_spec.h"
#include "timer/kz_timer.h"
#include "tip/kz_tip.h"



// TODO: move command registration to the service class?
void KZ::misc::RegisterCommands()
{
	// TODO: Fullupdate spectators on spec_mode/spec_next/spec_player/spec_prev
	KZJumpstatsService::RegisterCommands();
	KZHUDService::RegisterCommands();
	KZLanguageService::RegisterCommands();
	KZ::style::RegisterCommands();
}

void KZ::misc::OnClientActive(CPlayerSlot slot)
{
	KZPlayer *player = g_pKZPlayerManager->ToPlayer(slot);
	player->Reset();
}
