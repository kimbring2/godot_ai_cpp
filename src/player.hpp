// A `player.gdns` file has already been created for you. Attach it to the Player node.

// Create two files `player.cpp` and `player.hpp` next to `entry.cpp` in `src`.
// This code goes in `player.hpp`. We also define the methods we'll be using here.
#ifndef PLAYER_H
#define PLAYER_H

#include <AnimatedSprite.hpp>
#include <Area2D.hpp>
#include <CollisionShape2D.hpp>
#include <Godot.hpp>
#include <Input.hpp>


class Player : public godot::Area2D {
    GODOT_CLASS(Player, godot::Area2D)
    
    godot::Node* parent;

    godot::AnimatedSprite *_animated_sprite;
    godot::CollisionShape2D *_collision_shape;
    godot::Input *_input;
    godot::Vector2 _screen_size; // Size of the game window.
    
public:
    real_t speed = 400; // How fast the player will move (pixels/sec).
    bool timeout = true;
    godot::Vector2 velocity;

    void _init() {}
    void _ready();
    void _process(const double p_delta);
    void _physics_process(const double p_delta);
    void start(const godot::Vector2 p_position);
    void _on_Player_body_entered(godot::Node2D *_body);

    static void _register_methods();
};

#endif // PLAYER_H