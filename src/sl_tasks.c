#include "sl_tasks.h"
#include "sl_includes.h"
#include "Strategy/src/STP/tasks/tasks_general.h"
#include "Strategy/src/STP/tasks/tasks_gameplay.h"
#include "Strategy/src/STP/tasks/tasks_positioning.h"
#include "GeneralRobocupFunctions/general_robocup_functions.h"


int sub_task_pass(pPE_InputStruct_t pIn, pPE_OutputStruct_t pOut,
		void* pTaskParam, pPE_sfun_global_data_t pGD)
{
	/* initialize */
	pSTP_TaskGivePassStruct_t pPS = (pSTP_TaskGivePassStruct_t) pTaskParam;
	pPS->pass_receiver_roleID =
			pIn->pTB->team_STP_PlayRoles[get_other_active_tid(pIn)-1];
	pPS->pass_flag = Pass_at_position;
	int pass_to_tID = getTurtleIDFromPlayRole(pPS->pass_receiver_roleID,
			pIn->pPlay);

	/* give actual pass*/
	if(pass_to_tID > ROLE_NOT_SET){
//		printf("Pass giver tid: %d! my pass state is: %d\n",
//				pGD->PassStruct.Pass_giver_turtle_ID,
//				pGD->PassStruct.Pass_giver_stage);
//				printf("Pass receiver tid: %d! my pass state is: %d\n",
//				pGD->PassStruct.Pass_receiver_turtle_ID,
//				pGD->PassStruct.Pass_receiver_stage);
		givePass(&pGD->PassStruct, pass_to_tID, pPS->pass_flag,
				&(pIn->pTB->current_xyo[pass_to_tID - 1]));
	}
	else{
		CreateActionIdle(pOut->pAHParams);
		DoAction(eActionSmoothStop, pIn->pAHState, pOut->pAHParams);
	}

	/* set slow aiming */
	pOut->pSTB->use_refbox_aim_va_limits = 1;

	/* set function output */
	return 0;
}


int sub_task_receive(pPE_InputStruct_t pIn, pPE_OutputStruct_t pOut,
		void* pTaskParam, pPE_sfun_global_data_t pGD)
{
	pSTP_TaskMoveOrDribbleToTargetStruct_t pPS =
			(pSTP_TaskMoveOrDribbleToTargetStruct_t) pTaskParam;
	if (pGD->PassStruct.Pass_giver_turtle_ID == pIn->pTRCB->turtleID){
		not_available_for_receiving_pass(&(pGD->PassStruct));
		reset_Pass_handler(&(pGD->PassStruct));
	}else {
		available_for_receiving_pass(&(pGD->PassStruct));
	}

	int arrivedAtTarget 				= 0;
	Pose_t	 TurtleTarget 				= pPS->Target;
	Pose_t   ClipTargetOutsideOpponent	= pPS->ClipTargetOutsideOpponent;

	/* prerequisites */
	Pos_t  		BallPos_for_positioning	= {.arr = {0.0, 0.0, 0.0}};;
	Obstacle_t	WMobstacles_for_positioning[MAX_OPPONENTS];
	InitObstacle(WMobstacles_for_positioning,MAX_OPPONENTS);

	/* Set hysterized ball and opponent positions during refbox tasks */
	if (REFBOX_IN_PROGRESS(pIn->pTB->consolidated_RefboxStatus)){
		/* Ball position used */
		BallPos_for_positioning = pGD->RefboxStruct.HysterizedBallPosition;
		memcpy(WMobstacles_for_positioning,
				pGD->RefboxStruct.HysterizedWMobstacles,
				MAX_OPPONENTS * sizeof(Obstacle_t));  /* Opponent pos used */
	}
	else{
		BallPos_for_positioning = pIn->pSFB->ball_xyzdxdydz.pos;
		memcpy(WMobstacles_for_positioning, pIn->pTB->WMopponents,
				MAX_OPPONENTS * sizeof(Obstacle_t));
	}

	/* mirror target when Blue is home goal
	 * TODO KM says, how to fix this? */
	if(pPS->mirrorTargetBlueHomeGoal && 0){
		TurtleTarget.x *= -1;
		TurtleTarget.y *= -1;
		if(TurtleTarget.o != LOOK_AT_BALL_FLAG){TurtleTarget.o *= -1;}
	}

	/* avoid collision with obstacles when positioning:
	 *  clip circle outside opponent towards ClipTargetOutsideOpponent.xy,
	 *   see clip_to_outside_circle() */
	int i;
	if(ClipTargetOutsideOpponent.o > 0.5){
		for(i = 0; i < MAX_OPPONENTS; i++){
			if(WMobstacles_for_positioning[i].valid){
				/* Make sure that in case of clipping,
				 *  we do not get too close */
				clip_to_outside_circle(TurtleTarget.arr,
						WMobstacles_for_positioning[i].pose.arr,
						ClipTargetOutsideOpponent.arr,
						ROBOTRADIUS + OPPONENTRADIUS);
			}
		}
	}

	/* Do the positioning */
	ActionReturnValue_t result = eActionNotActive;
	if(pPS->max_velocity < 0.0  || pPS->max_acceleration < 0.0) {
		/* No speed limit */
		CreateActionGoToTarget(pOut->pAHParams, &TurtleTarget);
		result = DoAction(eActionGoToTarget, pIn->pAHState, pOut->pAHParams);
	} else {
		/* Limit the speed */
		CreateActionGoToTargetVA(pOut->pAHParams, &TurtleTarget,
				pPS->max_velocity, pPS->max_acceleration);
		result = DoAction(eActionGoToTargetVA, pIn->pAHState, pOut->pAHParams);
	}

	arrivedAtTarget = ( (result == eActionFinished) ||
			( (pPS->min_target_approach_distance > 0.0) &&
					getdistance_vec(&TurtleTarget, &pIn->pSFB->cur_xyo) <
					pPS->min_target_approach_distance) );


	/********* Set function output **************/
	if (!pPS->waitForMe && (!pPS->waitForStart ||
			REFBOX_IN_PROGRESS_AFTER_START(pGD->RefboxStruct.myRefboxStatus))) {
		return TASK_EXECUTION_FINISHED;
	}
	if(pPS->TaskNeverFinished){
		return TASK_EXECUTION_BUSY;
	}
	if (arrivedAtTarget && (!pPS->waitForStart ||
			REFBOX_IN_PROGRESS_AFTER_START(pGD->RefboxStruct.myRefboxStatus))) {
		return TASK_EXECUTION_FINISHED;
	} else {
		return TASK_EXECUTION_BUSY;
	}
}


int sub_task_move(pPE_InputStruct_t pIn, pPE_OutputStruct_t pOut,
		void* pTaskParam, pPE_sfun_global_data_t pGD)
{
	return stp_task_move_or_dribble_to_target(pIn, pOut, pTaskParam, pGD);
}


int sub_task_intercept(pPE_InputStruct_t pIn, pPE_OutputStruct_t pOut,
		void* pTaskParam, pPE_sfun_global_data_t pGD)
{
	return stp_task_intercept_ball(pIn, pOut, pTaskParam, pGD);
}
