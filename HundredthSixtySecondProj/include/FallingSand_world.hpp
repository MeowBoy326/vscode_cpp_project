// Copyright(c) 2022-2023, KaoruXun All rights reserved.

#ifndef ME_WORLD_HPP
#define ME_WORLD_HPP

#include <deque>
#include <future>
#include <unordered_map>
#include <vector>

#include "FallingSand_chunk.hpp"
#include "FallingSand_audio.h"
#include "FallingSand_const.h"
#include "FallingSand_macros.hpp"
#include "FallingSand_ecs.hpp"
#include "box2d.h"
#include "FallingSand_utility.hpp"
#include "FallingSand_player.hpp"
#include "FallingSand_game_basic.hpp"
#include "FallingSand_game_datastruct.hpp"
#include "fastnoise.h"
#include "phmap.h"

namespace ME {

class Populator;
class WorldGenerator;
class Player;
struct CellData;

class LoadChunkParams {
public:
    int x;
    int y;
    bool populate;
    long long time;
    LoadChunkParams(int x, int y, bool populate, long long time) {
        this->x = x;
        this->y = y;
        this->populate = populate;
        this->time = time;
    }
};

struct WorldMeta {
    std::string worldName;
    std::string lastOpenedVersion;
    time_t lastOpenedTime = 0;

    static WorldMeta loadWorldMeta(std::string worldFileName, bool noSaveLoad = false);

    bool save(std::string worldFileName);
};
// METADOT_STRUCT(WorldMeta, worldName, lastOpenedVersion, lastOpenedTime);

struct WorldSystem {
    using TPL = scope<thread_pool>;
    TPL tickPool;
    TPL tickVisitedPool;
    TPL updateRigidBodyHitboxPool;
};

class world {
    // using PhyBodytype = phy::Body::BodyType;

public:
    std::string worldName = "";
    WorldMeta metadata{};
    bool noSaveLoad = false;

    R_Target *target = nullptr;

    ~world();

    struct {
        std::vector<CellData *> cells;
        std::vector<RigidBody *> rigidBodies;
        std::vector<RigidBody *> worldRigidBodies;
        std::vector<std::vector<MEvec2>> worldMeshes;
        std::vector<std::vector<MEvec2>> worldTris;

        std::vector<std::vector<b2PolygonShape>> polys2s = {};

        std::vector<LoadChunkParams> toLoad;                // 需要加载的区块的列表
        std::vector<std::future<Chunk *>> toLoadAsyncList;  // 区块生成加载异步对象列表
        std::deque<Chunk *> readyToMerge;                   // 区块合并列表

        std::vector<PlacedStructure> structures;
        std::vector<MEvec2> distributedPoints;
        phmap::flat_hash_map<int, phmap::flat_hash_map<int, Chunk *>> chunkCache;
        std::vector<Populator *> populators;

        ecs::entity_id player;
    };

    struct {
        ecs::registry registry;

        WorldSystem world_sys;
    };

    ecs::registry &Reg() { return registry; }

    bool *hasPopulator = nullptr;
    int highestPopulator = 0;
    FastNoise noise;
    Audio *audioEngine = nullptr;

    // 这里应该不同于区块类储存的材料实例
    // 这里储存的应该是世界改变的材料实例
    std::vector<MaterialInstance> real_tiles{};
    std::vector<MaterialInstance> real_layer2{};

    std::vector<u32> background{};

    f32 *flowX = nullptr;
    f32 *flowY = nullptr;
    f32 *prevFlowX = nullptr;
    f32 *prevFlowY = nullptr;

    u16 width = 0;
    u16 height = 0;
    int tickCt = 0;

    R_Image *fireTex = nullptr;
    bool *tickVisited1 = nullptr;
    bool *tickVisited2 = nullptr;
    i32 *newTemps = nullptr;
    bool needToTickGeneration = false;

    bool *dirty = nullptr;
    bool *active = nullptr;
    bool *lastActive = nullptr;
    bool *layer2Dirty = nullptr;
    bool *backgroundDirty = nullptr;
    MErect loadZone;
    MErect lastLoadZone{};
    MErect tickZone{};
    MErect meshZone{};
    MErect lastMeshZone{};
    MErect lastMeshLoadZone{};

    b2Vec2 gravity{};
    ME::scope<b2World> b2world = nullptr;
    RigidBody *staticBody = nullptr;
    WorldGenerator *gen = nullptr;

    void init(std::string worldPath, u16 w, u16 h, R_Target *renderer, Audio *audioEngine, WorldGenerator *generator);
    void init(std::string worldPath, u16 w, u16 h, R_Target *target, Audio *audioEngine);
    MaterialInstance getTile(int x, int y);
    void setTile(int x, int y, MaterialInstance type);
    MaterialInstance getTileLayer2(int x, int y);
    void setTileLayer2(int x, int y, MaterialInstance type);
    void tick();
    void tickTemperature();
    void frame();
    void tickCells();
    void renderCells(unsigned char **texture);
    void tickObjectBounds();
    void tickObjects();
    void tickObjectsMesh();
    void tickChunks();
    void tickChunkGeneration();
    void addCell(CellData *cell);
    void explosion(int x, int y, int radius);
    RigidBody *makeRigidBody(b2BodyType type, f32 x, f32 y, f32 angle, b2PolygonShape shape, f32 density, f32 friction, TextureRef texture);
    RigidBody *makeRigidBodyMulti(b2BodyType type, f32 x, f32 y, f32 angle, std::vector<b2PolygonShape> shape, f32 density, f32 friction, TextureRef texture);
    void updateRigidBodyHitbox(RigidBody *rb);
    void updateChunkMesh(Chunk *chunk);
    void updateWorldMesh();
    void queueLoadChunk(int cx, int cy, bool populate, bool render);
    Chunk *loadChunk(LoadChunkParams para);
    Chunk *loadChunk(Chunk *ch, bool populate, bool render);
    void unloadChunk(Chunk *ch);
    void writeChunkToDisk(Chunk *ch);
    void chunkSaveCache(Chunk *ch);
    void generateChunk(Chunk *ch);
    int getBiomeAt(int x, int y);             // 返回群系ID
    int getBiomeAt(Chunk *ch, int x, int y);  // 返回群系ID
    void addStructure(PlacedStructure str);
    MEvec2 getNearestPoint(f32 x, f32 y);
    std::vector<MEvec2> getPointsWithin(f32 x, f32 y, f32 w, f32 h);
    Chunk *getChunk(int cx, int cy);
    void populateChunk(Chunk *ch, int phase, bool render);
    void tickEntities(R_Target *target);
    void forLine(int x0, int y0, int x1, int y1, std::function<bool(int)> fn);
    void forLineCornered(int x0, int y0, int x1, int y1, std::function<bool(int)> fn);
    RigidBody *physicsCheck(int x, int y);
    void physicsCheck_flood(int x, int y, bool *visited, int *count, u32 *cols, int *minX, int *maxX, int *minY, int *maxY);
    void saveWorld();
    bool isPlayerInWorld();
    std::tuple<WorldEntity *, Player *> getHostPlayer();
};
}  // namespace ME

#endif
