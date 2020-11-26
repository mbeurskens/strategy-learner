/*
 * sl_model.h
 *
 * Contains reinforcement learning model and manual model.
 * Takes state input and returns current sub task.
 *
 *  Created on: May 25, 2020
 *      Author: Mickey Beurskens
 */

#ifndef TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_MODEL_H_
#define TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_MODEL_H_

#include "GeneralRobocupFunctions/general_robocup_functions.h"
#include "sl_data_types.h"
#include "sl_data_types_msl.h"

/*!
 * Get strategy learner task state.
 *
 * Chooses a task to execute. Chooses tasks manually when flag is set for
 * immitation learning. It chooses between available SL tasks.
 *
 * @param pIn Pointer to the general STP task input structure.
 * @param pOut Pointer to the general STP task output structure.
 * @param pTaskParam Pointer to task parameter of type
 * 			STP_TaskGivePassStruct_t.
 * @param pGD Pointer to global turtle data.
 * @param p_training_data Pointer to data structure storing data used for
 * 			machine learning model training.
 * @return Current learned RL task. */
int get_state(pPE_InputStruct_t pIn, pPE_OutputStruct_t pOut,
		pSTP_TaskLearned pLR, pPE_sfun_global_data_t pGD,
		p_sl_model_training_data_t p_training_data);

/*!
 * Get reinforcement learning based RL task state.
 *
 * Chooses a task based on a reinforcement learning algorithm based on
 * the current situation and previous training. It chooses between available
 * SL sub tasks.
 *
 * @param pIn Pointer to the general STP task input structure.
 * @param pOut Pointer to the general STP task output structure.
 * @param pTaskParam Pointer to task parameter of type
 * 			STP_TaskGivePassStruct_t.
 * @param pGD Pointer to global turtle data.
 * @param p_training_data Pointer to data structure storing data used for
 * 			machine learning model training.
 * @return Current learned SL task. */
static int get_learned_state(pPE_InputStruct_t pIn, pPE_OutputStruct_t pOut,
		pSTP_TaskLearned pLR, pPE_sfun_global_data_t pGD,
		p_sl_model_training_data_t p_training_data);

/*!
 * Get manually constructed RL task state.
 *
 * A manually built task with simple behavior that logs training data. Training
 * data is then used as a starting point for strategy learner training.
 *
 * @param pIn Pointer to the general STP task input structure.
 * @param pOut Pointer to the general STP task output structure.
 * @param pTaskParam Pointer to task parameter of type
 * 			STP_TaskGivePassStruct_t.
 * @param pGD Pointer to global turtle data.
 * @return Current manual SL task state. */
static int get_manual_state(pPE_InputStruct_t pIn, pPE_OutputStruct_t pOut,
		pSTP_TaskLearned pLR, pPE_sfun_global_data_t pGD);

/*!
 * Check if ball currently moves within a margin epsilon.
 *
 * @param pIn Pointer to the general STP task input structure.
 * 			Contains data busses.
 * @return True if ball moves. */
static int check_ball_move(pPE_InputStruct_t pIn);

/*!
 * Check if partner currently moves within a margin epsilon.
 *
 * The current partner is assumed to be either turtle 2 or 3.
 *
 * @param pIn Pointer to the general STP task input structure.
 * 			Contains data busses.
 * @return True if partner moves. */
static int check_partner_move(pPE_InputStruct_t pIn);

#endif /* TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_MODEL_H_ */
