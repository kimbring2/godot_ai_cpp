# Introduction
Training Godot game agent using the Deep Learning at the C++. It provides faster training speed than the GDScript.

# Game Description
The agent can obtain the score by collecting the coin during 20 second. The Action is consist of the left, right, up, and down.

<img src="images/godot_cpp_example.gif " width="250">

# Requirements
1. Godot 3 version.
2. The C++ code of this repository is based on [official tutorial of Godot](https://docs.godotengine.org/en/stable/getting_started/first_2d_game/index.html).
3. This project uses the [GodotAIGym](https://github.com/lupoglaz/GodotAIGym) to connect the Godot with Python.
4. The GDScript version code is available in [DogdeCreepTut](https://github.com/kimbring2/GodotAIGym/tree/uint_type_update/Tutorials/DogdeCreepTut).

**NOTE:**
- You need copy the [GodotModule](https://github.com/lupoglaz/GodotAIGym/tree/master/GodotModule) first before building this project.
- The libtorch, godot-cpp, and game exe file are excluded from this repositoy because of uploading size limitation.

# How To Run
1. Build the project.
```
$ scons platform=linux
```

2. Move to project folder.
```
$ cd project
```

3. Start the Learner.
```
$ python learner.py --env_num 1
$ python3.7 learner.py --env_num 1
```

4. Start the Actor
```
$ python actor.py --env_id your_env_id --learner_ip your_learner_ip
$ python3.7 actor.py --env_id 0 --learner_ip 192.168.1.3
```

5. Evaluate the trained model.
```
$ python run_evaluation.py --workspace_path /your/path --model_name reinforcement_model_xxxxx
$ python3.7 run_evaluation.py --workspace_path /home/kimbring2/dodge_the_creeps/project/ --model_name reinforcement_model_13000
```

**NOTE:**
- [Pretrained model](https://drive.google.com/drive/folders/1m2-JgGyMyhaTNKg3zcR0SGqQuKsvVf0w?usp=sharing) is avialable from my Google Drive.

# Speed Up The Game Speed
You can change the game speed by setting the turbo_mode flag of the dodgeCreepEnv. The game speed will be doubled if turbo_mode is True.

```
env = dodgeCreepEnv(exec_path=GODOT_BIN_PATH, env_path=env_abs_path, turbo_mode=False)
```
