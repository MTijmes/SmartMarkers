/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

#include <stm32l0xx_ll_bus.h>
#include <stm32l0xx_ll_cortex.h>
#include <stm32l0xx_ll_pwr.h>
#include <stm32l0xx_ll_rcc.h>
#include <stm32l0xx_ll_system.h>
#include <stm32l0xx_ll_utils.h>

#include <board.h>
#include <i2c.h>
#include <spi.h>
#include <rtc.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ID1             (*(uint32_t*)0x1FF80050)
#define ID2             (*(uint32_t*)0x1FF80054)
#define ID3             (*(uint32_t*)0x1FF80064)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*
 * Note: static variables are initialised to 0 / false.
 */
static bool board_is_initialized;

static uint8_t board_irq_nest_level;

/* Global variables ----------------------------------------------------------*/
extern uint8_t i2c_receive_buffer[];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void
board_sysclk_config(void)
{
    LL_RCC_PLL_Disable();
    /* Set new latency */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    /* HSE configuration and activation */
    LL_RCC_HSE_EnableCSS();
    LL_RCC_HSE_EnableBypass();
    LL_RCC_HSE_Enable();
    while(LL_RCC_HSE_IsReady() != 1);
    LL_PWR_EnableBkUpAccess();
    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
    LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_LOW);
    LL_RCC_LSE_Enable();
    /* Wait till LSE is ready */
    while(LL_RCC_LSE_IsReady() != 1) {}
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
    LL_RCC_EnableRTC();
    /* Main PLL configuration and activation */
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE,
                                LL_RCC_PLL_MUL_8,
                                LL_RCC_PLL_DIV_2);

    LL_RCC_PLL_Enable();
    while(LL_RCC_PLL_IsReady() != 1);

    /* Sysclk activation on the main PLL */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

    /* Set APB1 & APB2 prescaler*/
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    /* Set systick to 1ms in using frequency set to 32MHz */
    LL_Init1msTick(32000000);

    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
    LL_SetSystemCoreClock(32000000);
}

static void
board_sysclk_reconfig(void)
{
    /* TODO: port to low layer driver */

    // __HAL_RCC_PWR_CLK_ENABLE();
    // __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Enable HSE */
    // __HAL_RCC_HSE_CONFIG(RCC_HSE_ON);

    /* Wait till HSE is ready */
    // while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET);

    /* Enable PLL */
    // __HAL_RCC_PLL_ENABLE();

    /* Wait till PLL is ready */
    // while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET);

    /* Select PLL as system clock source */
    // __HAL_RCC_SYSCLK_CONFIG (RCC_SYSCLKSOURCE_PLLCLK);

    /* Wait till PLL is used as system clock source */
    // while(__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK);
}


/* Public functions ----------------------------------------------------------*/
void
board_error_handler(void)
{
    while(1) {
        /* Panic */
    }
}

void
board_irq_enable(void)
{
    board_irq_nest_level--;
    if (board_irq_nest_level == 0) {
        __enable_irq();
    }
}

void
board_irq_disable(void)
{
    __disable_irq();
    board_irq_nest_level++;
}

void
board_init(void)
{
    if (!board_is_initialized) {
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
        NVIC_SetPriority(SVC_IRQn,     2);
        NVIC_SetPriority(PendSV_IRQn,  0);
        NVIC_SetPriority(SysTick_IRQn, 0);

        board_sysclk_config();

        i2c_init(i2c_receive_buffer, I2C_RECEIVE_SIZE);
        spi_init();
        rtc_init();

        board_is_initialized = true;
    } else {
        board_sysclk_reconfig();
    }
}

void
board_deinit(void)
{
}

void
board_delay_ms(uint32_t ms)
{
    LL_mDelay(ms);
}

void
board_get_unique_id(uint8_t *id)
{
    id[7] = (ID1 + ID3) >> 24;
    id[6] = (ID1 + ID3) >> 16;
    id[5] = (ID1 + ID3) >> 8;
    id[4] = (ID1 + ID3);
    id[3] = ID2 >> 24;
    id[2] = ID2 >> 16;
    id[1] = ID2 >> 8;
    id[0] = ID2;
}

uint32_t
board_get_random_seed(void)
{
    return (ID1 ^ ID2 ^ ID3);
}

void
board_sleep(uint32_t sec)
{
    (void)sec;
}
