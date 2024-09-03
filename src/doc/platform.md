## issue

### `Panduola` board

the l475vg_iot01a board code is not compatible with the `Panduola` board. the uart pin is different. If you want to use the `Panduola` board, you need to modify the code.

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
```

