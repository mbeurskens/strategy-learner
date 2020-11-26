/*!
 * strategy_learner.h
 *
 * Main file for the strategy learner. Interfaces with STP.
 *
 *  Created on: May 25, 2020
 *      Author: Mickey Beurskens
 */

#ifndef TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_STRATEGY_LEARNER_H_
#define TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_STRATEGY_LEARNER_H_

#include "GeneralRobocupFunctions/general_robocup_functions.h"
#include "sl_data_types.h"
#include "sl_data_types_msl.h"
#include "sl_shared_memory.h"

/*! STP interface to the strategy learner.
 *
 * Chooses a task based on a reinforcement learning algorithm or a manually
 * constructed task to imitate from.
 *
 * @param pIn Pointer to the general STP task input structure.
 * @param pOut Pointer to the general STP task output structure.
 * @param pTaskParam Pointer to task parameter of type
 * 			STP_TaskGivePassStruct_t.
 * @param pGD Pointer to global turtle data.
 * @return Task state. */
int set_strategy_learner_task(pPE_InputStruct_t pIn,
		pPE_OutputStruct_t pOut, pSTP_TaskLearned pLR,
		pPE_sfun_global_data_t pGD);

/*!
 * Clean shared memory for turtle with turtle_id.
 * @param turtle_id turtle ID;
 */
void clear_turtle_shm();

/*!
 * Start all shared memory services on the turtle.
 * @param turtle_id Current turtle ID.
 */
static void start_shm(int turtle_id);

/*! Apply boundary conditions.
 *
 * @param pIn Pointer to the general STP task input structure.
 * @param pOut Pointer to the general STP task output structure.
 * @param pTaskParam Pointer to task parameter of type STP_TaskLearned.
 * @param pGD Pointer to global turtle data.
 * @return 1 if a boundary condition has been applied. */
static int apply_boundary_conditions(pPE_InputStruct_t pIn,
		pPE_OutputStruct_t pOut, void* pTaskParam, pPE_sfun_global_data_t pGD);

/*! Return new turtle position outside boundary condition area given turtle ids.
 *
 * @param pIn Pointer to the general STP task input structure.
 * @param id_1 Turtle id for the first turtle.
 * @param id_2 Turtle id for the second turtle.
 * @return New turtle position outside of boundary condition area. */
static Pose_t get_new_turtle_position(pPE_InputStruct_t pIn, int id_1, int id_2,
		double min_distance);

/* Strategy learner global parameters */
info_shm i_shm;             // Shared memory struct for communicating turtle status
turtle_shm t_shm;           // Shared memory struct controlling turtle shm
int t_id;                   // Current turtle ID
static int t_shm_opened = 0; 	// True if shared memory had been opened
turtle_info cur_t_info; 	// Turtle info struct communicated over turtle shm
float total_rew;

#endif /* TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_STRATEGY_LEARNER_H_ */
