/*
 * watch.h
 *
 *  Created on: Sep 24, 2024
 *      Author: MelvinJr.Gaviola
 */

#ifndef INC_WATCH_H_
#define INC_WATCH_H_

void displayTimeTask		(void *param);
void alarmTask				(void *param);
void hourlyAlarmTask		(void *param);
void changeFormatTask		(void *param);
void snoozeTask				(void *param);
void adjustTime				(void *param);
void stopwatchTask			(void *param);
void blinkBacklight			(void *param);

#endif /* INC_WATCH_H_ */
