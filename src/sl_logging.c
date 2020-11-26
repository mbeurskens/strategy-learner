#include <time.h>
#include <math.h>
#include "sl_logging.h"
#include "sl_includes.h"
#include "Strategy/src/STP/tasks/tasks_general.h"
#include "Strategy/src/STP/tasks/tasks_gameplay.h"
#include "Strategy/src/STP/tasks/tasks_positioning.h"
#include "GeneralRobocupFunctions/general_robocup_functions.h"

#ifndef LOG_DATA_FLAG
	#define LOG_DATA_FLAG 	1 	// Toggle to log model data
#endif

void set_model_input_data(p_sl_model_input_t p_model_input,
		pteambus_t p_team_bus, pTRCbus_t p_trc_bus,
		psensorfusionbus_t p_sensor_fusion_bus)
{
	/* Assign the model input data struct */
	set_own_model_input_data(p_model_input, p_sensor_fusion_bus);
	set_ball_model_input_data(p_model_input, p_sensor_fusion_bus);
	set_peer_model_input_data(p_model_input, p_team_bus,
			p_trc_bus, p_sensor_fusion_bus);
	set_ball_possession_model_input_data(p_model_input, p_team_bus, p_trc_bus);
}

static void set_own_model_input_data(p_sl_model_input_t p_model_input,
		psensorfusionbus_t p_sensor_fusion_bus)
{
	p_model_input->own_xyo_x = p_sensor_fusion_bus->cur_xyo.x/(FIELDWIDTH/2);
	p_model_input->own_xyo_y = p_sensor_fusion_bus->cur_xyo.y/(FIELDLENGTH/2);
	p_model_input->own_xyo_o = remainder(p_sensor_fusion_bus->cur_xyo.o/(2*PI), 1.0);
	p_model_input->own_xyo_dot_x = p_sensor_fusion_bus->cur_xyo_dot.x/MAX_MOVE_VELOCITY;
	p_model_input->own_xyo_dot_y = p_sensor_fusion_bus->cur_xyo_dot.y/MAX_MOVE_VELOCITY;
	p_model_input->own_xyo_dot_o = p_sensor_fusion_bus->cur_xyo_dot.o/MAX_MOVE_VELOCITY;
}

static void set_ball_model_input_data(p_sl_model_input_t p_model_input,
		psensorfusionbus_t p_sensor_fusion_bus)
{
	float max_ball_height = 2;
	float max_ball_vel = 14;
	p_model_input->ball_xyz_x = (p_sensor_fusion_bus->ball_xyzdxdydz.pos.x -
			 p_sensor_fusion_bus->cur_xyo.x)/(FIELDWIDTH/2);
	p_model_input->ball_xyz_y = (p_sensor_fusion_bus->ball_xyzdxdydz.pos.y -
			p_sensor_fusion_bus->cur_xyo.y)/(FIELDLENGTH/2);
	p_model_input->ball_xyz_z = p_sensor_fusion_bus->ball_xyzdxdydz.pos.z/max_ball_height;
	p_model_input->ball_xyz_dot_x = (p_sensor_fusion_bus->ball_xyzdxdydz.vel.x -
			p_sensor_fusion_bus->cur_xyo_dot.x)/max_ball_vel;
	p_model_input->ball_xyz_dot_y = (p_sensor_fusion_bus->ball_xyzdxdydz.vel.y -
			p_sensor_fusion_bus->cur_xyo_dot.y)/max_ball_vel;
	p_model_input->ball_xyz_dot_z = p_sensor_fusion_bus->ball_xyzdxdydz.vel.z/max_ball_vel;
}

static void set_peer_model_input_data(
		p_sl_model_input_t p_model_input, pteambus_t p_team_bus,
		pTRCbus_t p_trc_bus, psensorfusionbus_t p_sensor_fusion_bus)
{
	int array_size = sizeof(p_team_bus->isActivePeer)/sizeof(int);
	int i = 0;
	for(i=0; i < array_size; i++){
		/* Assign peer location, which is -100 if not active */
		p_model_input->peer_xyo_x = (p_team_bus->current_xyo[i].x -
				p_sensor_fusion_bus->cur_xyo.x)/(FIELDWIDTH);
		p_model_input->peer_xyo_y = (p_team_bus->current_xyo[i].y -
				p_sensor_fusion_bus->cur_xyo.y)/(FIELDLENGTH);
		p_model_input->peer_xyo_o = remainder((p_team_bus->current_xyo[i].o -
				p_sensor_fusion_bus->cur_xyo.o)/(2*PI), 1.0);
		p_model_input->peer_xyo_dot_x = (p_team_bus->current_xyo_dot[i].x -
				p_sensor_fusion_bus->cur_xyo_dot.x)/MAX_MOVE_VELOCITY;
		p_model_input->peer_xyo_dot_y = (p_team_bus->current_xyo_dot[i].y -
				p_sensor_fusion_bus->cur_xyo_dot.y)/MAX_MOVE_VELOCITY;
		p_model_input->peer_xyo_dot_o = (p_team_bus->current_xyo_dot[i].o -
				p_sensor_fusion_bus->cur_xyo_dot.o)/MAX_MOVE_VELOCITY;

		/* If we find the first active peer, return from function */
		if(p_trc_bus->turtleID != i+1 && p_team_bus->isActivePeer[i] == 1){
			return;
		}
	}
}


static void set_ball_possession_model_input_data(
		p_sl_model_input_t p_model_input, pteambus_t p_team_bus,
		pTRCbus_t p_trc_bus)
{
	p_model_input->own_ball_possession = 0.0;
	p_model_input->peer_ball_possession = 0.0;

	int array_size = sizeof(p_team_bus->isActivePeer)/sizeof(int);
	int i = 0;
	for(i=0; i < array_size; i++){
		/* If current turtle or peer has the ball, set ball possession */
		if(p_team_bus->CPBteam == p_trc_bus->turtleID){
			p_model_input->own_ball_possession = 1.0;
			return;
		} else if (p_team_bus->CPBteam == i+1) {
			p_model_input->peer_ball_possession = 1.0;
			return;
		}
	}
}

void set_environment_data(p_sl_environment_data_t env_data,
		pteambus_t p_team_bus, pTRCbus_t p_trc_bus,
		pStpPlayStruct_t p_play_struct, int state)
{
	env_data->reward = 0;
	env_data->n_balls_passed = 0;
	int passed_flag = get_pass(p_team_bus, p_trc_bus);
	set_reward(env_data, p_team_bus, p_trc_bus, passed_flag);
	set_n_balls_passed(env_data, p_team_bus, p_trc_bus, passed_flag);
}

static void set_reward(p_sl_environment_data_t env_data, pteambus_t p_team_bus,
		pTRCbus_t p_trc_bus, int passed_flag) {
	if (passed_flag) {
		env_data->reward += 1.0;
	}
	//env_data->reward += get_ball_collect_reward(p_team_bus, p_trc_bus);

	total_rew += env_data->reward;
}

static float get_ball_collect_reward(pteambus_t p_team_bus, pTRCbus_t p_trc_bus)
{
	// Get a reward when picking up ball
	static int ball_picked_up = 0;
	float reward = 0;

	if (p_team_bus->CPBteam == p_trc_bus->turtleID && !ball_picked_up) {
		ball_picked_up = 1;
		reward = 1;
	}

	return reward;
}

static void set_n_balls_passed(p_sl_environment_data_t env_data,
		pteambus_t p_team_bus, pTRCbus_t p_trc_bus, int passed_flag) {
	env_data->n_balls_passed += passed_flag;
}


static int get_pass(pteambus_t p_team_bus, pTRCbus_t p_trc_bus) {
	/* Because of static values only call this once per frame! */
	const int max_pass_time = 5e6;
	if (pass_given(p_team_bus, p_trc_bus, max_pass_time) ||
			pass_received(p_team_bus, p_trc_bus, max_pass_time)) {
		return 1;
	}
	return 0;
}

static int pass_given(pteambus_t p_team_bus, pTRCbus_t p_trc_bus,
		int max_pass_time) {
	static int pass_ready_flag = 0;
	static int pass_giving_flag = 0;
	static int pass_start_time = 0;
	float passed = 0;

	int cur_time = get_time_stamp();

	if (p_team_bus->CPBteam == p_trc_bus->turtleID && !pass_giving_flag) {
		pass_ready_flag = 1;
		pass_giving_flag = 0;
	}

	if (pass_ready_flag) {
		if (p_team_bus->CPBteam < 1) {
			pass_ready_flag = 0;
			pass_giving_flag = 1;
			pass_start_time = get_time_stamp();
		}
	}

	if (pass_giving_flag && (cur_time - pass_start_time < max_pass_time)) {
		if (p_team_bus->CPBteam ==
				get_other_active_tid_from_bus(p_team_bus, p_trc_bus)) {
			pass_giving_flag = 0;
			passed = 1;
		}
	} else {
		pass_giving_flag = 0;
	}

	return passed;
}

static int pass_received(pteambus_t p_team_bus, pTRCbus_t p_trc_bus,
		int max_pass_time) {
	static int receive_ready_flag = 0;
	static int receive_receiving_flag = 0;
	static int pass_start_time = 0;
	int received = 0;
	int cur_time = get_time_stamp();

	if (p_team_bus->CPBteam == get_other_active_tid_from_bus(p_team_bus, p_trc_bus) &&
			!receive_receiving_flag) {
		receive_ready_flag = 1;
		receive_receiving_flag = 0;
	}

	if (receive_ready_flag) {
		if (p_team_bus->CPBteam < 1) {
			receive_ready_flag = 0;
			receive_receiving_flag = 1;
			pass_start_time = get_time_stamp();
		}
	}

	if (receive_receiving_flag && (cur_time - pass_start_time < max_pass_time)) {
		if (p_team_bus->CPBteam == p_trc_bus->turtleID) {
			receive_receiving_flag = 0;
			received = 1;
		}
	} else {
		receive_receiving_flag = 0;
	}

	return received;
}

void set_model_output_data(p_sl_model_output_t p_model_output, int state)
{
	p_model_output->role = state;
	p_model_output->action_space_size = SL_TASK_AMOUNT;
	p_model_output->vpg_value = read_pred_shared_memory(&t_shm).vpg_value;
}


void save_model_data(p_sl_model_training_data_t p_training_data,
		pTRCbus_t p_trc_bus)
{
#if LOG_DATA_FLAG > 0
	static FILE *p_model_log_file = NULL; 	// Log file for model training data
	if(p_model_log_file == NULL && p_trc_bus->turtleID > 0 &&
			strcmp(cur_t_info.turtle_data_save_dir, "")){
		/* File opened here because ID not available during initialization */
		p_model_log_file = open_log_file(p_trc_bus);
	}else if(p_model_log_file != NULL){
		/*Time stamp*/
		fwrite(&p_training_data->time_stamp,
				sizeof(uint64_t), 1, p_model_log_file);

		/* Model input */
		fwrite(&p_training_data->model_input.own_xyo_x,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.own_xyo_y,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.own_xyo_o,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.own_xyo_dot_x,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.own_xyo_dot_y,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.own_xyo_dot_o,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.peer_xyo_x,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.peer_xyo_y,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.peer_xyo_o,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.peer_xyo_dot_x,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.peer_xyo_dot_y,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.peer_xyo_dot_o,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.ball_xyz_x,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.ball_xyz_y,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.ball_xyz_z,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.ball_xyz_dot_x,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.ball_xyz_dot_y,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.ball_xyz_dot_z,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.own_ball_possession,
				sizeof(double), 1, p_model_log_file);
		fwrite(&p_training_data->model_input.peer_ball_possession,
				sizeof(double), 1, p_model_log_file);

		/* Environment data */
		fwrite(&p_training_data->env_data.reward,
				sizeof(float), 1, p_model_log_file);
		fwrite(&p_training_data->env_data.n_balls_passed,
				sizeof(int), 1, p_model_log_file);

		/* Model output */
		fwrite(&p_training_data->model_output.role,
				sizeof(int), 1, p_model_log_file);
		fwrite(&p_training_data->model_output.action_space_size,
				sizeof(int), 1, p_model_log_file);
		fwrite(&p_training_data->model_output.vpg_value,
				sizeof(float), 1, p_model_log_file);
	}
#endif
}


static FILE* open_log_file(pTRCbus_t p_trc_bus)
{
	/* Initialize variables for filename */
	//printf("WARNING: Opening log file for writing strategy learner data.\n");
	//printf("\tMake sure your system is not flooded by data!\n");
	char file_name[512] = "";
	char file_path[512];
	char turtle_id[8];
	char time_stamp[32];
	FILE *p_model_log_file;
	sprintf(turtle_id, "%d", p_trc_bus->turtleID);
	sprintf(time_stamp, "%d", (int)time(NULL));
	// strncpy(file_path, cur_t_info.turtle_data_save_dir, 512);

	/* Open file */
	strcat(file_name, cur_t_info.turtle_data_save_dir);
	strcat(file_name, "/model_data_t");
	strcat(file_name, turtle_id);
	strcat(file_name, "_");
	strcat(file_name, time_stamp);
	strcat(file_name, ".mb");
	p_model_log_file = fopen(file_name, "wb");
    if (p_model_log_file == NULL) {
       perror("Error opening file: ");
       return NULL;
    }
    return p_model_log_file;
}


int64_t get_time_stamp()
{
	struct timespec tms;

	/* The C11 way */
	/* if (! timespec_get(&tms, TIME_UTC)) { */

	/* POSIX.1-2008 way */
	if (clock_gettime(CLOCK_REALTIME,&tms)) {
		return -1;
	}
	/* seconds, multiplied with 1 million */
	int64_t micros = tms.tv_sec * 1000000;
	/* Add full microseconds */
	micros += tms.tv_nsec/1000;
	/* round up if necessary */
	if (tms.tv_nsec % 1000 >= 500) {
		++micros;
	}
	return micros;
}


//static void close_strategy_learner()
//{
//	if(p_model_log_file != NULL){
//		fclose(p_model_log_file);
//	}
//}
