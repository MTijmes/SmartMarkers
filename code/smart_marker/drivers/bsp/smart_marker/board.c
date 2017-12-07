/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include <board.h>
#include <uart.h>
#include <i2c.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static bool board_is_initialized = false;

static uint8_t irq_nest_level = 0;

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
    irq_nest_level--;
    if (irq_nest_level == 0) {
        __enable_irq();
    }
}

void
board_irq_disable(void)
{
    __disable_irq();
    irq_nest_level++;
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

        uart_init();
        // spi_init();
        i2c_init(i2c_receive_buffer, RECEIVE_SIZE);
        // rtc_init();

        board_is_initialized = true;
    } else {
        board_sysclk_reconfig();
    }
}
