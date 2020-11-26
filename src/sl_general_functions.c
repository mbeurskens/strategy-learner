#include "sl_includes.h"
#include "sl_general_functions.h"


double get_euclidean_distance_by_tid(pPE_InputStruct_t pIn, int id_1,
		int id_2)
{
	int own_t_index = id_1 - 1;
	int peer_t_index = id_2 - 1;
	if (own_t_index >= 0 && peer_t_index >= 0) {
		Pose_t own_pos = pIn->pTB->current_xyo[own_t_index];
		Pose_t peer_pos = pIn->pTB->current_xyo[peer_t_index];
		return sqrt(pow(own_pos.x-peer_pos.x, 2.0) +
					pow(own_pos.y-peer_pos.y, 2.0));
	}
	return 0;
}


int get_other_active_tid(pPE_InputStruct_t pIn){
	int array_size = sizeof(pIn->pTB->isActivePeer)/sizeof(int);
	int i = 0;
	for(i=0; i < array_size; i++){
		/* If we find the first active peer, return turtle ID */
		if(pIn->pTRCB->turtleID != i+1 && pIn->pTB->isActivePeer[i] == 1){
			return i+1;
		}
	}
	/* If there are no active peers, return own ID */
	return pIn->pTRCB->turtleID;
}


int get_other_active_tid_from_bus(pteambus_t pTB, pTRCbus_t pTRCB){
	int array_size = sizeof(pTB->isActivePeer)/sizeof(int);
	int i = 0;
	for(i=0; i < array_size; i++){
		/* If we find the first active peer, return turtle ID */
		if(pTRCB->turtleID != i+1 && pTB->isActivePeer[i] == 1){
			return i+1;
		}
	}
	/* If there are no active peers, return own ID */
	return pTRCB->turtleID;
}
