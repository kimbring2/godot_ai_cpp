// Copy `player.gdns` to `mob.gdns` and replace `Player` with `Mob`.
// Attach the `mob.gdns` file to the Mob node.

// Create two files `mob.cpp` and `mob.hpp` next to `entry.cpp` in `src`.
// This code goes in `mob.hpp`. We also define the methods we'll be using here.
#ifndef MOB_H
#define MOB_H

#include <AnimatedSprite.hpp>
#include <Godot.hpp>
#include <RigidBody2D.hpp>

class Mob : public godot::RigidBody2D {
    GODOT_CLASS(Mob, godot::RigidBody2D)

    godot::AnimatedSprite *_animated_sprite;

public:
    void _init() {}
    void _ready();
    void _on_VisibilityNotifier2D_screen_exited();

    static void _register_methods();

    int id = 1;
};

#endif // MOB_H