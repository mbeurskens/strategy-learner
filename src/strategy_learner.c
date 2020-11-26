#include "sl_includes.h"
#include "strategy_learner.h"
#include "sl_shared_memory.h"


int gmax(float a, float b){
	if(a >= b){
		return a;
	}
	return b;
}

int set_strategy_learner_task(pPE_InputStruct_t pIn,
		pPE_OutputStruct_t pOut, pSTP_TaskLearned pLR,
		pPE_sfun_global_data_t pGD)
{
	int task_status = 0;
	int state = SL_TASK_NONE;
	int boundary_condition = 0;

	// Start shared communication as server
	if (!t_shm_opened) {
		t_shm_opened = 1;

		start_shm(pIn->pTRCB->turtleID);
    }
	if (t_shm_opened) {
		cur_t_info = read_info_shm(&i_shm);
	}

	/* Initialize training data and set input */
	sl_model_training_data_t training_data;

	state = get_state(pIn, pOut, pLR, pGD, &training_data);

	/* Check for boundary conditions and execute corresponding behavior */
	boundary_condition = apply_boundary_conditions(pIn, pOut, pLR, pGD);

	/* If a boundary condition applies, override normal behavior */
	if (!boundary_condition) {
		/* Normal behavior, intercept if no ball possession */
		switch (state)
		{
			case SL_TASK_PASS:
				task_status = sub_task_pass(pIn, pOut, &(pLR->stp_pass), pGD);
				break;
			case SL_TASK_RECEIVE:
				task_status = sub_task_receive(pIn, pOut, &(pLR->stp_receive),
						pGD);
				break;
			case SL_TASK_MOVE:
				task_status = sub_task_move(pIn, pOut, &(pLR->stp_move), pGD);
				break;
			case SL_TASK_INTERCEPT:
				task_status = sub_task_intercept(pIn, pOut,
								&(pLR->stp_intercept), pGD);
				break;
			default:
				break;
		}
	}

	debug_print(&training_data);
	return task_status;
}

void clear_turtle_shm()
{
    // First communicate that info server will be shut down
	cur_t_info.running_flag = 0;
    write_info_shm_blocking(cur_t_info, &i_shm);
    clear_info_shm(&i_shm);

    // Close turtle shared memory
    clear_shared_memory(t_id, &t_shm);
    total_rew = 0;
}

static void start_shm(int turtle_id)
{
    // Set current turtle id in strategy learner
    t_id = turtle_id;

    // Run shared memory as client and write status to info server
    initialize_info_shm(t_id, 1, &i_shm);
    cur_t_info = read_info_shm(&i_shm);
    cur_t_info.running_flag = 1;
    write_info_shm(cur_t_info, &i_shm);

    // Run turtle shared memory for data communication
    initialize_shared_memory(t_id, 0, &t_shm);
}

static int apply_boundary_conditions(pPE_InputStruct_t pIn,
		pPE_OutputStruct_t pOut,
		void* pTaskParam, pPE_sfun_global_data_t pGD)
{
	int boundary_condition_set = 0;
	pSTP_TaskLearned pLR = (pSTP_TaskLearned) pTaskParam;

	/* Calculate euclidean distance between two turtles */
	double min_eucl_dist = 2.5;
	if (get_euclidean_distance_by_tid(pIn, pIn->pTRCB->turtleID,
			get_other_active_tid(pIn)) < min_eucl_dist) {
		boundary_condition_set = 1;
		/* Move away from other turtle */
		pLR->stp_move.Target = get_new_turtle_position(pIn,
				pIn->pTRCB->turtleID, get_other_active_tid(pIn),
				min_eucl_dist);
		sub_task_move(pIn, pOut, &(pLR->stp_move), pGD);
	}

	return boundary_condition_set;
}

static Pose_t get_new_turtle_position(pPE_InputStruct_t pIn, int id_1, int id_2,
		double min_distance)
{
	/* Move the turtle in the opposite direction of peer */
	Pose_t new_turtle_pos = {{0.0,0.0,0.0}};
	int own_t_index = id_1 - 1;
	int peer_t_index = id_2 - 1;
	if (own_t_index >= 0 && peer_t_index >= 0) {
		Pose_t own_pos = pIn->pTB->current_xyo[own_t_index];
		Pose_t peer_pos = pIn->pTB->current_xyo[peer_t_index];
		double angle = atan2(peer_pos.y - own_pos.y, peer_pos.x - own_pos.x);
		new_turtle_pos.x = -cos(angle)*min_distance;
		new_turtle_pos.y = -sin(angle)*min_distance;
	}
	return new_turtle_pos;
}
