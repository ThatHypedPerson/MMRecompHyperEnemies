#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "z64recomp_api.h"

#define HYPER_UPDATES recomp_get_config_u32("update_rate")

// the following is mostly derived from OoTMM's Hyper Actor system

bool Hyper_ShouldApply(PlayState* play, Actor* actor) {
    bool should = false;
    
    Player* player = GET_PLAYER(play);
    if (Player_InBlockingCsMode(play, player)) {
        return false;
    }

    if (actor->category == ACTORCAT_BOSS || actor->category == ACTORCAT_ENEMY) {
        should = true;
    }

    // dogs are enemies
    if (actor->id == ACTOR_EN_DG && !recomp_get_config_u32("include_dogs")) {
        should = false;
    }
    
    // flying pots are not enemies
    if (actor->id == ACTOR_EN_TUBO_TRAP) {
        should = true;
    }

    return should;
}

// this function happens after actor->update(actor, play) in Actor_UpdateActor
RECOMP_HOOK("DynaPoly_UnsetAllInteractFlags")
void Hyper_UpdateActor(PlayState* play, DynaCollisionContext* dyna, Actor* actor) {
    if (!Hyper_ShouldApply(play, actor)) return;

    bool prev = play->frameAdvCtx.enabled;
    play->frameAdvCtx.enabled = true;

    for (u32 i = 0; i < HYPER_UPDATES - 1; i++) {
        if (actor->update != NULL) {
            actor->update(actor, play);
        }
    }

    play->frameAdvCtx.enabled = prev;
}