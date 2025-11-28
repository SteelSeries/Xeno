/*************************************************************************************************
 *                                         INCLUDES                                              *
 *************************************************************************************************/
#include "power_control.h"
#include "at32f402_405_clock.h"
#include <string.h>

/*************************************************************************************************
 *                                  LOCAL MACRO DEFINITIONS                                      *
 *************************************************************************************************/
/*************************************************************************************************
 *                                  LOCAL TYPE DEFINITIONS                                       *
 *************************************************************************************************/
/*************************************************************************************************
 *                                GLOBAL VARIABLE DEFINITIONS                                    *
 *************************************************************************************************/
/*************************************************************************************************
 *                                STATIC VARIABLE DEFINITIONS                                    *
 *************************************************************************************************/

/*************************************************************************************************
 *                                STATIC FUNCTION DECLARATIONS                                   *
 *************************************************************************************************/
static void SystemClockRecover(void);

/*************************************************************************************************
 *                                GLOBAL FUNCTION DEFINITIONS                                    *
 *************************************************************************************************/
void PowerControl_Resume(void)
{
    printf("resume\n");
}

void PowerControl_EnterDeepSleep(void)
{
    printf("Enter deep sleep\n");
    crm_clocks_freq_type crm_clocks_freq_struct = {0};
    uint32_t saved_enable;
    uint32_t tmp;
    const uint32_t SYSTICK_ENABLE_BIT = (1U << 0); /* SysTick CTRL bit0 = ENABLE */
    /* get system clock */
    crm_clocks_freq_get(&crm_clocks_freq_struct);

    /* save SysTick enable bit */
    saved_enable = (SysTick->CTRL & SYSTICK_ENABLE_BIT) ? 1U : 0U;

    /* disable SysTick (read-modify-write done on local var; write back once) */
    __disable_irq();
    tmp = SysTick->CTRL & ~SYSTICK_ENABLE_BIT;
    SysTick->CTRL = tmp;
    __enable_irq();

    /* ensure previous writes complete and are visible to peripherals */
    __DSB();
    __ISB();

    /* config the voltage regulator mode */
    pwc_voltage_regulate_set(PWC_REGULATOR_EXTRA_LOW_POWER);

    /* enter deep sleep mode */
    pwc_deep_sleep_mode_enter(PWC_DEEP_SLEEP_ENTER_WFI);

    /* after wakeup, restore SysTick enable if it was enabled before */
    if (saved_enable)
    {
        __disable_irq();
        tmp = SysTick->CTRL | SYSTICK_ENABLE_BIT;
        SysTick->CTRL = tmp;
        __enable_irq();
    }

    /* optional barriers to ensure restore completed before proceeding */
    __DSB();
    __ISB();

    /* turn on the led light */
    at32_led_on(LED2);

    /* determine if the debugging function is enabled */
    if ((DEBUGMCU->ctrl & 0x00000007) != 0x00000000)
    {
        /* wait 3 LICK(maximum 120us) cycles to ensure clock stable */
        /* when wakeup from deepsleep,system clock source changes to HICK */
        if ((CRM->misc1_bit.hick_to_sclk == TRUE) && (CRM->misc1_bit.hickdiv == TRUE))
        {
            /* HICK is 48MHz */
            delay_us(((120 * 6 * HICK_VALUE) / crm_clocks_freq_struct.sclk_freq) + 1);
        }
        else
        {
            /* HICK is 8MHz */
            delay_us(((120 * HICK_VALUE) / crm_clocks_freq_struct.sclk_freq) + 1);
        }
    }

    /* config the system clock */
    SystemClockRecover();
    printf("Exit deep sleep\n");
}

/*************************************************************************************************
 *                                STATIC FUNCTION DEFINITIONS                                    *
 *************************************************************************************************/
static void SystemClockRecover(void)
{
  /* enable external high-speed crystal oscillator - hext */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);

  /* wait till hext is ready */
  while(crm_hext_stable_wait() == ERROR);

  /* config pllu div */
  crm_pllu_div_set(CRM_PLL_FU_18);

  /* enable pll */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

  /* wait till pll is ready */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) == RESET);

  /* enable auto step mode */
  crm_auto_step_mode_enable(TRUE);

  /* select pll as system clock source */
  crm_sysclk_switch(CRM_SCLK_PLL);

  /* wait till pll is used as system clock source */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL);
}
