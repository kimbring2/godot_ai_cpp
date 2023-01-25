// This code goes in `mob.cpp`.
#include "mob.hpp"

#include <RandomNumberGenerator.hpp>
#include <SpriteFrames.hpp>


void Mob::_register_methods() {
    godot::register_method("_ready", &Mob::_ready);
    godot::register_method("_on_VisibilityNotifier2D_screen_exited", &Mob::_on_VisibilityNotifier2D_screen_exited);
}


void Mob::_ready() {
    godot::Ref<godot::RandomNumberGenerator> random = godot::RandomNumberGenerator::_new();
    random->randomize();
    //_animated_sprite = get_node<godot::AnimatedSprite>("AnimatedSprite");
    //_animated_sprite->set_playing(true);
    //godot::PoolStringArray mob_types = _animated_sprite->get_sprite_frames()->get_animation_names();
    //_animated_sprite->set_animation(mob_types[random->randi() % mob_types.size()]);
}


// This code goes in `mob.cpp`.
void Mob::_on_VisibilityNotifier2D_screen_exited() {
    queue_free();
}