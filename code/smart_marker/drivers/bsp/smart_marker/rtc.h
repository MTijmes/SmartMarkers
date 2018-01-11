#ifndef RTC_H
#define RTC_H

#include <stdbool.h>
#include <timer.h>
/* Includes ------------------------------------------------------------------*/
/* Public types --------------------------------------------------------------*/
/* Public constants ----------------------------------------------------------*/
/* Public macro --------------------------------------------------------------*/
/* Public functions --------------------------------------------------------- */
void rtc_init(void);
void BlockLowPowerDuringTask(bool status);
void RtcComputeWakeUpTime(void);
void RtcRecoverMcuStatus(void);
void RTC_AlarmIRQHandler();
TimerTime_t RtcGetTimerValue(void);

#endif /* RTC_H */
