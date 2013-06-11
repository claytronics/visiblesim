/*
 * blinky01BlockCode.h
 *
 *  Created on: 26 mars 2013
 *      Author: dom
 */

#ifndef VMEMULATOR_H_
#define VMEMULATOR_H_

typedef struct VMMessage_tt {
        uint64_t messageType;
        uint64_t param1;
        uint64_t param2;
        uint64_t param3;
} VMMessage_t;

void vm_thread_function(void *data);

#endif /* BLINKY01BLOCKCODE_H_ */
