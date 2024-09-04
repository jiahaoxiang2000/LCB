## issue

### `Panduola` board

the l475vg_iot01a board code is not compatible with the `Panduola` board. the uart pin is different. If you want to use the `Panduola` board, you need to modify the code.

> we pay one day time to find this issue. beacause not famliar with the stm32 lib. The issue key is the pin function reuse for specific peripheral on the particular board.  

```c
// l475vg_iot01a ~/.platformio/packages/framework-arduinoststm32/variants/STM32L4xx/L475V(C-E-G)T_L476V(C-E-G)T_L486VGT/variant_B_L475E_IOT01A.h
// Default pin used for 'Serial' instance (ex: ST-Link)
// Mandatory for Firmata
#ifndef PIN_SERIAL_RX
  #define PIN_SERIAL_RX           PA10
#endif
#ifndef PIN_SERIAL_TX
  #define PIN_SERIAL_TX           PA9
#endif
// ~/.platformio/packages/framework-arduinoststm32/variants/STM32L4xx/L475V(C-E-G)T_L476V(C-E-G)T_L486VGT/PeripheralPins_B_L475E_IOT01A.c
WEAK const PinMap PinMap_UART_TX[] = {
    {PA_9, USART1, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF7_USART1)}, // USB_OTG_FS_VBUS
}
WEAK const PinMap PinMap_UART_RX[] = {
    {PA_10, USART1, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF7_USART1)}, // USB_OTG_FS_ID
}
```

