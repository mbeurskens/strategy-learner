/*
 * sl_general_functions.h
 *
 * General helper functions for strategy learner.
 *
 *  Created on: May 25, 2020
 *      Author: Mickey Beurskens
 */

#ifndef TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_GENERAL_FUNCTIONS_H_
#define TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_GENERAL_FUNCTIONS_H_

/*! Return euclidean distance between turtles with turtle given turtle ids.
 *
 * @param pIn Pointer to the general STP task input structure.
 * @param id_1 Turtle id for the first turtle.
 * @param id_2 Turtle id for the second turtle.
 * @return Euclidean distance between turtles. */
double get_euclidean_distance_by_tid(pPE_InputStruct_t pIn, int id_1,
		int id_2);

/*! Return first active peer turtle id. */
int get_other_active_tid(pPE_InputStruct_t pIn);

/*! Return first active peer turtle id based on bus input.
 *
 *  @param p_team_bus Pointer to the teambus with information on the peer.
 *	@param p_trc_bus Pointer to TRC bus in order to read current ID. */
int get_other_active_tid_from_bus(pteambus_t pTB, pTRCbus_t pTRCB);

#endif /* TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SL_GENERAL_FUNCTIONS_H_ */
