/* Includes ------------------------------------------------------------------*/
#include <board.h>
#include <rtc.h>

#include <stm32l0xx_ll_rtc.h>
#include <stm32l0xx_ll_gpio.h>
#include <stm32l0xx_ll_bus.h>
#include <stm32l0xx_ll_pwr.h>
#include <stm32l0xx_ll_rcc.h>
#include <stm32l0xx_ll_exti.h>
#include <math.h>

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
    uint8_t Hours;          /*!< Specifies the RTC Time Hour.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 12 if the RTC_HourFormat_12 is selected.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 23 if the RTC_HourFormat_24 is selected */

    uint8_t Minutes;        /*!< Specifies the RTC Time Minutes.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 59 */

    uint8_t Seconds;        /*!< Specifies the RTC Time Seconds.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 59 */

    uint32_t SubSeconds;   /*!< Specifies the RTC_SSR RTC Sub Second register content.
                                 This parameter corresponds to a time unit range between [0-1] Second
                                 with [1 Sec / SecondFraction +1] granularity */
} RTC_TimeType;

typedef struct RtcCalendar_s
{
    LL_RTC_DateTypeDef CalendarDate; //! Reference time in calendar format
    RTC_TimeType CalendarTime; //! Reference date in calendar format
} RtcCalendar_t;

RtcCalendar_t RtcCalendarContext;
/* Private define ------------------------------------------------------------*/
#define RTC_ALARM_TICK_PER_MS                       0x7FF

/* sub-second number of bits */
#define N_PREDIV_S                11

#define RTC_ASYNCH_PREDIV           (1 << (15-N_PREDIV_S))-1

#define RTC_SYNCH_PREDIV           ((1 << N_PREDIV_S)-1)

/* RTC Time base in us */
#define USEC_NUMBER               1000000
#define MSEC_NUMBER               (USEC_NUMBER / 1000)
#define RTC_ALARM_TIME_BASE       (USEC_NUMBER >> N_PREDIV_S)

#define COMMON_FACTOR             3
#define CONV_NUMER                (MSEC_NUMBER >> COMMON_FACTOR)
#define CONV_DENOM                (1 << (N_PREDIV_S-COMMON_FACTOR))

/*!
 * Times
 */
static const uint8_t SecondsInMinute = 60;
static const uint16_t SecondsInHour = 3600;
static const uint32_t SecondsInDay = 86400;
static const uint8_t HoursInDay = 24;
static const uint32_t SecondsInLeapYear = 31622400;
static const uint32_t SecondsInYear = 31536000;
static const uint8_t DaysInMonth[] =
{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const uint8_t DaysInMonthLeapYear[] =
{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static bool RtcTimerEventAllowsLowPower = false;
static bool LowPowerDisableDuringTask = false;
//static bool RtcInitialized = false;
static bool WakeUpTimeInitialized = false;
volatile uint32_t McuWakeUpTime = 0;

volatile bool NonScheduledWakeUp = false;

static void RtcStartWakeUpAlarm(uint32_t timeoutValue);
RtcCalendar_t RtcConvertTimerTimeToCalendarTick(TimerTime_t timeCounter);
static TimerTime_t RtcConvertCalendarTickToTimerTime(RtcCalendar_t *calendar);
static TimerTime_t RtcConvertMsToTick(TimerTime_t timeoutValue);
static TimerTime_t RtcConvertTickToMs(TimerTime_t timeoutValue);
static RtcCalendar_t RtcComputeTimerTimeToAlarmTick(TimerTime_t   timeCounter,
                                                    RtcCalendar_t now);
static RtcCalendar_t RtcGetCalendar(void);

uint32_t
WaitForSynchro_RTC(void)
{
    /* Clear RSF flag */
    LL_RTC_ClearFlag_RS(RTC);

    /* Wait the registers to be synchronised */
    while(LL_RTC_IsActiveFlag_RS(RTC) != 1) {}
    return 0;
}

uint32_t
Exit_RTC_InitMode(void)
{
    LL_RTC_DisableInitMode(RTC);

    /* Wait for synchro */
    /* Note: Needed only if Shadow registers is enabled           */
    /*       LL_RTC_IsShadowRegBypassEnabled function can be used */
    return (0);
}

uint32_t
Enter_RTC_InitMode(void)
{
    /* Set Initialization mode */
    LL_RTC_EnableInitMode(RTC);
    while (LL_RTC_IsActiveFlag_INIT(RTC) != 1) {}
    return 0;
}

void
rtc_init(void)
{
    LL_RTC_InitTypeDef RTC_InitStruct;
    LL_RTC_TimeTypeDef RTC_TimeStruct;
    LL_RTC_DateTypeDef RTC_DateStruct;

    /* Peripheral clock enable */
    LL_RCC_EnableRTC();

    /**Initialize RTC and set the Time and Date
     */
    RTC_InitStruct.HourFormat = LL_RTC_HOURFORMAT_24HOUR;
    RTC_InitStruct.AsynchPrescaler = RTC_ASYNCH_PREDIV;
    RTC_InitStruct.SynchPrescaler = RTC_SYNCH_PREDIV;
    LL_RTC_Init(RTC, &RTC_InitStruct);

    if(LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR0) != 0x32F2) {
        RTC_TimeStruct.TimeFormat = LL_RTC_TIME_FORMAT_AM_OR_24;
        RTC_TimeStruct.Hours = 0;
        RTC_TimeStruct.Minutes = 0;
        RTC_TimeStruct.Seconds = 0;
        LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BCD, &RTC_TimeStruct);

        RTC_DateStruct.Day = 1;
        RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_SATURDAY;
        RTC_DateStruct.Month = LL_RTC_MONTH_JANUARY;
        RTC_DateStruct.Year = 0;
        LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BCD, &RTC_DateStruct);

        LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR0, 0x32F2);
    }

    LL_RTC_DisableWriteProtection(RTC);

    /* Set Initialization mode */
    if (Enter_RTC_InitMode() != 0) {
        /* Initialization Error */
    }

    LL_RTC_SetAlarmOutEvent(RTC, LL_RTC_ALARMOUT_DISABLE);
    LL_RTC_SetOutputPolarity(RTC, LL_RTC_OUTPUTPOLARITY_PIN_HIGH);
    LL_RTC_SetAlarmOutputType(RTC, LL_RTC_ALARM_OUTPUTTYPE_OPENDRAIN);

    LL_RTC_TIME_DisableDayLightStore(RTC);
    LL_RTC_TIME_SetFormat(RTC, LL_RTC_FORMAT_BIN);

    LL_RTC_EnableShadowRegBypass(RTC);


    Exit_RTC_InitMode();

    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);
    LL_RTC_EnableWriteProtection(RTC);

    NVIC_SetPriority(RTC_IRQn, 1);
    NVIC_EnableIRQ(RTC_IRQn);
}

void
RtcSetTimeout(uint32_t timeout)
{
    RtcStartWakeUpAlarm(RtcConvertMsToTick(timeout));
}

TimerTime_t
RtcGetAdjustedTimeoutValue(uint32_t timeout)
{
    if( timeout > McuWakeUpTime ) { // we have waken up from a GPIO and we have lost "McuWakeUpTime" that we need to compensate on next event
        if( NonScheduledWakeUp == true ) {
            NonScheduledWakeUp = false;
            timeout -= McuWakeUpTime;
        }
    }

    if( timeout > McuWakeUpTime ) { // we don't go in Low Power mode for delay below 50ms (needed for LEDs)
        if( timeout < 50 ) { // 50 ms
            RtcTimerEventAllowsLowPower = false;
        }else {
            RtcTimerEventAllowsLowPower = true;
            timeout -= McuWakeUpTime;
        }
    }
    return timeout;
}

TimerTime_t
RtcGetTimerValue(void)
{
    TimerTime_t retVal = 0;
    retVal = RtcConvertCalendarTickToTimerTime(0);
    RtcConvertTickToMs(retVal);

    return(RtcConvertTickToMs(retVal));
}

TimerTime_t
RtcGetElapsedAlarmTime(void)
{
    TimerTime_t retVal = 0;
    TimerTime_t currentTime = 0;
    TimerTime_t contextTime = 0;

    currentTime = RtcConvertCalendarTickToTimerTime(0);
    contextTime = RtcConvertCalendarTickToTimerTime(&RtcCalendarContext);

    if( currentTime < contextTime ) {
        retVal = (currentTime+(0xFFFFFFFF-contextTime));
    }else {
        retVal = (currentTime-contextTime);
    }
    return(RtcConvertTickToMs(retVal));
}

TimerTime_t
RtcComputeFutureEventTime(TimerTime_t futureEventInTime)
{
    return(RtcGetTimerValue()+futureEventInTime);
}

TimerTime_t
RtcComputeElapsedTime(TimerTime_t eventInTime)
{
    TimerTime_t elapsedTime = 0;

    // Needed at boot, cannot compute with 0 or elapsed time will be equal to current time
    if( eventInTime == 0 ) {
        return 0;
    }

    elapsedTime = RtcConvertCalendarTickToTimerTime(0);

    elapsedTime = RtcConvertTickToMs(elapsedTime);

    if( elapsedTime < eventInTime ) { // roll over of the counter
        return(elapsedTime+(0xFFFFFFFF-eventInTime));
    }else {
        return(elapsedTime-eventInTime);
    }
}

void
BlockLowPowerDuringTask(bool status)
{
    if( status == true ) {
        RtcRecoverMcuStatus();
    }
    LowPowerDisableDuringTask = status;
}

void
RtcEnterLowPowerStopMode(void)
{
    if((LowPowerDisableDuringTask == false) &&
       (RtcTimerEventAllowsLowPower == true)) {
        //BoardDeInitMcu(); //TODO: Implement in board.c?

        // Disable the Power Voltage Detector
        LL_PWR_DisablePVD();

        SET_BIT(PWR->CR, PWR_CR_CWUF);

        // Enable Ultra low power mode
        LL_PWR_EnableUltraLowPower();

        // Enable the fast wake up from Ultra low power mode
        LL_PWR_EnableFastWakeUp();

        // Enter Stop Mode
        LL_PWR_SetPowerMode(LL_PWR_MODE_STOP);
    }
}

void
RtcRecoverMcuStatus(void)
{
    // PWR_FLAG_WU indicates the Alarm has waken-up the MCU
    if( LL_RTC_IsActiveFlag_WUT(RTC)) {
        LL_RTC_ClearFlag_WUT(RTC);
    }else {
        NonScheduledWakeUp = true;
    }
    // check the clk source and set to full speed if we are coming from sleep mode
    if((LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_HSI) ||
       (LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_MSI)) {
        //BoardInitMcu( ); //TODO: Fix in board.c?
    }
}

void
RtcComputeWakeUpTime(void)
{
    uint32_t start = 0;
    uint32_t stop = 0;
    uint32_t subsec = LL_RTC_ALMA_GetSubSecond(RTC);
    RtcCalendar_t now;

    if( WakeUpTimeInitialized == false ) {
        now = RtcGetCalendar();

        start = RTC_SYNCH_PREDIV-subsec;
        stop = RTC_SYNCH_PREDIV-now.CalendarTime.SubSeconds;

        McuWakeUpTime = RtcConvertTickToMs(stop-start);

        WakeUpTimeInitialized = true;
    }
}

static void
RtcStartWakeUpAlarm(uint32_t timeoutValue)
{
    RtcCalendar_t now;
    RtcCalendar_t alarmTimer;
    LL_RTC_AlarmTypeDef al_struct_a;

    LL_RTC_DisableWriteProtection(RTC);
    LL_RTC_ALMA_Disable(RTC);
    while (!LL_RTC_IsActiveFlag_ALRAW(RTC));

    LL_RTC_DisableIT_ALRA(RTC);

    if( timeoutValue <= 3 ) {
        timeoutValue = 3;
    }

    // Load the RTC calendar
    now = RtcGetCalendar();

    // Save the calendar into RtcCalendarContext to be able to calculate the elapsed time
    RtcCalendarContext = now;

    // timeoutValue is in ms
    alarmTimer = RtcComputeTimerTimeToAlarmTick(timeoutValue, now);
    LL_RTC_ALMA_StructInit(&al_struct_a);

    al_struct_a.AlarmDateWeekDaySel = LL_RTC_ALMA_DATEWEEKDAYSEL_DATE;
    al_struct_a.AlarmMask = LL_RTC_ALMA_MASK_NONE;
    al_struct_a.AlarmDateWeekDay = alarmTimer.CalendarDate.Day;

    al_struct_a.AlarmTime.Seconds = alarmTimer.CalendarTime.Seconds;
    al_struct_a.AlarmTime.Minutes = alarmTimer.CalendarTime.Minutes;
    al_struct_a.AlarmTime.Hours = alarmTimer.CalendarTime.Hours;

    LL_RTC_ALMA_Init(RTC,
                     LL_RTC_FORMAT_BIN,
                     &al_struct_a);

    LL_RTC_ALMA_SetSubSecond(RTC, alarmTimer.CalendarTime.SubSeconds);

    LL_RTC_DisableWriteProtection(RTC);

    LL_RTC_ClearFlag_ALRA(RTC);
    while (!LL_RTC_IsActiveFlag_ALRAW(RTC));

    LL_RTC_ALMA_Enable(RTC);
    LL_RTC_EnableIT_ALRA(RTC);

    LL_RTC_EnableWriteProtection(RTC);
}

static RtcCalendar_t
RtcComputeTimerTimeToAlarmTick(TimerTime_t timeCounter, RtcCalendar_t now)
{
    RtcCalendar_t calendar = now;

    TimerTime_t timeoutValue = 0;

    uint16_t milliseconds = 0;
    uint16_t seconds = now.CalendarTime.Seconds;
    uint16_t minutes = now.CalendarTime.Minutes;
    uint16_t hours = now.CalendarTime.Hours;
    uint16_t days = now.CalendarDate.Day;

    timeoutValue = timeCounter;

    milliseconds = RTC_SYNCH_PREDIV-now.CalendarTime.SubSeconds;
    milliseconds += (timeoutValue & RTC_SYNCH_PREDIV);

    /* convert timeout  to seconds */
    timeoutValue >>= N_PREDIV_S;  /* convert timeout  in seconds */

    // Convert milliseconds to RTC format and add to now
    while( timeoutValue >= SecondsInDay ) {
        timeoutValue -= SecondsInDay;
        days++;
    }

    // Calculate hours
    while( timeoutValue >= SecondsInHour ) {
        timeoutValue -= SecondsInHour;
        hours++;
    }

    // Calculate minutes
    while( timeoutValue >= SecondsInMinute ) {
        timeoutValue -= SecondsInMinute;
        minutes++;
    }

    // Calculate seconds
    seconds += timeoutValue;

    // Correct for modulo
    while( milliseconds >= (RTC_SYNCH_PREDIV+1)) {
        milliseconds -= (RTC_SYNCH_PREDIV+1);
        seconds++;
    }

    while( seconds >= SecondsInMinute ) {
        seconds -= SecondsInMinute;
        minutes++;
    }

    while( minutes >= 60 ) {
        minutes -= 60;
        hours++;
    }

    while( hours >= HoursInDay ) {
        hours -= HoursInDay;
        days++;
    }

    if((now.CalendarDate.Year == 0) || (now.CalendarDate.Year % 4) == 0 ) {
        if( days > DaysInMonthLeapYear[now.CalendarDate.Month-1] ) {
            days = days % DaysInMonthLeapYear[now.CalendarDate.Month-1];
            calendar.CalendarDate.Month++;
        }
    }else {
        if( days > DaysInMonth[now.CalendarDate.Month-1] ) {
            days = days % DaysInMonth[now.CalendarDate.Month-1];
            calendar.CalendarDate.Month++;
        }
    }

    calendar.CalendarTime.SubSeconds = RTC_SYNCH_PREDIV-milliseconds;
    calendar.CalendarTime.Seconds = seconds;
    calendar.CalendarTime.Minutes = minutes;
    calendar.CalendarTime.Hours = hours;
    calendar.CalendarDate.Day = days;

    return calendar;
}

//
// REMARK: Removed function static attribute in order to suppress
//         "#177-D function was declared but never referenced" warning.
// static RtcCalendar_t RtcConvertTimerTimeToCalendarTick( TimerTime_t timeCounter )
//
RtcCalendar_t
RtcConvertTimerTimeToCalendarTick(TimerTime_t timeCounter)
{
    RtcCalendar_t calendar = { { 0 }, { 0 } };

    TimerTime_t timeoutValue = 0;

    uint16_t milliseconds = 0;
    uint16_t seconds = 0;
    uint16_t minutes = 0;
    uint16_t hours = 0;
    uint16_t days = 0;
    uint8_t months = 1; // Start at 1, month 0 does not exist
    uint16_t years = 0;

    timeoutValue = timeCounter;

    milliseconds += (timeoutValue & RTC_SYNCH_PREDIV);

    /* convert timeout  to seconds */
    timeoutValue >>= N_PREDIV_S; // convert timeout  in seconds

    // Convert milliseconds to RTC format and add to now
    while( timeoutValue >= SecondsInDay ) {
        timeoutValue -= SecondsInDay;
        days++;
    }

    // Calculate hours
    while( timeoutValue >= SecondsInHour ) {
        timeoutValue -= SecondsInHour;
        hours++;
    }

    // Calculate minutes
    while( timeoutValue >= SecondsInMinute ) {
        timeoutValue -= SecondsInMinute;
        minutes++;
    }

    // Calculate seconds
    seconds += timeoutValue;

    // Correct for modulo
    while( milliseconds >= (RTC_SYNCH_PREDIV+1)) {
        milliseconds -= (RTC_SYNCH_PREDIV+1);
        seconds++;
    }

    while( seconds >= SecondsInMinute ) {
        seconds -= SecondsInMinute;
        minutes++;
    }

    while( minutes >= 60 ) {
        minutes -= 60;
        hours++;
    }

    while( hours >= HoursInDay ) {
        hours -= HoursInDay;
        days++;
    }

    while( days > DaysInMonthLeapYear[months-1] ) {
        days -= DaysInMonthLeapYear[months-1];
        months++;
    }

    calendar.CalendarTime.SubSeconds = RTC_SYNCH_PREDIV-milliseconds;
    calendar.CalendarTime.Seconds = seconds;
    calendar.CalendarTime.Minutes = minutes;
    calendar.CalendarTime.Hours = hours;
    calendar.CalendarDate.Day = days;
    calendar.CalendarDate.Month = months;
    calendar.CalendarDate.Year = years; // on 32-bit, years will never go up

    return calendar;
}

static TimerTime_t
RtcConvertCalendarTickToTimerTime(RtcCalendar_t *calendar)
{
    TimerTime_t timeCounter = 0;
    RtcCalendar_t now;
    uint32_t timeCounterTemp = 0;

    // Passing a 0 pointer will compute from "now" else,
    // compute from the given calendar value
    if( calendar == 0 ) {
        now = RtcGetCalendar();
    }else {
        now = *calendar;
    }

    // Years (calculation valid up to year 2099)
    for( int16_t i = 0; i < now.CalendarDate.Year; i++ )
    {
        if((i == 0) || (i % 4) == 0 ) {
            timeCounterTemp += (uint32_t)SecondsInLeapYear;
        }else {
            timeCounterTemp += (uint32_t)SecondsInYear;
        }
    }

    // Months (calculation valid up to year 2099)*/
    if((now.CalendarDate.Year == 0) || (now.CalendarDate.Year % 4) == 0 ) {
        for( uint8_t i = 0; i < (now.CalendarDate.Month-1); i++ )
        {
            timeCounterTemp +=
                (uint32_t)(DaysInMonthLeapYear[i] * SecondsInDay);
        }
    }else {
        for( uint8_t i = 0; i < (now.CalendarDate.Month-1); i++ )
        {
            timeCounterTemp += (uint32_t)(DaysInMonth[i] * SecondsInDay);
        }
    }

    timeCounterTemp += (uint32_t)((uint32_t)now.CalendarTime.Seconds+
                                  ((uint32_t)now.CalendarTime.Minutes *
                                   SecondsInMinute)+
                                  ((uint32_t)now.CalendarTime.Hours *
                                   SecondsInHour)+
                                  ((uint32_t)(now.CalendarDate.Day *
                                              SecondsInDay)));

    timeCounter = (timeCounterTemp << N_PREDIV_S)+
                  (RTC_SYNCH_PREDIV-now.CalendarTime.SubSeconds);

    return (timeCounter);
}

TimerTime_t
RtcConvertMsToTick(TimerTime_t timeoutValue)
{
    double retVal = 0;
    retVal = round(((double)timeoutValue * CONV_DENOM) / CONV_NUMER);
    return((TimerTime_t)retVal);
}

TimerTime_t
RtcConvertTickToMs(TimerTime_t timeoutValue)
{
    double retVal = 0.0;
    retVal = round(((double)timeoutValue * CONV_NUMER) / CONV_DENOM);
    return((TimerTime_t)retVal);
}

static RtcCalendar_t
RtcGetCalendar(void)
{
    uint32_t first_read = 0;
    uint32_t second_read = 0;
    RtcCalendar_t now;

    // Get Time and Date
    first_read = LL_RTC_TIME_GetSubSecond(RTC);
    second_read = LL_RTC_TIME_GetSubSecond(RTC);

    // make sure it is correct due to asynchronous nature of RTC
    while( first_read != second_read ) {
        first_read = second_read;
        second_read = LL_RTC_TIME_GetSubSecond(RTC);
    }
    now.CalendarTime.Hours =
        __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC));
    now.CalendarTime.Minutes =
        __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC));
    now.CalendarTime.Seconds =
        __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC));
    now.CalendarTime.SubSeconds = second_read;

    now.CalendarDate.WeekDay =
        __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetWeekDay(RTC));
    now.CalendarDate.Month =
        __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetMonth(RTC));
    now.CalendarDate.Day =
        __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetDay(RTC));
    now.CalendarDate.Year =
        __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetYear(RTC));
    return(now);
}

void
RTC_AlarmIRQHandler()
{
    if(LL_RTC_IsEnabledIT_ALRA(RTC)) {
        /* Get the pending status of the AlarmA Interrupt */
        if(LL_RTC_IsActiveFlag_ALRA(RTC)) {
            LL_RTC_ClearFlag_ALRA(RTC);
        }
    }
    if(LL_RTC_IsEnabledIT_ALRB(RTC)) {
        /* Get the pending status of the AlarmB Interrupt */
        if(LL_RTC_IsActiveFlag_ALRB(RTC)) {
            LL_RTC_ClearFlag_ALRB(RTC);
        }
    }
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_17);
}

