#ifndef UTILITIES_H
#define UTILITIES_H

#define MIN(a, b)   (((a) < (b)) ? (a) : (b))
#define MAX(a, b)   (((a) > (b)) ? (a) : (b))
#define POW2(n)     (1 << n)

int32_t rand1(void);

// Initializes the pseudo random generator initial value
void srand1(uint32_t seed);
int32_t randr(int32_t min, int32_t max);
void memcpy1(uint8_t *dst, const uint8_t *src, uint16_t size);

// Copies size elements of src array to dst array reversing the byte order
void memcpyr(uint8_t *dst, const uint8_t *src, uint16_t size);

/*
 * Set size elements of dst array with value
 *
 * STM32 Standard memset function only works on pointers that are aligned
 */
void memset1(uint8_t *dst, uint8_t value, uint16_t size);
int8_t Nibble2HexChar(uint8_t a);

#endif /* UTILITIES_H */
