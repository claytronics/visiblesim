/*
 * uniqueEventsId.h
 *
 *  Created on: 26 mars 2013
 *      Author: dom
 */

#ifndef UNIQUEEVENTSID_H_
#define UNIQUEEVENTSID_H_

#define EVENT_GENERIC						1
#define EVENT_CODE_START					2
#define EVENT_NI_START_TRANSMITTING			3
#define EVENT_NI_STOP_TRANSMITTING			4
#define EVENT_NI_RECEIVE					5
#define EVENT_NI_ENQUEUE_OUTGOING_MESSAGE 	6
#define EVENT_PROCESS_LOCAL_EVENT			7
#define EVENT_BLOCK_TIMER					8
#define EVENT_END_SIMULATION				9

#define EVENT_VM_START_COMPUTATION			1000
#define EVENT_VM_END_COMPUTATION			1001
#define EVENT_VM_START_TRANSMISSION			1002

#define EVENT_SET_ID						1050
#define EVENT_STOP							1051
#define EVENT_ADD_NEIGHBOR					1052
#define EVENT_REMOVE_NEIGHBOR				1053
#define EVENT_TAP							1054
#define EVENT_RECEIVE_MESSAGE 				EVENT_NI_RECEIVE
#define EVENT_ACCEL							1055
#define EVENT_SHAKE							1056

#endif /* UNIQUEEVENTSID_H_ */
