/*! \file strategy_learner_data_types.h
 * Data types used in the strategy learner.
 *
 * The strategy learner needs data to train. The data types for the model data
 * and other shared data types needed for training and implementation are
 * defined here.
 *
 * @author 		Mickey Beurskens <mickey_beurskens@hotmail.com>
 * @created 	2020-April-08
 */

#ifndef STRATEGY_LEARNER__DATA_TYPES_H_
#define STRATEGY_LEARNER__DATA_TYPES_H_

#include <stdio.h>
#include <stdint.h>

/*!
 * Learning model input data structure.
 *
 * Data structure for the machine learning model. Contains the input parameters
 * for the model.
 */
typedef struct {
    // Own turtle planar position and velocities
    double own_xyo_x;
    double own_xyo_y;
    double own_xyo_o;
    double own_xyo_dot_x;
    double own_xyo_dot_y;
    double own_xyo_dot_o;

    // Peer turtle planar position and velocities
    double peer_xyo_x;
    double peer_xyo_y;
    double peer_xyo_o;
    double peer_xyo_dot_x;
    double peer_xyo_dot_y;
    double peer_xyo_dot_o;

    // Ball spatial position and velocity excluding orientation
    double ball_xyz_x;
    double ball_xyz_y;
    double ball_xyz_z;
    double ball_xyz_dot_x;
    double ball_xyz_dot_y;
    double ball_xyz_dot_z;

    // Ball possession which, either true or false
    double own_ball_possession;
    double peer_ball_possession;
} sl_model_input_t, *p_sl_model_input_t;

/*!
 * Learning model state data structure.
 *
 * Data structure for the machine learning model. Contains the rewards and
 * other environment variables needed for training.
 */
typedef struct {
	float reward;	/* Reward for the machine learning model at time step */
	int n_balls_passed; /* Times the ball has been passed */
} sl_environment_data_t, *p_sl_environment_data_t;

/*!
 * Learning model output state data structure.
 *
 * Data structure for the machine learning model. Contains the one hot encoded
 * states of the agent which is the output of the machine learning model.
 */
typedef struct {
	int role; 	/* Predicted role ID */\
	int action_space_size;  /* Number of roles to choose from */
	float vpg_value;  	/* Value as predicted by vpg algorithm when used */
}sl_model_output_t, *p_sl_model_output_t;

/*! Data structure to log ML model data used for training the strategy learner.
 *
 *  The data is packed as tightly as possible to increase storage efficiency.
 */
typedef struct __attribute__((packed)) {
	uint64_t time_stamp;				    // Time stamp for data
	sl_model_input_t	 	model_input;  	// ML model input data
	sl_environment_data_t 	env_data; 		// ML model environment data
	sl_model_output_t 		model_output; 	// ML model output data
}sl_model_training_data_t, *p_sl_model_training_data_t;

/*!
 * Data structure that stores model and experiment settings
 */
typedef struct {
	int manual_mode; // True if strategy learner executes manual mode
} sl_settings_t, *p_sl_settings_t;

#endif /* STRATEGY_LEARNER__DATA_TYPES_H_ */
