/*!
 * sl_model.c
 *
 * Contains reinforcement learning model and manual model.
 * Takes state input and returns current sub task.
 *
 *  Created on: May 25, 2020
 *      Author: Mickey Beurskens
 */


#include "strategy_learner.h"
#include "sl_model.h"
#include "sl_includes.h"
#include "sl_shared_memory.h"

int get_state(pPE_InputStruct_t pIn, pPE_OutputStruct_t pOut,
		pSTP_TaskLearned pLR, pPE_sfun_global_data_t pGD,
		p_sl_model_training_data_t p_training_data)
{
	/* Local parameters */
	int state = 0;

	/* Set model input data */
	set_model_input_data(&(p_training_data->model_input), pIn->pTB, pIn->pTRCB,
			pIn->pSFB);
	p_training_data->time_stamp = get_time_stamp();

	/* Choose a learned task or a manual task */
	if (cur_t_info.manual_mode_flag == 1) {
		state = get_manual_state(pIn, pOut, pLR, pGD);
	}else {
		state = get_learned_state(pIn, pOut, pLR, pGD, p_training_data);
	}

	/* Set model output data and log all results */
	set_environment_data(&(p_training_data->env_data), pIn->pTB, pIn->pTRCB,
			pIn->pPlay, state);
	set_model_output_data(&(p_training_data->model_output), state);
	save_model_data(p_training_data, pIn->pTRCB);  // Log if data flag in sl_logging.c is 1
	return state;
}


static int get_learned_state(pPE_InputStruct_t pIn, pPE_OutputStruct_t pOut,
		pSTP_TaskLearned pLR, pPE_sfun_global_data_t pGD,
		p_sl_model_training_data_t p_training_data)
{
	// Write model input data to shared memory
	write_obs_shared_memory(p_training_data->model_input, &t_shm);

	// Read state data from shared memory
	return read_pred_shared_memory(&t_shm).role;
}


static int get_manual_state(pPE_InputStruct_t pIn, pPE_OutputStruct_t pOut,
		pSTP_TaskLearned pLR, pPE_sfun_global_data_t pGD)
{
	int boundary_condition = 0;
	int task_status = 0;
	static int state = SL_TASK_MOVE;
	static int prev_state = -1;

	if (pIn->pTRCB->turtleID == 2 && prev_state == SL_TASK_NONE){
		state = SL_TASK_INTERCEPT;
	}
	/* Intercept if no ball possession */
	switch (state)
	{
		case SL_TASK_PASS:
			if(get_other_active_tid(pIn) == pIn->pTB->CPBteam){
				state = SL_TASK_RECEIVE;
			}
			else if(pGD->PassStruct.Pass_giver_stage ==
					Pass_state_giving_failed) {
				state = SL_TASK_INTERCEPT;
			}
			else if (!check_ball_move(pIn) && !check_partner_move(pIn) &&
					pIn->pTRCB->turtleID == 2) {
				state = SL_TASK_INTERCEPT;
			}
			break;
		case SL_TASK_RECEIVE:
			if(pIn->pTRCB->turtleID == pIn->pTB->CPBteam){
				state = SL_TASK_PASS;
			} else if(pGD->PassStruct.Pass_receiver_stage ==
					Pass_state_receive_failed) {
				state = SL_TASK_INTERCEPT;
			} else if (!check_ball_move(pIn) && !check_partner_move(pIn) &&
					pIn->pTRCB->turtleID == 2) {
				state = SL_TASK_INTERCEPT;
			}
			break;
		case SL_TASK_MOVE:
			if(pIn->pTRCB->turtleID == pIn->pTB->CPBteam){
				state = SL_TASK_PASS;
			} else if(get_other_active_tid(pIn) == pIn->pTB->CPBteam) {
				state = SL_TASK_RECEIVE;
			}
			break;
		case SL_TASK_INTERCEPT:
			if(pIn->pTRCB->turtleID == pIn->pTB->CPBteam){
				state = SL_TASK_PASS;
			} else if(get_other_active_tid(pIn) == pIn->pTB->CPBteam){
				state = SL_TASK_RECEIVE;
			}
			break;
		default:
			state = SL_TASK_NONE;
			break;
	}
	prev_state = state;

	return state;
}


static int check_ball_move(pPE_InputStruct_t pIn) {
	double move_epsilon = 0.01;  // m/s

	if (pIn->pSFB->ball_xyzdxdydz.vx > move_epsilon){
		return 1;
	}
	if (pIn->pSFB->ball_xyzdxdydz.vy > move_epsilon){
		return 1;
	}
	if (pIn->pSFB->ball_xyzdxdydz.vz > move_epsilon){
		return 1;
	}
	return 0;
}


static int check_partner_move(pPE_InputStruct_t pIn) {
	double move_epsilon = 0.01; 	// m/s
	int partner_t_id = 3;
	if (pIn->pTRCB->turtleID == 3){
		partner_t_id = 2;
	}

	if (pIn->pTB->current_xyo_dot[partner_t_id-1].x > move_epsilon){
		return 1;
	}
	if (pIn->pTB->current_xyo_dot[partner_t_id-1].y > move_epsilon){
		return 1;
	}
	if (pIn->pTB->current_xyo_dot[partner_t_id-1].o > move_epsilon){
		return 1;
	}
	return 0;
}
