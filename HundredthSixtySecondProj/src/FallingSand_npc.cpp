
#include "FallingSand_npc.hpp"

#include "FallingSand_global.hpp"
#include "FallingSand_game.hpp"
#include "FallingSand_items.hpp"

namespace ME {

void State::retrieveState(world &world, int bot) {}

bool Bot::tryInterrupt(State _state) {

    interrupt = true;
    return true;
}

void Bot::executeTask(world &world) {
    if (interrupt) {

        interrupt = false;
    } else {
    }
}

void Bot::updateMemory(Memory &query, bool all, Memory &memory) {
    for (unsigned int i = 0; i < memories.size(); i++) {
        if (all && (memories[i].state == query.state)) {
            memories[i] = memory;
            continue;
        } else if (!all && (memories[i].state || query.state)) {
            memories[i] = memory;
            continue;
        }
    }
}

std::deque<Memory> Bot::recallMemories(Memory &query, bool all) {

    std::deque<Memory> recalled;

    for (unsigned int i = 0; i < memories.size(); i++) {
        if (all && (memories[i].state == query.state)) {
            recalled.push_back(memories[i]);
            memories[i].recallScore++;
            continue;
        } else if (!all && (memories[i].state || query.state)) {
            recalled.push_back(memories[i]);
            memories[i].recallScore++;
            continue;
        }
    }
    return recalled;
}

inline void Bot::addMemory(State &state) {
    for (unsigned int i = 0; i < memories.size(); i++) {
        if (!ME_vec2_equals(memories[i].state.pos, state.pos)) continue;

        memories[i].state.task = state.task;
        return;
    }

    Memory memory;
    Memory oldMemory;
    memory.state = state;
    memory.state.reachable = true;

    for (unsigned int i = 1; i < memories.size(); i++) {
        if (memories[i].recallScore > memories[i - 1].recallScore) {
            oldMemory = memories[i - 1];
            memories[i - 1] = memories[i];
            memories[i] = oldMemory;
        }
    }

    memories.push_front(memory);

    if (memories.size() > memorySize) {
        memories.pop_back();
    }
}

void Bot::setupSprite() {}

void Bot::render(WorldEntity *we, R_Target *target, int ofsX, int ofsY) {
    if (heldItem != NULL) {
        int scaleEnt = global.game->Iso.globaldef.hd_objects ? global.game->Iso.globaldef.hd_objects_size : 1;

        MErect *ir = new MErect{(f32)(int)(ofsX + we->x + we->hw / 2.0 - heldItem->texture->surface()->w), (f32)(int)(ofsY + we->y + we->hh / 2.0 - heldItem->texture->surface()->h / 2),
                                (f32)heldItem->texture->surface()->w, (f32)heldItem->texture->surface()->h};
        f32 fx = (f32)(int)(-ir->x + ofsX + we->x + we->hw / 2.0);
        f32 fy = (f32)(int)(-ir->y + ofsY + we->y + we->hh / 2.0);
        fx -= heldItem->pivotX;
        ir->x += heldItem->pivotX;
        fy -= heldItem->pivotY;
        ir->y += heldItem->pivotY;
        R_SetShapeBlendMode(R_BlendPresetEnum::R_BLEND_ADD);
        // R_BlitTransformX(heldItem->texture, NULL, target, ir->x, ir->y, fp->x, fp->y, holdAngle, 1, 1);
        // SDL_RenderCopyExF(renderer, heldItem->texture, NULL, ir, holdAngle, fp, abs(holdAngle) > 90 ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
        ir->x *= scaleEnt;
        ir->y *= scaleEnt;
        ir->w *= scaleEnt;
        ir->h *= scaleEnt;
        R_BlitRectX(heldItem->image, NULL, target, ir, holdAngle, fx, fy, abs(holdAngle) > 90 ? R_FLIP_VERTICAL : R_FLIP_NONE);
        delete ir;
    }
}

void Bot::renderLQ(WorldEntity *we, R_Target *target, int ofsX, int ofsY) { R_Rectangle(target, we->x + ofsX, we->y + ofsY, we->x + ofsX + we->hw, we->y + ofsY + we->hh, {0xff, 0x00, 0xff, 0xff}); }

void NpcSystem::process(ecs::registry &world, const move_player_event &evt) {
    world.for_joined_components<WorldEntity, Bot>(
            [&evt](ecs::entity, WorldEntity &we, Bot &npc) {
                npc.renderLQ(&we, evt.g->TexturePack_.textureEntitiesLQ->target, evt.g->Iso.world->loadZone.x + (int)(we.vx * evt.thruTick),
                             evt.g->Iso.world->loadZone.y + (int)(we.vy * evt.thruTick));
                npc.render(&we, evt.g->TexturePack_.textureEntities->target, evt.g->Iso.world->loadZone.x + (int)(we.vx * evt.thruTick), evt.g->Iso.world->loadZone.y + (int)(we.vy * evt.thruTick));
            },
            ecs::exists<Bot>{} && ecs::exists<Controlable>{});
}

}  // namespace ME