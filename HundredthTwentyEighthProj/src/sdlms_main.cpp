#include <SDL.h>
#include <stdio.h>
#include "sdlms_World.h"
#include "sdlms_RenderSystem.h"
#include "sdlms_SoundSystem.h"
#include "sdlms_DeltaTimeSystem.h"
#include "sdlms_CameraSystem.h"
#include "sdlms_PhysicSystem.h"
#include "sdlms_InputSystem.h"
#include "sdlms_SpriteSystem.h"
#include "sdlms_TransformSystem.h"
#include "sdlms_Sound.h"
#include "sdlms_Camera.h"
#include "sdlms_Player.h"
#include "sdlms_Normal.h"
#include "sdlms_Video.h"
#include "sdlms_Character.h"
#include "sdlms_Mob.h"
#include "sdlms_Map.h"
#include "sdlms_FreeType.hpp"
#include "sdlms_Wz.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

int width = 800;
int height = 600;

void main_loop()
{
    World *world = World::get_world();
    [[unlikely]]
    if (world->is_game_quit())
    {
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop(); /* this should "kill" the app. */
#else
        exit(0);
#endif
    }
    world->poll_events();
    world->tick_delta_time();

    Window::clear();
    world->process_systems();
    Window::update();
}

int main(int argc, char *argv[])
{
    World world;
#ifdef __ANDROID__
    Wz *wz = new Wz("/sdcard/Data/"); // wz文件路径
    FreeType::init("/sdcard/Data/");
#else
    Wz *wz = new Wz("./Data/");
    FreeType::init("./Data/");
#endif
    world.add_resource(wz);

    Window::create_window("sdlMS", width, height);

    world.add_system(new SoundSystem());

    world.add_system(new InputSystem());

    world.add_system(new PhysicSystem());

    world.add_system(new DeltaTimeSystem());

    world.add_system(new SpriteSystem());

    world.add_system(new TransformSystem());

    world.add_system(new CameraSystem());

    world.add_system(new RenderSystem());

    Camera *camera = new Camera(0, 0, width, height);
    Player *pla = new Player(); // 玩家控制的角色
    {
        Character *cha = new Character(&world, {0, 0});
        Normal *nor = new Normal();

        cha->add_component(camera);
        cha->add_component(nor);
        cha->add_component(pla);

        world.add_component(camera, 0);
        world.add_component(nor);
        world.add_component(pla);
    }
    // {
    //     Mob *mob = new Mob(&world, {0, 0});
    //     Normal *nor = new Normal();
    //     mob->add_component(nor);
    //     mob->add_component(pla);
    //     mob->add_component(camera);
    //     world.add_component(camera, 0);
    //     world.add_component(nor);
    //     world.add_component(pla);
    // }
    // Map::load(104040000, &world);
    Map::load(105040300, &world);

    // Video *vid = Video::load("a.mp4", 244, 184);
    // if (vid != nullptr)
    // {
    //     Entity *ent = new Entity();
    //     Transform *t = new Transform{(float)244, (float)184};
    //     ent->add_component(vid);
    //     ent->add_component(t);
    //     world.add_component(t,123000);
    //     world.add_component(vid);
    // }
    world.tick_delta_time();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while (true)
    {
        main_loop();
    }
#endif
    return 0;
}