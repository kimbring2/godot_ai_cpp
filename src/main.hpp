// Copy `player.gdns` to `main.gdns` and replace `Player` with `Main`.
// Attach the `main.gdns` file to the Main node.

// Create two files `main.cpp` and `main.hpp` next to `entry.cpp` in `src`.
// This code goes in `main.hpp`. We also define the methods we'll be using here.
#ifndef MAIN_H
#define MAIN_H

#include <AudioStreamPlayer.hpp>
#include <CanvasLayer.hpp>
#include <Godot.hpp>
#include <Node.hpp>
#include <PackedScene.hpp>
#include <PathFollow2D.hpp>
#include <RandomNumberGenerator.hpp>
#include <Timer.hpp>

#include "hud.hpp"
#include "player.hpp"
#include "coin.hpp"
#include "cSharedMemory.hpp"
#include "OS.hpp"
#include "Engine.hpp"
#include "String.hpp"
#include "Semaphore.hpp"

#include "Viewport.hpp"
#include "ViewportTexture.hpp"
#include "Texture.hpp"
#include "Image.hpp"
#include "PoolArrays.hpp"

class Main : public godot::Node {
    GODOT_CLASS(Main, godot::Node)

    Player *_player;
    godot::Node2D *_start_position;
    godot::PathFollow2D *_mob_spawn_location;
    godot::Timer *_mob_timer;
    godot::Timer *_score_timer;
    godot::Timer *_start_timer;
    godot::Timer *_end_timer;
    godot::AudioStreamPlayer *_music;
    godot::AudioStreamPlayer *_death_sound;
    godot::Ref<godot::RandomNumberGenerator> _random;
public:
    HUD *_hud;
    godot::Ref<godot::PackedScene> mob_scene;
    godot::Ref<godot::PackedScene> coin_scene;

    int time_scale = 1;
    int score;
    int env_action_0 = 0;
    int env_action_1 = 0;

    std::vector<float> reward = {0.0};
    std::vector<int> done = {0};

    void _init() {}
    void _ready();
    void game_over();
    void new_game();
    void _add_Coin(float x, float y);
    void get_coin();
    void _on_MobTimer_timeout();
    void _on_ScoreTimer_timeout();
    void _on_StartTimer_timeout();
    void _on_EndTimer_timeout();

    static void _register_methods();

    godot::PoolByteArray _get_screen_frame();
    void _observation_Function();

    cPersistentFloatTensor *agent_action_tensor;
    cPersistentIntTensor *env_action_tensor;
    cPersistentUintTensor *observation_tensor;
    cPersistentFloatTensor *reward_tensor;
    cPersistentIntTensor *done_tensor;

    cSharedMemory *mem;
    godot::Semaphore sem_physics_class;
    godot::Semaphore *sem_physics;

    cSharedMemorySemaphore *sem_action;
    cSharedMemorySemaphore *sem_observation;

    godot::Viewport *viewport;
    godot::Ref<godot::ViewportTexture> viewport_texture;
    godot::PoolByteArray img_data;
    godot::Ref<godot::Image> img;
    godot::PoolByteArray observation;
};

#endif // MAIN_H
