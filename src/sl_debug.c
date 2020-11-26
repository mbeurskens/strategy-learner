#include <stdio.h>
#include "sl_debug.h"
#include "sl_includes.h"
#include "GeneralRobocupFunctions/general_robocup_functions.h"

#ifndef LEVEL_DEBUG
	#define LEVEL_DEBUG 	1
	#define LEVEL_INFO 		2
	#define DEBUG_LEVEL 	1 	// Set to appropriate logging level
#endif

void debug_print(p_sl_model_training_data_t p_training_data)
{
#if DEBUG_LEVEL >= LEVEL_DEBUG
	cur_print_line = 1;
	printl(cur_print_line, "Settings");
	cur_print_line += 1;
	printl(cur_print_line, "Manual mode: %d", cur_t_info.manual_mode_flag);
	cur_print_line += 1;
	printl(cur_print_line, "Data dir: %s", cur_t_info.turtle_data_save_dir);
	cur_print_line += 1;
	printl(cur_print_line, "Running flag: %d", cur_t_info.running_flag);
	cur_print_line += 1;

	printl(cur_print_line, "Training Data");
	cur_print_line += 1;
	printl(cur_print_line, "Total reward: %f", total_rew);
	cur_print_line += 1;
	static int passes = 0;
	passes += p_training_data->env_data.n_balls_passed;
	printl(cur_print_line, "Total passes: %d", passes);
	cur_print_line += 1;
	printl(cur_print_line, "Time stamp: %ul",
			(long)p_training_data->time_stamp);
	cur_print_line += 1;
	debug_print_model_input(&p_training_data->model_input);
	debug_print_environment_data(&p_training_data->env_data);
	debug_print_model_output(&p_training_data->model_output);
#endif
}

static void debug_print_model_input(p_sl_model_input_t p_model_input)
{
	/* Debug prints */
	printl(cur_print_line, "Input data");
	cur_print_line += 1;
	printl(cur_print_line, "Own pos x, y, o: %f, %f, %f",
			p_model_input->own_xyo_x,
			p_model_input->own_xyo_y, p_model_input->own_xyo_o);
	cur_print_line += 1;
	printl(cur_print_line, "Own vel x, y, o: %f, %f, %f",
			p_model_input->own_xyo_dot_x,
			p_model_input->own_xyo_dot_y, p_model_input->own_xyo_dot_o);
	cur_print_line += 1;
	printl(cur_print_line, "Peer x, y, o: %f, %f, %f",
			p_model_input->peer_xyo_x,
			p_model_input->peer_xyo_y,
			p_model_input->peer_xyo_o);
	cur_print_line += 1;
	printl(cur_print_line, "Peer vel x, y, o: %f, %f, %f",
			p_model_input->peer_xyo_dot_x,
			p_model_input->peer_xyo_dot_y,
			p_model_input->peer_xyo_dot_o);
	cur_print_line += 1;
	printl(cur_print_line, "Ball pos x, y, z: %f, %f, %f",
			p_model_input->ball_xyz_x,
			p_model_input->ball_xyz_y,
			p_model_input->ball_xyz_z);
	cur_print_line += 1;
	printl(cur_print_line, "Ball vel x, y, z: %f, %f, %f",
			p_model_input->ball_xyz_dot_x,
			p_model_input->ball_xyz_dot_y,
			p_model_input->ball_xyz_dot_z);
	cur_print_line += 1;
	printl(cur_print_line, "Own ball possession: %f, peer: %f",
			p_model_input->own_ball_possession,
			p_model_input->peer_ball_possession);
	cur_print_line += 1;
}


static void debug_print_environment_data(p_sl_environment_data_t p_env_data)
{
	/* Debug prints */
	printl(cur_print_line, "Environment data");
	cur_print_line += 1;
	printl(cur_print_line, "Reward: %f", p_env_data->reward);
	cur_print_line += 1;
	printl(cur_print_line, "Pass: %d", p_env_data->n_balls_passed);
	cur_print_line += 1;
}


static void debug_print_model_output(p_sl_model_output_t p_model_output)
{
	/* Debug prints */
	printl(cur_print_line, "Output data");
	cur_print_line += 1;

	int pass = 0;
	int receive = 0;
	int move = 0;
	int intercept = 0;
	if(p_model_output->role == SL_TASK_PASS){
		pass = 1;
	} else if(p_model_output->role == SL_TASK_RECEIVE){
		receive = 1;
	} else if(p_model_output->role == SL_TASK_MOVE){
		move = 1;
	} else if(p_model_output->role == SL_TASK_INTERCEPT){
		intercept = 1;
	}
	printl(cur_print_line, "Agent role pass: %i, receive: %i, move: %i, "
			"intercept: %i", pass, receive, move, intercept);
	cur_print_line += 1;
}
