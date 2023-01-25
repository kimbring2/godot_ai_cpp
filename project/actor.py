import collections
import zmq
import gym
import numpy as np
import statistics
import tqdm
import glob
import random
import tensorflow as tf
import cv2
import argparse
import os
from matplotlib import pyplot as plt
from typing import Any, List, Sequence, Tuple
from absl import flags
from dodgeCreep import dodgeCreepEnv

parser = argparse.ArgumentParser(description='DodgeCreep IMPALA Actor')
parser.add_argument('--env_id', type=int, default=0, help='ID of environment')
parser.add_argument('--learner_ip', type=str, default=0, help='IP of learner')
arguments = parser.parse_args()

os.environ['CUDA_VISIBLE_DEVICES'] = '-1'

writer = tf.summary.create_file_writer("tensorboard")

context = zmq.Context()

#  Socket to talk to server
print("Connecting to hello world server…")
socket = context.socket(zmq.REQ)
#socket.connect("tcp://localhost:" + str(5555 + arguments.env_id))
socket.connect("tcp://" + arguments.learner_ip + ":" + str(5555 + arguments.env_id))

num_actions = 4
state_size = (64,64,1)

# Create the environment
GODOT_BIN_PATH = "./DodgeCreep.x86_64"
env_abs_path = "./DodgeCreep.pck"
env = dodgeCreepEnv(exec_path=GODOT_BIN_PATH, env_path=env_abs_path, turbo_mode=True)

scores = []
episodes = []
average = []
for episode_step in range(0, 2000000):
    obs = env.reset()

    obs = np.reshape(obs, (128,128,3))
    obs = np.array(obs).astype(np.uint8)
    obs = cv2.resize(obs, dsize=(64,64), interpolation=cv2.INTER_CUBIC)
    obs = 0.299*obs[:,:,0] + 0.587*obs[:,:,1] + 0.114*obs[:,:,2]
    obs[obs < 100] = 0
    obs[obs >= 150] = 255
    obs = np.array(obs).astype(np.float32) / 255.0

    state = obs
    
    done = False
    reward = 0.0
    reward_sum = 0
    step = 0
    #if episode_step % 5 == 0:
    #    fourcc = cv2.VideoWriter_fourcc(*'mp4v')
    #    video = cv2.VideoWriter('/home/kimbring2/dodge_the_creeps/images/video_' + str(episode_step) + '.avi', fourcc, 20, (128,128))

    while True:
        try:
            step += 1

            state_reshaped = np.reshape(state, (1,*state_size)) 

            env_output = {"env_id": np.array([arguments.env_id]), 
                          "reward": reward / 10.0,
                          "done": done, 
                          "observation": state_reshaped}
            socket.send_pyobj(env_output)
            action = int(socket.recv_pyobj()['action'])

            obs1, reward, done, _ = env.step(action)
            reward = reward[0]
            done = done[0]

            obs1_ = np.reshape(obs1, (128,128,3))
            obs1 = np.array(obs1_).astype(np.uint8)
            obs1 = cv2.resize(obs1, dsize=(64,64), interpolation=cv2.INTER_CUBIC)
            obs1 = 0.299*obs1[:,:,0] + 0.587*obs1[:,:,1] + 0.114*obs1[:,:,2]
            obs1[obs1 < 100] = 0
            obs1[obs1 >= 150] = 255
            obs1 = np.array(obs1).astype(np.float32) / 255.0

            #print("episode_step: ", episode_step)
            #print("step: ", step)
            #if episode_step % 5 == 0:
                #cv2.imwrite("/home/kimbring2/dodge_the_creeps/images/image-" + str(episode_step) + "-" + str(step) + ".jpg", obs1_)
            #    video.write(obs1_)

            #if arguments.env_id == 0: 
            #    cv2.imshow("obs1: ", obs1)
            #    if cv2.waitKey(25) & 0xFF == ord("q"):
            #        cv2.destroyAllWindows()

            next_state = obs1

            reward_sum += reward
            state = next_state
            if done:
                if arguments.env_id == 0:
                    scores.append(reward_sum)
                    episodes.append(episode_step)
                    average.append(sum(scores[-50:]) / len(scores[-50:]))

                    with writer.as_default():
                        tf.summary.scalar("average_reward", average[-1], step=episode_step)
                        writer.flush()

                    print("average_reward: " + str(average[-1]))
                else:
                    print("reward_sum: " + str(reward_sum))

                #if episode_step % 5 == 0:
                #    video.release()

                break

        except (tf.errors.UnavailableError, tf.errors.CancelledError):
            logging.info('Inference call failed. This is normal at the end of training.')

env.close()
