// Copy `player.gdns` to `mob.gdns` and replace `Player` with `Mob`.
// Attach the `mob.gdns` file to the Mob node.

// Create two files `mob.cpp` and `mob.hpp` next to `entry.cpp` in `src`.
// This code goes in `mob.hpp`. We also define the methods we'll be using here.
#ifndef COIN_H
#define COIN_H

#include <AnimationPlayer.hpp>
#include <CollisionShape2D.hpp>
#include <Godot.hpp>
#include <RigidBody2D.hpp>

class Coin : public godot::RigidBody2D {
    GODOT_CLASS(Coin, godot::RigidBody2D)

    godot::AnimationPlayer *_animation_player;
    godot::CollisionShape2D *_collision_shape;

public:
    void _init() {}
    void _ready();
    void _on_VisibilityNotifier2D_screen_exited();

    static void _register_methods();

    int id = 1;
};

#endif // COIN_H