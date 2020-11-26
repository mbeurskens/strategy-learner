/*
 * sl_tasks.h
 *
 * Contains RL tasks and task helper functions.
 *
 *  Created on: May 25, 2020
 *      Author: Mickey Beurskens
 */

#ifndef TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_TASKS_H_
#define TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_TASKS_H_

#include "GeneralRobocupFunctions/general_robocup_functions.h"
#include "sl_data_types.h"

/* Reinforcement learning sub tasks. Can be called by RL algorithm. */
enum {
	SL_TASK_NONE = -1,
	SL_TASK_PASS,
	SL_TASK_RECEIVE,
	SL_TASK_MOVE,
	SL_TASK_INTERCEPT,

	/* Add every RL task above this one */
	SL_TASK_AMOUNT
};

/*! Reinforcement learning sub task.
 *
 * Used as a RL task in the strategy learner. Performs action and logs training
 * 	data.
 *
 * @param pIn Pointer to the general STP task input structure.
 * @param pOut Pointer to the general STP task output structure.
 * @param pTaskParam Pointer to task parameter of type
 * 			STP_TaskGivePassStruct_t.
 * @param pGD Pointer to global turtle data.
 * @return Task execution status */
int sub_task_pass(pPE_InputStruct_t pIn, pPE_OutputStruct_t pOut,
		void* pTaskParam, pPE_sfun_global_data_t pGD);

/*! Reinforcement learning sub task.
 *
 * Used as a RL task in the strategy learner. Performs action and logs training
 * 	data.
 *
 * @param pIn Pointer to the general STP task input structure.
 * @param pOut Pointer to the general STP task output structure.
 * @param pTaskParam Pointer to task parameter of type
 * 			STP_TaskMoveOrDribbleToTargetStruct_t.
 * @param pGD Pointer to global turtle data.
 * @return Task execution status */
int sub_task_receive(pPE_InputStruct_t pIn, pPE_OutputStruct_t pOut,
		void* pTaskParam, pPE_sfun_global_data_t pGD);

/*! Reinforcement learning sub task.
 *
 * Used as a RL task in the strategy learner. Performs action and logs training
 * 	data.
 *
 * @param pIn Pointer to the general STP task input structure.
 * @param pOut Pointer to the general STP task output structure.
 * @param pTaskParam Pointer to task parameter of type
 * 			STP_TaskMoveOrDribbleToTargetStruct_t.
 * @param pGD Pointer to global turtle data.
 * @return Task execution status */
int sub_task_move(pPE_InputStruct_t pIn, pPE_OutputStruct_t pOut,
		void* pTaskParam, pPE_sfun_global_data_t pGD);

/*! Reinforcement learning sub task.
 *
 * Used as a RL task in the strategy learner. Performs action and logs training
 *
 * @param pIn Pointer to the general STP task input structure.
 * @param pOut Pointer to the general STP task output structure.
 * @param pTaskParam Pointer to task parameter of type
 * 			STP_TaskInterceptBallStruct_t.
 * @param pGD Pointer to global turtle data.
 * @return Task execution status */
int sub_task_intercept(pPE_InputStruct_t pIn, pPE_OutputStruct_t pOut,
		void* pTaskParam, pPE_sfun_global_data_t pGD);

#endif /* TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_TASKS_H_ */
