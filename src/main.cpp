// This code goes in `main.cpp`.
#include "main.hpp"

#include <SceneTree.hpp>

#include "mob.hpp"
#include <iostream>
#include <string>
#include <stdlib.h>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>


void Main::_ready() {
    //std::cout << "Main::_ready()" << std::endl;

    _hud = get_node<HUD>("HUD");
    _player = get_node<Player>("Player");
    _start_position = get_node<godot::Node2D>("StartPosition");
    _mob_spawn_location = get_node<godot::PathFollow2D>("MobPath/MobSpawnLocation");
    _mob_timer = get_node<godot::Timer>("MobTimer");
    _score_timer = get_node<godot::Timer>("ScoreTimer");
    _start_timer = get_node<godot::Timer>("StartTimer");
    _end_timer = get_node<godot::Timer>("EndTimer");
    // Uncomment these after adding the nodes in the "Sound effects" section of "Finishing up".
    //_music = get_node<godot::AudioStreamPlayer>("Music");
    //_death_sound = get_node<godot::AudioStreamPlayer>("DeathSound");
    _random = (godot::Ref<godot::RandomNumberGenerator>)godot::RandomNumberGenerator::_new();
    _random->randomize();
    
    godot::PoolStringArray cmdline_args = godot::OS::get_singleton()->get_cmdline_args();
    int cmdline_args_size = cmdline_args.size();

    mem = new cSharedMemory();
    sem_physics = sem_physics_class._new();
    sem_physics->post();

    mem->found = false;
    for (int i = 0; i < cmdline_args_size; i++) {
    	//godot::Godot::print("cmdline_args[i]: " + cmdline_args[i]);
    
        if (cmdline_args[i].find(godot::String("--handle")) == 0) {
            mem->found = true;
        }
        
        if (cmdline_args[i].find(godot::String("--turbo")) == 0) {
            godot::Godot::print("turbo mode");
            time_scale = 2;
        }
    }
    
    godot::Engine::get_singleton()->set_time_scale(1 * time_scale);
    godot::Engine::get_singleton()->set_iterations_per_second(10 * time_scale);
    
    if (mem->exists()) {
        sem_action = new cSharedMemorySemaphore();
        sem_observation = new cSharedMemorySemaphore();

        agent_action_tensor = mem->findFloatTensor("agent_action");
        env_action_tensor = mem->findIntTensor("env_action");
        observation_tensor = mem->findUintTensor("observation");
        reward_tensor = mem->findFloatTensor("reward");
        done_tensor = mem->findIntTensor("done");

        sem_action->init("sem_action");
        sem_observation->init("sem_observation");
    }

    viewport = get_viewport();
    viewport_texture = viewport->get_texture();
}


// This code goes in `main.cpp`.
void Main::game_over() {
    //_score_timer->stop();
    //_mob_timer->stop();
    //_hud->show_game_over();
    reward[0] = 0.0;
    done[0] = 1;

    //new_game();
}


void Main::get_coin() {
    //std::cout << "Main::get_coin()" << std::endl;
    //_score_timer->stop();
    //_mob_timer->stop();
    //_hud->show_game_over();
    reward[0] = 1;

    score += 1;
    _hud->update_score(score);

    //std::cout << "" << std::endl;
}


void Main::new_game() {
    //std::cout << "Main::new_game()" << std::endl;

    reward[0] = 0.0;
    done[0] = 0;

    score = 0;
    _player->start(_start_position->get_position());
    _start_timer->start();
    _hud->update_score(score);
    _hud->show_get_ready();
    get_tree()->call_group("coins", "queue_free");

    _add_Coin(50, 250);
    _add_Coin(100, 100);
    _add_Coin(200, 450);
    _add_Coin(700, 100);
    _add_Coin(300, 300);
    _add_Coin(100, 700);
    _add_Coin(600, 250);
    _add_Coin(400, 50);
    _add_Coin(500, 700);
    _add_Coin(700, 700);
}


// This code goes in `main.cpp`.
void Main::_on_ScoreTimer_timeout() {
    //std::cout << "Main::_on_ScoreTimer_timeout()" << std::endl;
    //score += 1;
    //test_score = score;
    //_hud->update_score(score);
    ;
}


void Main::_on_StartTimer_timeout() {
    //std::cout << "Main::_on_StartTimer_timeout()" << std::endl;
    //_mob_timer->start();
    _score_timer->start();
    _end_timer->start();
}


void Main::_on_EndTimer_timeout() {
    //std::cout << "Main::_on_EndTimer_timeout()" << std::endl;
    game_over();
}


// Also add this to register all methods and the mob scene property.
void Main::_register_methods() {
    godot::register_method("_ready", &Main::_ready);
    godot::register_method("_add_Coin", &Main::_add_Coin);
    godot::register_method("get_coin", &Main::get_coin);
    godot::register_method("game_over", &Main::game_over);
    godot::register_method("new_game", &Main::new_game);
    godot::register_method("_on_MobTimer_timeout", &Main::_on_MobTimer_timeout);
    godot::register_method("_on_ScoreTimer_timeout", &Main::_on_ScoreTimer_timeout);
    godot::register_method("_on_StartTimer_timeout", &Main::_on_StartTimer_timeout);
    godot::register_method("_on_EndTimer_timeout", &Main::_on_EndTimer_timeout);
    godot::register_property("mob_scene", &Main::mob_scene, (godot::Ref<godot::PackedScene>)nullptr);
    godot::register_property("coin_scene", &Main::coin_scene, (godot::Ref<godot::PackedScene>)nullptr);
    godot::register_method("_get_screen_frame", &Main::_get_screen_frame);
    godot::register_method("_observation_Function", &Main::_observation_Function);
}


void Main::_add_Coin(float x, float y) {
    // Create a new instance of the Mob scene.
    godot::Node *coin = coin_scene->instance();
    godot::Vector2 position(x, y);

    // Set the mob's position to a random location.
    coin->set("position", position);

    // Spawn the mob by adding it to the Main scene.
    add_child(coin);
}


// This code goes in `main.cpp`.
void Main::_on_MobTimer_timeout() {
    // Create a new instance of the Mob scene.
    godot::Node *mob = mob_scene->instance();

    // Choose a random location on Path2D.
    _mob_spawn_location->set_offset((real_t)_random->randi());

    // Set the mob's direction perpendicular to the path direction.
    real_t direction = _mob_spawn_location->get_rotation() + (real_t)Math_PI / 2;

    // Set the mob's position to a random location.
    mob->set("position", _mob_spawn_location->get_position());

    // Add some randomness to the direction.
    direction += _random->randf_range((real_t)-Math_PI / 4, (real_t)Math_PI / 4);
    mob->set("rotation", direction);

    // Choose the velocity for the mob.
    godot::Vector2 velocity = godot::Vector2(_random->randf_range(150.0, 250.0), 0.0);
    mob->set("linear_velocity", velocity.rotated(direction));

    // Spawn the mob by adding it to the Main scene.
    add_child(mob);
}


godot::PoolByteArray Main::_get_screen_frame() {
    img = viewport_texture->get_data();

    std::string img_format = std::to_string(img->get_format());
    //godot::Godot::print("img_format: " + godot::String(img_format.c_str()));

    img->convert(4);
    img->resize(128, 128, 0);

    int height = img->get_height();
	int width = img->get_width();

    img->lock();
    img_data = img->get_data();
    img->unlock();

    return img_data;
}


void Main::_observation_Function() {
    //godot::Godot::print("_observation_Function loop");
    sem_physics->wait();

    observation = _get_screen_frame();

    if (mem->exists()) {
		observation_tensor->write(observation);
		reward_tensor->write(reward);
        if (reward[0] == 1.0)
            reward[0] = 0.0;

		done_tensor->write(done);
		sem_observation->post();

        if (done[0] == 1)
            new_game();
    }

    _player->timeout = true;
	sem_physics->post();
}
