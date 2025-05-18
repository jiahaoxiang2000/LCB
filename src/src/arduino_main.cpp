//
// NIST-developed software is provided by NIST as a public service.
// You may use, copy and distribute copies of the software in any medium,
// provided that you keep intact this entire notice. You may improve,
// modify and create derivative works of the software or any portion of
// the software, and you may copy and distribute such modifications or
// works. Modified works should carry a notice stating that you changed
// the software and should note the date and nature of any such change.
// Please explicitly acknowledge the National Institute of Standards and
// Technology as the source of the software.
//
// NIST-developed software is expressly provided "AS IS." NIST MAKES NO
// WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION
// OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
// NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
// UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST
// DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE
// OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY,
// RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
//
// You are solely responsible for determining the appropriateness of using and
// distributing the software and you assume all risks associated with its use,
// including but not limited to the risks and costs of program errors, compliance
// with applicable laws, damage to or loss of data, programs or equipment, and
// the unavailability or interruption of operation. This software is not intended
// to be used in any situation where a failure could cause risk of injury or
// damage to property. The software developed by NIST employees is not subject to
// copyright protection within the United States.
//

#include <Arduino.h>
#include "lwc_benchmark.h"

// Disable watchdog timer on ESP for KAT and Timing experiments
#if defined(LWC_PLATFORM_NODEMCUV2) && !defined(LWC_EXPERIMENT_SIZE)
#include <Esp.h>
#endif

#ifdef LWC_PLATFORM_l475vg
// stm32l4xx_hal_uart.h, these is support by the STM32CubeL4 HAL library
UART_HandleTypeDef huart1;

// Define the serial pins explicitly for the STM32L475VG board
#define SERIAL_TX_PIN PA9
#define SERIAL_RX_PIN PA10
const PinMap PinMap_UART_TX[] = {
    {PA_9, USART1, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF7_USART1)}, // USB_OTG_FS_VBUS
};
const PinMap PinMap_UART_RX[] = {
    {PA_10, USART1, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF7_USART1)}, // USB_OTG_FS_ID
};
#endif

void setup()
{

// Don't use IO for size experiments to reduce the overall size of the binary
#ifndef LWC_EXPERIMENT_SIZE

// initialize LED digital pin as an output and open the serial port
#ifdef LWC_PLATFORM_l475vg
  pinMode(PE9, OUTPUT);

  // Initialize Serial with specific pins PA9 (TX) and PA10 (RX)
  // The setPins method must be called before begin()
  Serial.setRx(SERIAL_RX_PIN);
  Serial.setTx(SERIAL_TX_PIN);
  Serial.begin(9600);
  Serial.println();
  stop_watch(3);
#elif LCB_PLATFORM_esp32s3
  pinMode(GPIO_NUM_1, OUTPUT);
  Serial0.begin(9600);
  Serial0.println();
  stop_watch(5);
#endif
#endif

  int ret = do_experiments();

#ifndef LWC_EXPERIMENT_SIZE
  SOUT << "# LCB exit " << SENDL;
#endif
}

void loop()
{
#ifdef LCB_PLATFORM_esp32s3
  // let the eps32s3 board blink
  gpio_set_level(GPIO_NUM_1, 1);
  delay(1000);
  gpio_set_level(GPIO_NUM_1, 0);
  delay(1000);

#endif
// turn the LED on (HIGH is the voltage level)
#ifdef LWC_PLATFORM_l475vg
  digitalWrite(PE9, HIGH);
  // wait for a second
  delay(1000);
  // turn the LED off by making the voltage LOW
  digitalWrite(PE9, LOW);
  // wait for a second
  delay(1000);
#endif
}
