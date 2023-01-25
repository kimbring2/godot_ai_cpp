import gym
import numpy as np
import random
import matplotlib.pylab as plt
import cv2
import io 
import time

from dodgeCreep import dodgeCreepEnv

import dodgeCreep
#print("dodgeCreep.__file__: ", dodgeCreep.__file__)

if __name__ == '__main__':
	num_warmup = 1000
	num_train = 200000
	num_eval = 0
	buffer_length = 600000

	GODOT_BIN_PATH = "./DodgeCreep.x86_64"
	env_abs_path = "./DodgeCreep.pck"
	env = dodgeCreepEnv(exec_path=GODOT_BIN_PATH, env_path=env_abs_path, turbo_mode=True)

	num_states = env.observation_space.shape[0]
	num_actions = env.action_space.shape[0]

	#print("num_actions: ", num_actions)

	for episode in range(1000):
		print("episode: ", episode)

		state = env.reset()
		step = 0
		start = time.time()

		#fourcc = cv2.VideoWriter_fourcc(*'mp4v')
    	#video = cv2.VideoWriter('/home/kimbring2/dodge_the_creeps/images/video_' + str(episode_step) + '.avi', fourcc, 20, (128,128))
		while True:
			#print("step: ", step)
			action = random.randint(0,4)
			#action = 2

			#start = time.time()
			state_next, reward, done, _ = env.step(action)
			#print("done: ", done)
			#end = time.time()
			#print("elapsed time of loop: ", end - start)
			#print("")

			#print("state_next.shape: ", state_next.shape)
			#if reward != 0:
			#	print("reward: ", reward)
			#print("done: ", done)

			state_next = np.reshape(state_next, (128,128,3))
			state_next = state_next.astype(np.uint8)
			state_next = cv2.resize(state_next, dsize=(64,64), interpolation=cv2.INTER_CUBIC)
			state_next = 0.299*state_next[:,:,0] + 0.587*state_next[:,:,1] + 0.114*state_next[:,:,2]
			state_next[state_next < 100] = 0
			state_next[state_next >= 150] = 255
			state_next = np.array(state_next).astype(np.float32) / 255.0

			#cv2.imshow("state_next: ", state_next)
			#if cv2.waitKey(25) & 0xFF == ord("q"):
			#	cv2.destroyAllWindows()
			#video.write(obs1_)

			#print("")
			#time.sleep(0.1)
			step += 1
			#time.sleep(1)

			if done[0] == True:
			#if step == 200:
				print("step: ", step)
				end = time.time()
				print("elapsed time: ", end - start)
				print("")

				break
		
	env.close()
