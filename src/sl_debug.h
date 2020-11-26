/*
 * sl_debug.h
 *
 * Debugging functionality for strategy learner.
 *
 *  Created on: May 25, 2020
 *      Author: Mickey Beurskens
 */

#ifndef TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_DEBUG_H_
#define TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_DEBUG_H_

#include <stdio.h>
#include "sl_data_types.h"
#include "GeneralRobocupFunctions/general_robocup_functions.h"

/*! Print debug messages for model input, environment data and model output.
 *
 *  @param p_training_data Pointer to data struct needed to train the algorithm.
 */
void debug_print(p_sl_model_training_data_t p_training_data);

/*! Print debug messages for model input.
 *
 *	Prints all the parameters stored in the model input data structure.
 *
 *	@param p_model_input Pointer to the model input struct.
 */
static void debug_print_model_input(p_sl_model_input_t p_model_input);

/*! Print debug messages for model environment data.
 *
 *	Prints all the parameters stored in the environment data structure.
 *
 *	@param env_data Pointer to the environment data structure to store env data.
 */
static void debug_print_environment_data(p_sl_environment_data_t p_env_data);

/*! Print debug messages for model output.
 *
 *	Prints all the parameters stored in the model output data structure.
 *
 *	@param p_model_output Pointer to the model output struct.
 */
static void debug_print_model_output(p_sl_model_output_t p_model_output);

/* Debug variables */
int cur_print_line;

#endif /* TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_DEBUG_H_ */
