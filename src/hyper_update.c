#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "z64recomp_api.h"

#define HYPER_UPDATES recomp_get_config_u32("update_rate")

typedef struct {
    /* 0x00 */ PlayState* play;
    /* 0x04 */ Actor* actor;
    /* 0x08 */ u32 freezeExceptionFlag;
    /* 0x0C */ u32 canFreezeCategory;
    /* 0x10 */ Actor* talkActor;
    /* 0x14 */ Player* player;
    /* 0x18 */ u32 updateActorFlagsMask; // Actor will update only if at least 1 actor flag is set in this bitmask
} UpdateActor_Params;                    // size = 0x1C

ActorExtensionId hyperUpdateCountExtension;

RECOMP_CALLBACK("*", recomp_on_init) 
void Hyper_ExtendActors() {
    hyperUpdateCountExtension = z64recomp_extend_actor_all(sizeof(u32));
}

Actor* Actor_UpdateActor(UpdateActor_Params* params);

// the following is mostly derived from OoTMM's Hyper Actor system
RECOMP_HOOK("Actor_UpdateActor")
void Hyper_UpdateActor(UpdateActor_Params* params) {
    PlayState* play = params->play;
    Actor* actor = params->actor;
    Player* player = GET_PLAYER(play);

    if (actor->category != ACTORCAT_BOSS && actor->category != ACTORCAT_ENEMY) {
        return;
    }

    if (actor->id == ACTOR_EN_DG && !recomp_get_config_u32("include_dogs")) {
        return;
    }

    if (Player_InBlockingCsMode(play, player)) {
        return;
    }

    u32* hyperCount = z64recomp_get_extended_actor_data(actor, hyperUpdateCountExtension);
    *hyperCount += 1;

    if (*hyperCount < HYPER_UPDATES) {
        Actor_UpdateActor(params);
    } else {
        *hyperCount = 0;
    }
}