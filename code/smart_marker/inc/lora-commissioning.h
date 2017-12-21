#ifndef LORA_COMMISSIONING_H
#define LORA_COMMISSIONING_H

/* Includes ------------------------------------------------------------------*/
/* Public types --------------------------------------------------------------*/
/* Public constants ----------------------------------------------------------*/
#define OVER_THE_AIR_ACTIVATION                     0
#define LORAWAN_PUBLIC_NETWORK                      true

#define IEEE_OUI                                    0x00, 0x59, 0xAC
#define LORAWAN_DEVICE_EUI                          { IEEE_OUI, 0x00, 0x00, 0x18, 0x01, 0xC5 }

#define LORAWAN_APPLICATION_EUI                     { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define LORAWAN_APPLICATION_KEY                     { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C }
#define LORAWAN_NETWORK_ID                          ( uint32_t )0

#define LORAWAN_DEVICE_ADDRESS                      ( uint32_t )0x142031C6

#define LORAWAN_NWKSKEY                             { 0xA7, 0xBA, 0xA9, 0xE5, 0x8B, 0x9F, 0xCA, 0x70, 0xC3, 0xD7, 0xB6, 0xBE, 0x65, 0x12, 0xA6, 0xCB }
#define LORAWAN_APPSKEY                             { 0xC7, 0xF3, 0xAA, 0xF1, 0x48, 0xE4, 0x35, 0x30, 0x82, 0x72, 0xED, 0xF3, 0x30, 0x92, 0x35, 0x6E }

/* Public macro --------------------------------------------------------------*/
/* Public functions --------------------------------------------------------- */

#endif /* LORA_COMMISSIONING_H */
