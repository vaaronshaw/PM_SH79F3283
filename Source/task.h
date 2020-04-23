/*
 * task.h
 *
 *  Created on: 2018��9��19��
 *      Author: hp
 */

#ifndef _TASK_H_
#define _TASK_H_

#include "defines.h"


#define TASK_BASE_TICK		2	//!< 2ms
#define TASK_MS2TICK(x)		((x) / TASK_BASE_TICK)
	




typedef struct _TaskStruct{
	uchar ucIsRun;        	//��ʾ�����Ƿ�������
	uint uiSliceValue;   	//!< time slice for task running, 0 means  task run for every main loop
	uint uiSliceResetValue;  	//!< reset value for time slice
	void (*pvTaskPointer)(void);   //����ĺ���ָ��
}TaskStruct;






extern void TASK_vIRQHandler(void);
extern void TASK_vScheduleTasks(void);





























#endif /* _TASK_H_ */
