#include "stm32l0xx.h"

/*
 * ConfigureGPIO()
 */
inline void ConfigureGPIO (void)
{  
  /* enable the peripheral clock of GPIOA */
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

  /* select output mode on PA5 */
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE5)) | (GPIO_MODER_MODE5_0);
}

/*
 * main function
 */			
int main(void)
{
  ConfigureGPIO();

  while(1)
    {
      /* toogle pin PA5 */
      GPIOA->ODR ^= (1 << 5);

      /* some delay */
      for (int i = 0; i < 100000; ++i);
    }

  return 0;
}
