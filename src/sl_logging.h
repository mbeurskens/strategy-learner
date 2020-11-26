/*
 * sl_logging.h
 *
 * Log reinforcement learning data and debug data.
 *
 *  Created on: May 25, 2020
 *      Author: Mickey Beurskens
 */

#ifndef TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_LOGGING_H_
#define TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_LOGGING_H_

#include "sl_data_types.h"
#include "GeneralRobocupFunctions/general_robocup_functions.h"

/*! Set the data in the model input data struct.
 *
 *  Set the data in the model input data structure for use in training machine
 *  learning models.
 *
 *  @param p_model_input Pointer to the model input data struct that is updated.
 *	@param p_team_bus Pointer to the teambus with information on the peer.
 *	@param p_trc_bus Pointer to TRC bus in order to read current ID.
 *	@param p_sensorfusion_bus Pointer to SF bus to read own position/velocity.
 */
void set_model_input_data(p_sl_model_input_t p_model_input,
		pteambus_t p_team_bus, pTRCbus_t p_trc_bus,
		psensorfusionbus_t p_sensor_fusion_bus);

/*! Set environment data for the machine learning model.
 *
 * 	The current reward is based on succes in passing the ball.
 *
 *	@param env_data Pointer to the environment data structure to store env data.
 *	@param p_team_bus Pointer to the teambus with information on the peer.
 *	@param p_trc_bus Pointer to TRC bus in order to read current ID.
 *	@param p_play_struct Pointer to the current play struct.
 *	@param state Current RL sub task. */
void set_environment_data(p_sl_environment_data_t p_env_data,
		pteambus_t p_team_bus, pTRCbus_t p_trc_bus,
		pStpPlayStruct_t p_play_struct, int state);

/*! Set the data in the model output data struct.
 *
 *  Set the data in the model output data structure for use in training
 *  machine learning models.
 *
 *  @param p_model_output Pointer to model output data.
 *	@param state Current RL sub task. */
void set_model_output_data(p_sl_model_output_t p_model_output,
		int state);

/*! Save the model input struct in binary form to a file.
 *
 *  Save the model input struct data to a file in binary form each time step.
 *
 *  @param p_training_data Pointer to data struct needed to train the algorithm.
 *  @param p_trc_bus Pointer to TRC bus.
 */
void save_model_data(p_sl_model_training_data_t p_training_data,
		pTRCbus_t p_trc_bus);

static void set_own_model_input_data(p_sl_model_input_t p_model_input,
		psensorfusionbus_t p_sensor_fusion_bus);

static void set_ball_model_input_data(p_sl_model_input_t p_model_input,
		psensorfusionbus_t p_sensor_fusion_bus);

static void set_peer_model_input_data(p_sl_model_input_t p_model_input,
		pteambus_t p_team_bus, pTRCbus_t p_trc_bus,
		psensorfusionbus_t p_sensor_fusion_bus);

static void set_ball_possession_model_input_data(
		p_sl_model_input_t p_model_input, pteambus_t p_team_bus,
		pTRCbus_t p_trc_bus);

static void set_reward(p_sl_environment_data_t env_data, pteambus_t p_team_bus,
		pTRCbus_t p_trc_bus, int passed_flag);

static float get_ball_collect_reward(pteambus_t p_team_bus,
		pTRCbus_t p_trc_bus);

static void set_n_balls_passed(p_sl_environment_data_t env_data,
		pteambus_t p_team_bus, pTRCbus_t p_trc_bus, int passed_flag);

static int get_pass(pteambus_t p_team_bus, pTRCbus_t p_trc_bus);

static int pass_given(pteambus_t p_team_bus, pTRCbus_t p_trc_bus,
		int max_pass_time);

static int pass_received(pteambus_t p_team_bus, pTRCbus_t p_trc_bus,
		int max_pass_time);

/*! Open file for model data logging.
 *
 *  @param p_trc_bus Pointer to TRC bus.
 */
static FILE* open_log_file(pTRCbus_t p_trc_bus);

/*! Get current time in microseconds.
 */
int64_t get_time_stamp();

/*! Destructor for strategy learner.
 *
 *  Handle cleaning up for strategy learner.
 */
static void close_strategy_learner();

#endif /* TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_LOGGING_H_ */
