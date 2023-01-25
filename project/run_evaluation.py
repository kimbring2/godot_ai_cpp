import collections
import gym
import numpy as np
import statistics
import tensorflow as tf
import tqdm
import glob
import random
import cv2
import tensorflow_probability as tfp
import argparse
import os
from matplotlib import pyplot as plt
from tensorflow.keras import layers
from typing import Any, List, Sequence, Tuple
from absl import flags

from dodgeCreep import dodgeCreepEnv

import network

tfd = tfp.distributions

parser = argparse.ArgumentParser(description='DodgeCreep Evaluation')

parser.add_argument('--workspace_path', type=str, help='root directory of project')
parser.add_argument('--model_name', type=str, help='name of saved model')
parser.add_argument('--gpu_use', type=bool, default=False, help='use gpu')

arguments = parser.parse_args()

if arguments.gpu_use == True:
    gpus = tf.config.experimental.list_physical_devices('GPU')
    tf.config.experimental.set_virtual_device_configuration(gpus[0],
                [tf.config.experimental.VirtualDeviceConfiguration(memory_limit=3000)])
else:
  os.environ['CUDA_VISIBLE_DEVICES'] = '-1'

workspace_path = arguments.workspace_path

num_actions = 4
num_hidden_units = 512

model = network.ActorCritic(num_actions, num_hidden_units)
model.load_weights("model/" + arguments.model_name)

#seed = 980
#tf.random.set_seed(seed)
#np.random.seed(seed)

# Create the environment
GODOT_BIN_PATH = "./DodgeCreep.x86_64"
env_abs_path = "./DodgeCreep.pck"
env = dodgeCreepEnv(exec_path=GODOT_BIN_PATH, env_path=env_abs_path, turbo_mode=True)

reward_sum = 0

for i_episode in range(0, 10000):
    state = env.reset()

    state = np.reshape(state, (128,128,3))
    state = np.array(state).astype(np.uint8)
    state = cv2.resize(state, dsize=(64,64), interpolation=cv2.INTER_CUBIC)
    state = 0.299*state[:,:,0] + 0.587*state[:,:,1] + 0.114*state[:,:,2]
    state[state < 100] = 0
    state[state >= 150] = 255
    state = np.array(state).astype(np.float32) / 255.0

    memory_state = tf.zeros([1,128], dtype=np.float32)
    carry_state = tf.zeros([1,128], dtype=np.float32)
    step = 0
    while True:
        step += 1

        action_logits, _, memory_state, carry_state = model(tf.expand_dims(state, 0), memory_state, carry_state)
        action_dist = tfd.Categorical(logits=action_logits[0])
        action = int(action_dist.sample())

        next_state, reward, done, info = env.step(action)

        next_state = np.reshape(next_state, (128,128,3))
        next_state = np.array(next_state).astype(np.uint8)
        next_state = cv2.resize(next_state, dsize=(64,64), interpolation=cv2.INTER_CUBIC)
        next_state = 0.299*next_state[:,:,0] + 0.587*next_state[:,:,1] + 0.114*next_state[:,:,2]
        next_state[next_state < 100] = 0
        next_state[next_state >= 150] = 255
        next_state = np.array(next_state).astype(np.float32) / 255.0

        #cv2.imshow("next_state: ", next_state)
        #if cv2.waitKey(25) & 0xFF == ord("q"):
        #    cv2.destroyAllWindows()

        reward_sum += reward[0]
        #print("reward_sum: ", reward_sum)

        state = next_state
        if done[0]:
            print("Total reward: {:.2f},  Total step: {:.2f}".format(reward_sum, step))
            step = 0
            reward_sum = 0  
            break

env.close()
