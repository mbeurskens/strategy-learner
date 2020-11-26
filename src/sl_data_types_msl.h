/*
 * sl_data_types_msl.h
 *
 * Custom data types depending on the strategy part of the soccer robot
 * software.
 *
 *  Created on: Jul 6, 2020
 *      Author: Mickey Beurskens
 */

#ifndef TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SRC_SL_DATA_TYPES_MSL_H_
#define TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SRC_SL_DATA_TYPES_MSL_H_

#include "Strategy/src/STP/tasks/tasks_general.h"
#include "Strategy/src/STP/tasks/tasks_gameplay.h"
#include "Strategy/src/STP/tasks/tasks_positioning.h"

/* task_learned */
typedef struct {
    STP_TaskGivePassStruct_t				stp_pass;
    STP_TaskMoveOrDribbleToTargetStruct_t   stp_receive;
    STP_TaskMoveOrDribbleToTargetStruct_t 	stp_move;
    STP_TaskInterceptBallStruct_t 			stp_intercept;
}STP_TaskLearned, *pSTP_TaskLearned;


#endif /* TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SRC_SL_DATA_TYPES_MSL_H_ */
