/* Kernel includes. */
#include "sam.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "uart.h"
#include "myprintf.h"

/* Priorities at which the tasks are created. */
#define myTASK_TASK_PRIORITY         	( tskIDLE_PRIORITY + 1 )

xQueueHandle Global_Queue_Handle = 0;

/* Parte I y II

void my_task(void *p) {
	while(1) {
		static uint32_t i = 0;
		myprintf("\nHello World task 1 %d", i++);
		vTaskDelay(1000);
	}
}

void sender_task(void *p) {
	while(1) {
		static uint32_t i = 0;
		myprintf("Send %d to receiver task\n", i);
		if (! xQueueSend(Global_Queue_Handle, &i, 1000)){
			myprintf("Failed to receive data from queue\n");
		}
		i++;
		vTaskDelay(2000);
	}
}


void receiver_task(void *p) {
	while(1) {
		static uint32_t j = 0;
		if(xQueueReceive(Global_Queue_Handle, &j, 1000)){
			myprintf("Received %d\n", j);
		}else{
			myprintf("Failed to receive data from queue\n");
		}
	}
}
*/

void thread1_task(void *p) {
	while(1) {
		static uint32_t i = 0, Ack;
		if( ( PORT->Group[ 0 ].IN.reg & PORT_IN_IN( PORT_PA14 ) ) == _U_( 0x00000000 ) ){
			i = 1;
			if (! xQueueSend(Global_Queue_Handle, &i, 1000)){
				myprintf("Failed to receive data from queue\n");
			}
		}
		else if ( ( PORT->Group[ 0 ].IN.reg & PORT_IN_IN( PORT_PA09 ) ) == _U_( 0x00000000 ) ){
			i = 2;
			if (! xQueueSend(Global_Queue_Handle, &i, 1000)){
				myprintf("Failed to receive data from queue\n");
			}
		}
		else if ( ( PORT->Group[ 0 ].IN.reg & PORT_IN_IN( PORT_PA08 ) ) == _U_( 0x00000000 ) ){
			i = 3;
			if (! xQueueSend(Global_Queue_Handle, &i, 1000)){
				myprintf("Failed to receive data from queue\n");
			}
		}	
		else if ( ( PORT->Group[ 0 ].IN.reg & PORT_IN_IN( PORT_PA15 ) ) == _U_( 0x00000000 ) ){
			i = 4;
			if (! xQueueSend(Global_Queue_Handle, &i, 1000)){
				myprintf("Failed to receive data from queue\n");
			}
		}	
		else {
			continue;
		}
		
		xQueueReceive(Global_Queue_Handle, &Ack, 1000);
		if(Ack == 0){
			myprintf("Error");
		}
	}
}

void thread2_task(void *p) {
	while(1) {
		static uint32_t j = 0, Ack;
		if(xQueueReceive(Global_Queue_Handle, &j, 1000)){
			//myprintf("Received %d\n", j);
			switch (j)
			{
			case 1:
				myprintf("Up\n");
				break;
			case 2:
				myprintf("Down\n");
				break;
			case 3:
				myprintf("Left\n");
				break;
			case 4:
				myprintf("Right\n");
				break;
			}
			Ack = 5;
			xQueueSend(Global_Queue_Handle, &Ack, 1000);
		}else{
			myprintf("Failed to receive data from queue\n");
			
			Ack = 0;
			xQueueSend(Global_Queue_Handle, &Ack, 1000);
		}
	}
}

int main()
{
	SystemInit();
	/* Switch to 8MHz clock (disable prescaler) */
	SYSCTRL->OSC8M.bit.PRESC = 0;
	initUART();
	
	PORT->Group[ 0 ].PINCFG[ PIN_PA14 ].reg = 0x2; //bit INEN must be set for input pins D2
	PORT->Group[ 0 ].PINCFG[ PIN_PA09 ].reg = 0x2; //bit INEN must be set for input pins D3
	PORT->Group[ 0 ].PINCFG[ PIN_PA08 ].reg = 0x2; //bit INEN must be set for input pins D4
	PORT->Group[ 0 ].PINCFG[ PIN_PA15 ].reg = 0x2; //bit INEN must be set for input pins D5

	PORT->Group[ 0 ].DIRCLR.reg = PORT_PA14; //pin 14 declared as data input D2
	PORT->Group[ 0 ].DIRCLR.reg = PORT_PA09; //pin 09 declared as data input D3
	PORT->Group[ 0 ].DIRCLR.reg = PORT_PA08; //pin 08 declared as data input D4
	PORT->Group[ 0 ].DIRCLR.reg = PORT_PA15; //pin 15 declared as data input D5
	
	
	Global_Queue_Handle = xQueueCreate(3, sizeof(int));
	/*
	xTaskCreate( sender_task,        	    // The function that implements the task. 
	"sender",              // The text name assigned to the task. 
	512,            	    // The size of the stack to allocate to the task. 
	NULL,           	    // The parameter passed to the task  
	myTASK_TASK_PRIORITY,   // The priority assigned to the task. 
	NULL );         	    // The task handle is not required, so NULL is passed.                  
	
	xTaskCreate( receiver_task,        	    // The function that implements the task. 
	"receiver",              // The text name assigned to the task. 
	512,            	    // The size of the stack to allocate to the task. 
	NULL,           	    // The parameter passed to the task  
	myTASK_TASK_PRIORITY,   // The priority assigned to the task. 
	NULL );         	    // The task handle is not required, so NULL is passed.                  
	*/
	
	xTaskCreate(thread1_task, "thread1", 512, NULL, myTASK_TASK_PRIORITY, NULL);
	xTaskCreate(thread2_task, "thread2", 512, NULL, myTASK_TASK_PRIORITY, NULL);
	
	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	for( ;; );
	return(0);
}