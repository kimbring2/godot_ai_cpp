// This code goes in `player.cpp`.
#include "player.hpp"
#include <string>

#include <chrono>
#include <thread>

#include "main.hpp"


// This code goes in `player.cpp`.
// We need to register the signal here, and while we're here, we can also
// register the other methods and register the speed property.
void Player::_register_methods() {
    godot::register_method("_ready", &Player::_ready);
    godot::register_method("_physics_process", &Player::_physics_process);
    godot::register_method("_process", &Player::_process);
    godot::register_method("start", &Player::start);
    godot::register_method("_on_Player_body_entered", &Player::_on_Player_body_entered);
    godot::register_property("speed", &Player::speed, (real_t)400.0);

    // This below line is the signal.
    godot::register_signal<Player>("hit", godot::Dictionary());
    godot::register_signal<Player>("coin", godot::Dictionary());
}


void Player::_ready() {
    parent = get_parent();

    _animated_sprite = get_node<godot::AnimatedSprite>("AnimatedSprite");
    _collision_shape = get_node<godot::CollisionShape2D>("CollisionShape2D");

    //_animated_sprite->set_playing(true);

    _input = godot::Input::get_singleton();
    _screen_size = get_viewport_rect().size;

    velocity[0] = 0;
    velocity[1] = 0;

    hide();
}


void Player::_process(const double p_delta) {
    ;
}


// This code goes in `player.cpp`.
void Player::_physics_process(const double p_delta) {
    //std::cout << "p_delta: " << p_delta << std::endl;
    if (timeout == true) {
        //std::cout << "Player::_physics_process" << std::endl;
        //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        godot::Node* parent = get_parent();
        godot::String parent_name = parent->get_name();
        Main *_main = godot::Object::cast_to<Main>(parent);

        _main->sem_physics->wait();

        //godot::Vector2 velocity(0, 0);
        if (_main->mem->exists()) {
            _main->sem_action->wait();
            std::vector<int> env_action = _main->env_action_tensor->read();
            std::vector<float> agent_action = _main->agent_action_tensor->read();

            //std::vector<int> env_action = {0, 0}
            //std::vector<float> agent_action = {0};

            //float action = 0.0;

            float action = agent_action[0];
            if (env_action[0] == 1) {
                _main->_hud->_on_StartButton_pressed();
            }
            
            if (action == 0.0) {
                velocity.x = 1;
                velocity.y = 0;
            }
                
            else if (action == 1.0) {
                velocity.x = -1;
                velocity.y = 0;
            }
                
            else if (action == 2.0) {
                velocity.x = 0;
                velocity.y = 1;
            }
                
            else if (action == 3.0) {
                velocity.x = 0;
                velocity.y = -1;
            }
            /*
            if (_input->is_action_pressed("move_right")) {
                velocity.x = 1;
                velocity.y = 0;
            } else if (_input->is_action_pressed("move_left")) {
                velocity.x = -1;
                velocity.y = 0;
            } else if (_input->is_action_pressed("move_down")) {
                velocity.x = 0;
                velocity.y = 1;
            } else if (_input->is_action_pressed("move_up")) {
                velocity.x = 0;
                velocity.y = -1;
            }
            */
        } else {
            //std::cout << "Non _main->mem->exists()" << std::endl;
            //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            //std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
            //std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

            if (_input->is_action_pressed("move_right")) {
                velocity.x = 1;
                velocity.y = 0;
            } else if (_input->is_action_pressed("move_left")) {
                velocity.x = -1;
                velocity.y = 0;
            } else if (_input->is_action_pressed("move_down")) {
                velocity.x = 0;
                velocity.y = 1;
            } else if (_input->is_action_pressed("move_up")) {
                velocity.x = 0;
                velocity.y = -1;
            }

            //velocity.x = _input->get_action_strength("move_right") - _input->get_action_strength("move_left");
            //velocity.y = _input->get_action_strength("move_down") - _input->get_action_strength("move_up");
        }
        
        if (velocity.length() > 0) {
            velocity = velocity.normalized() * speed;
            _animated_sprite->play();
        } else {
            _animated_sprite->stop();
        }

        godot::Vector2 position = get_position();
        position += velocity * (real_t) p_delta;
        position.x = godot::Math::clamp(position.x, (real_t)0.0, _screen_size.x);
        position.y = godot::Math::clamp(position.y, (real_t)0.0, _screen_size.y);
        set_position(position);
        
        if (velocity.x != 0) {
            _animated_sprite->set_animation("walk");
            _animated_sprite->set_flip_v(false);
            // See the note below about boolean assignment.
            _animated_sprite->set_flip_h(velocity.x < 0);
        } else if (velocity.y != 0) {
            _animated_sprite->set_animation("up");
            _animated_sprite->set_flip_v(velocity.y > 0);
        }

        timeout = false;
        _main->sem_physics->post();
        std::this_thread::sleep_for(std::chrono::nanoseconds(5000000 / _main->time_scale));
        _main->_observation_Function();
    }
}


// This code goes in `player.cpp`.
void Player::_on_Player_body_entered(godot::Node2D *_body) {
    //std::cout << "Player::_on_Player_body_entered" << std::endl;

    godot::String body_name = _body->get_name();

    //std::cout << "body_name: " << body_name << std::endl;
    //godot::Godot::print("body_name: " + godot::String(body_name));
    if (body_name.find("Coin") != std::string::npos) {
        //std::cout << "found!" << '\n';
        emit_signal("coin");
        _body->queue_free();
        //_body->_collision_shape->set_deferred("disabled", true);
    } else {
        hide(); // Player disappears after being hit.
        emit_signal("hit");
        // Must be deferred as we can't change physics properties on a physics callback.
        _collision_shape->set_deferred("disabled", true);
    }
}


// This code goes in `player.cpp`.
void Player::start(const godot::Vector2 p_position) {
    set_position(p_position);
    show();
    _collision_shape->set_disabled(false);
}
