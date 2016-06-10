/*
 * Copyright (c) 2016, Ari Suutari <ari@stonepile.fi>.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <picoos.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "port_irq.h"
#include <stdbool.h>

#if PORTCFG_TICK_RTC == 1

/*
 * This tick module runs with or without periodic tick. If periodic
 * tick is used, RTC wakeup counter is used to generate it. 
 * In tickless mode, SysTick is used to generate tick and RTC is
 * used for wakeup from tickless sleep.
 */
void portInitClock(void)
{
#if POSCFG_FEATURE_TICKLESS != 0

/*
 * Initialize Systick timer.
 */

  SysTick_Config(SystemCoreClock / HZ);
  NVIC_SetPriority(SysTick_IRQn, PORT_SYSTICK_PRI);

#endif

/*
 * Initialize RTC.
 */

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  PWR_BackupAccessCmd(ENABLE);

  RCC_LSEConfig(RCC_LSE_ON);
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {
  }

  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  RCC_RTCCLKCmd(ENABLE);
  RTC_WaitForSynchro();

  RTC_InitTypeDef RTC_InitStructure;

/*
 * RTC ticks every sec. Asynch/Synchr dividers
 * are set so that we have 1024 resolution in subseconds.
 */

  RTC_InitStructure.RTC_AsynchPrediv = 0x1f;
  RTC_InitStructure.RTC_SynchPrediv  = 0x3ff;
  RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
  RTC_Init(&RTC_InitStructure);

  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;

  time.RTC_H12     = 0;
  time.RTC_Hours   = 0;
  time.RTC_Minutes = 0;
  time.RTC_Seconds = 0;
  date.RTC_Date    = 1;
  date.RTC_Month   = 1;
  date.RTC_Year    = 0;
  date.RTC_WeekDay = RTC_Weekday_Saturday; // 1.1.2000: saturday

  RTC_SetTime(RTC_Format_BIN, &time);
  RTC_SetDate(RTC_Format_BIN, &date);

/*
 * Configure Wakup interrupt
 */
  
  EXTI_InitTypeDef EXTI_InitStructure;
  EXTI_InitStructure.EXTI_Line = EXTI_Line22;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
 
  NVIC_SetPriority(RTC_WKUP_IRQn, PORT_SYSTICK_PRI);

  RTC_WakeUpCmd(DISABLE);

#if POSCFG_FEATURE_TICKLESS == 0

  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);
  RTC_SetWakeUpCounter(16384 / HZ);

  EXTI_ClearITPendingBit(EXTI_Line22);
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  RTC_ClearITPendingBit(RTC_IT_WUT);
  RTC_ClearFlag(RTC_FLAG_WUTF);
  NVIC_EnableIRQ(RTC_WKUP_IRQn);

  RTC_WakeUpCmd(ENABLE);

#endif

}

#if POSCFG_FEATURE_TICKLESS != 0

static long startTime;

static long rtcTimeNow()
{
  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;
  struct tm t;

  RTC_GetTime(RTC_Format_BIN, &time);
  RTC_GetDate(RTC_Format_BIN, &date);

  memset(&t, '\0', sizeof(t));

  t.tm_sec  = time.RTC_Seconds;
  t.tm_min  = time.RTC_Minutes;
  t.tm_hour = time.RTC_Hours;
  t.tm_mday = date.RTC_Date;
  t.tm_mon  = date.RTC_Month - 1;
  t.tm_year = date.RTC_Year + 2000 - 1900;

  time_t secs;
  long msecs;

  secs = mktime(&t);
  msecs = 1000 - (RTC_GetSubSecond() * 1000 / 1024);

  return 1000 * secs + msecs;
}

#ifndef PORTCFG_POWER_TICKLESS_SAFETY_MARGIN
#define PORTCFG_POWER_TICKLESS_SAFETY_MARGIN MS(3)
#endif

/*
 * Turn of timer tick and schedule wakeup
 * after given ticks.
 */
void p_pos_powerTickSuspend(UVAR_t ticks)
{
/*
 * First, systick off.
 */
  SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk |
                     SysTick_CTRL_ENABLE_Msk);
/*
 * Then, schedule RTC wakeup after ticks have passed.
 */
  if (ticks == INFINITE)
    return;

  if (ticks / HZ > 65536) // max 18 hours
    ticks = 65536 * HZ;

  ticks -= PORTCFG_POWER_TICKLESS_SAFETY_MARGIN; // oscillator starts up 2 ms

  RTC_WakeUpCmd(DISABLE);
  if (ticks <= MS(10000)) {

    // Wakeup timer ticks at 32768 / 8 = 4096 Hz
    RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div8);
    RTC_SetWakeUpCounter(4096 * ticks / HZ);
  }
  else {

    // Wakeup timer ticks at 1 Hz
    RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
    RTC_SetWakeUpCounter(ticks / HZ);
  }

  EXTI_ClearITPendingBit(EXTI_Line22);
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  RTC_ClearITPendingBit(RTC_IT_WUT);
  RTC_ClearFlag(RTC_FLAG_WUTF);
  NVIC_EnableIRQ(RTC_WKUP_IRQn);

  startTime = rtcTimeNow();
  RTC_WakeUpCmd(ENABLE);
}

void p_pos_powerTickResume()
{
/*
 * Make sure that wakeup counter is disabled.
 */
  NVIC_DisableIRQ(RTC_WKUP_IRQn);
  RTC_WakeUpCmd(DISABLE);
  RTC_ClearITPendingBit(RTC_IT_WUT);
  EXTI_ClearITPendingBit(EXTI_Line22);

/*
 * Do application-specific clock setup after sleep.
 */
  portRestoreClocksAfterWakeup();

/*
 * Now step the timer with amount of ticks we really slept.
 */
  c_pos_timerStep(HZ * (rtcTimeNow() - startTime) / 1000);

/*
 * Safe to enable systick now.
 */
  SysTick->CTRL |= (SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk);
}

/*
 * Timer interrupt from SysTick.
 */
void SysTick_Handler()
{
  c_pos_intEnter();
  c_pos_timerInterrupt();
  c_pos_intExitQuick();
}

#endif

/*
 * RTC wakeup interrupt handler.
 */
void RTC_WKUP_IRQHandler(void)
{
#if POSCFG_FEATURE_TICKLESS != 0

  if (RTC_GetITStatus(RTC_IT_WUT) != RESET) {

/*
 * Stop wakeup counter and ensure 
 * system wakes up from sleep.
 */
    RTC_WakeUpCmd(DISABLE);
    RTC_ClearITPendingBit(RTC_IT_WUT);
    EXTI_ClearITPendingBit(EXTI_Line22);
    SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
  }

#else

  c_pos_intEnter();

  if (RTC_GetITStatus(RTC_IT_WUT) != RESET) {

    RTC_ClearITPendingBit(RTC_IT_WUT);
    EXTI_ClearITPendingBit(EXTI_Line22);
    c_pos_timerInterrupt();
  }

  c_pos_intExitQuick();

#endif
}

#endif
