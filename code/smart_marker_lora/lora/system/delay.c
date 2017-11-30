#include <board.h>

void Delay(float s)
{
    DelayMs(s * 1000.0f);
}

void DelayMs(uint32_t ms)
{
    HAL_Delay(ms);
}
