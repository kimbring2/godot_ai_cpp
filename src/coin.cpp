// This code goes in `coin.cpp`.
#include "coin.hpp"
#include <iostream>

#include <RandomNumberGenerator.hpp>
#include <SpriteFrames.hpp>


void Coin::_register_methods() {
    godot::register_method("_ready", &Coin::_ready);
    godot::register_method("_on_VisibilityNotifier2D_screen_exited", &Coin::_on_VisibilityNotifier2D_screen_exited);
}


void Coin::_ready() {
    //std::cout << "Coin::_ready()" << std::endl;

    godot::Ref<godot::RandomNumberGenerator> random = godot::RandomNumberGenerator::_new();
    random->randomize();
    _animation_player = get_node<godot::AnimationPlayer>("AnimationPlayer");
    _animation_player->play("Side1");
    //godot::PoolStringArray mob_types = _animated_sprite->get_sprite_frames()->get_animation_names();
    //_animated_sprite->set_animation(mob_types[random->randi() % mob_types.size()]);
    _collision_shape = get_node<godot::CollisionShape2D>("CollisionShape2D");
}


// This code goes in `mob.cpp`.
void Coin::_on_VisibilityNotifier2D_screen_exited() {
    queue_free();
}