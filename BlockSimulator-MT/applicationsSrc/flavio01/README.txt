In this directory you will find two applications:

* "flavio01" is the blockSimulator main file (it is built and linked to the simulator core)

* VMEmulator is a simple test software that send some commands to the simulator and read its replies.

To build, run 'make' in the base directory of the simulator, not here !!

Communications between the simulator and the VM are handled with the following structure:

#define VM_MESSAGE_TYPE_START_SIMULATION        1
#define VM_MESSAGE_TYPE_END_SIMULATION          2
#define VM_MESSAGE_TYPE_CREATE_LINK             3
#define VM_MESSAGE_TYPE_COMPUTATION_LOCK        4
#define VM_MESSAGE_TYPE_COMPUTATION_UNLOCK      5
#define VM_MESSAGE_TYPE_SEND_MESSAGE            6
#define VM_MESSAGE_TYPE_RECEIVE_MESSAGE         7
#define VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE		8

typedef struct VMMessage_tt {
        uint64_t messageType;
        uint64_t param1;
        uint64_t param2;
        uint64_t param3;
} VMMessage_t;

The implementation changed considerably from what we had last year. I do not use a separate thread to manage the communications anymore, 
and the scheduler has been reworked.
Also, to prevents deadlocks, I now make the blocks alternate between two states.
For a block being in "defined" state means the simulator knows what to expect and can proceed with the event list.
The "undefined" state means the scheduler has to halt until this block tell what it wants (will it start some computation, send a message, etc.)

The scheduler can process events only if no block is in "undefined" state.


Message detail:
* VM_MESSAGE_TYPE_START_SIMULATION
param1 : the number of blocks required. For now, the blocks are created upon request from the VM, but if needed it's also possible 
to directly specify them in the xml file. In fact, as soon as the 3D visualization will be back, it will be usefull to use the xml file 
to handle the positionning of the blocks. All created blocks are set to an "undefined" state, meaning the simulator does not know yet 
what those blocks are doing.

* VM_MESSAGE_TYPE_END_SIMULATION
not really used anymore, as the simulator now stops as soon as there is no event left in the queue AND no block is in undefined state.
Sending an "END_SIMULATION" message whereas some blocks are still in undefined state will issue an error.


* VM_MESSAGE_TYPE_CREATE_LINK
param1 : source block ID
param2 : destination block ID
Create a point-to-point network link between the specified blocks. For test purpose, the default throughput is 1 Mbit/s, 
easilly modified if necessary.
Does not alter the "defined/undefined" state of a block

* VM_MESSAGE_TYPE_COMPUTATION_LOCK
param1 : concerned block
param2 : duration
Tell the simulator to lock this block for the specified amount of time (in microseconds)
Puts the block in "defined" state

* VM_MESSAGE_TYPE_COMPUTATION_UNLOCK
param1 : concerned block
param2 : time
Tell the VM this block finished its computation. Upon sending this message, the simulator puts the block in "undefined" state. 
The simulator wait for the VM to tell what to do next.

* VM_MESSAGE_TYPE_SEND_MESSAGE
param1 : source block ID
param2 : destination block ID
param3 : data size
Initiate the transmission simulation between two given blocks. The effective duration depends on the throuput of the interface used.
For now, only point-to-point interfaces are implemented. There is no data loss nor collision.
Puts the block in "defined" state.

* VM_MESSAGE_TYPE_RECEIVE_MESSAGE
param1 : receiver block ID
param2 : time of reception
param3 : data size
Tell the VM when a message has been received. After sending this, the simulator wait for the VM to tell what to do next.
Puts the block in "undefined" state

* VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE
param1 : block ID
Tell the simulator this block as nothing left to do, and that it only whait for someone else to send it a message.
Puts the block in "defined" state.
 