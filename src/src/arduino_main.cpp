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

UART_HandleTypeDef huart1;

void setup()
{

// Don't use IO for size experiments to reduce the overall size of the binary
#ifndef LWC_EXPERIMENT_SIZE

  // initialize LED digital pin as an output.
  pinMode(PE9, OUTPUT);

  Serial.begin(9600);
  Serial.println();

  // Wait for a few seconds before running the experiments in order to allow
  // enonugh time for opening the terminal window if run from the IDE
  stop_watch(1);
#endif

  int ret = do_experiments();

#ifndef LWC_EXPERIMENT_SIZE
  SOUT << "# lCB exit " << SENDL;
#endif
}

void loop()
{
  // turn the LED on (HIGH is the voltage level)
  digitalWrite(PE9, HIGH);
  // wait for a second
  delay(1000);
  // turn the LED off by making the voltage LOW
  digitalWrite(PE9, LOW);
  // wait for a second
  delay(1000);
}

#ifdef LWC_PLATFORM_l475vg

// void SystemClock_Config(void) {
//   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//   RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

//   /** Configure LSE Drive Capability
//   */
//   HAL_PWR_EnableBkUpAccess();
//   __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
//   /** Initializes the CPU, AHB and APB busses clocks
//   */
//   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
//                               |RCC_OSCILLATORTYPE_LSE;
//   RCC_OscInitStruct.HSEState = RCC_HSE_ON;
//   RCC_OscInitStruct.LSEState = RCC_LSE_ON;
//   RCC_OscInitStruct.LSIState = RCC_LSI_ON;
//   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
//   RCC_OscInitStruct.PLL.PLLM = 1;
//   RCC_OscInitStruct.PLL.PLLN = 20;
//   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
//   RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
//   RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
//   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   /** Initializes the CPU, AHB and APB busses clocks
//   */
//   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                               |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
//   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
//   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

//   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
//                               |RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USB
//                               |RCC_PERIPHCLK_ADC;
//   PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
//   PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
//   PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
//   PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
//   PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
//   PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
//   PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
//   PeriphClkInit.PLLSAI1.PLLSAI1N = 12;
//   PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
//   PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
//   PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
//   PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK|RCC_PLLSAI1_ADC1CLK;
//   if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   /** Configure the main internal regulator output voltage
//   */
//   if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
//   {
//     Error_Handler();
//   }
// }


#endif
