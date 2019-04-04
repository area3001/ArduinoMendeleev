/*
  Copyright (c) 2019 Bert Outtier.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
  Timer usage:

  +------------+--------+----------------+-------------+-------------+
  | Pin number |  PIN   | Mendeleev      | E           |  F          |
  +------------+--------+----------------+-------------+-------------+
  | 0          |  PA11  |  M1CTRL0_PIN   |  TCC0/WO[3] | *TCC1/WO[1] |
  | 1          |  PA10  |  M2CTRL0_PIN   |  TCC0/WO[2] | *TCC1/WO[0] |
  | 2          |  PA14  |  PIN_SPI_CS0   |  TC3/WO[0]  |  TCC0/WO[4] |
  | 3          |  PA09  |  M2CTRL1_PIN   |  TCC0/WO[1] |  TCC1/WO[3] |
  | 4          |  PA08  |  M2CTRL2_PIN   | *TCC0/WO[0] |  TCC1/WO[2] |
  | 5          |  PA15  |  LED_R_PIN     |  TC3/WO[1]  | *TCC0/WO[5] |
  | 6          |  PA20  |  LED_UV_PIN    |             | *TCC0/WO[6] |
  | 7          |  PA21  |  PIN_SPI_CS1   |             |  TCC0/WO[7] |
  | 8          |  PA06  |  M1CTRL2_PIN   |  TCC1/WO[0] |             |
  | 9          |  PA07  |  M1CTRL1_PIN   |  TCC1/WO[1] |             |
  | 10         |  PA18  |  LED_A_PIN     | *TC3/WO[0]  |  TCC0/WO[2] |
  | 11         |  PA16  |  LED_G_PIN     | *TCC2/WO[0] |  TCC0/WO[6] |
  | 12         |  PA19  |  LED_W_PIN     |  TC3/WO[1]  | *TCC0/WO[3] |
  | 13         |  PA17  |  LED_B_PIN     | *TCC2/WO[1] |  TCC0/WO[7] |
  | 14         |  PA02  |  INPUT2_PIN    |             |             |
  | 15         |  PB08  |  INPUT0_PIN    |  TC4/WO[0]  |             |
  | 16         |  PB09  |  INPUT1_PIN    |  TC4/WO[1]  |             |
  | 17         |  PA04  |  INPUT3_PIN    |  TCC0/WO[0] |             |
  | 18         |  PA05  |  PROX_PIN      |  TCC0/WO[1] |             |
  | 19         |  PB02  |  RS485_DIR_PIN |             |             |
  | 20         |  PA22  |  SDA           |  TC4/WO[0]  |  TCC0/WO[4] |
  | 21         |  PA23  |  SCL           |  TC4/WO[1]  |  TCC0/WO[5] |
  | 22         |  PA12  |  MISO          |  TCC2/WO[0] |  TCC0/WO[6] |
  | 23         |  PB10  |  MOSI          |  TC5/WO[0]  |  TCC0/WO[4] |
  | 24         |  PB11  |  SCK           |  TC5/WO[1]  |  TCC0/WO[5] |
  | 34         |  PA19  |                |  TC3/WO[1]  |  TCC0/WO[3] |
  | 35         |  PA16  |                |  TCC2/WO[0] |  TCC0/WO[6] |
  | 36         |  PA18  |                | *TC3/WO[0]  |  TCC0/WO[2] |
  | 37         |  PA17  |                | *TCC2/WO[1] |  TCC0/WO[7] |
  | 38         |  PA13  |  LED_TXT_PIN   |  TCC2/WO[1] | *TCC0/WO[7] |
  | 41         |  PA30  |                |  TCC1/WO[0] |             |
  | 42         |  PA31  |                |  TCC1/WO[1] |             |
  |            |  PA00  |                |  TCC2/WO[0] |             |
  |            |  PA01  |                |  TCC2/WO[1] |             |
  +------------+--------+----------------+-------------+-------------+

  +-------+---------+-------------------------------------------+--------------+
  | Timer | Channel | Pin                                       | Mendeleev    |
  +-------+---------+-------------------------------------------+--------------+
  | TCC0  |  0 - 4  | PA14, *PA08, PA04, PA22, PB10             | M2CTRL2_PIN? |
  | TCC0  |  1 - 5  | PA09, *PA15, PA05, PA23, PB11             | Red          |
  | TCC0  |  2 - 6  | PA10, *PA20, PA18, PA16, PA12             | UV           |
  | TCC0  |  3 - 7  | PA11, PA21, PA19, PA17, *PA13             | TXT          |
  | TCC1  |  0 - 2  | *PA10, PA08, PA06, PA30                   | M2CTRL0_PIN? |
  | TCC1  |  1 - 3  | *PA11, PA09, PA07, PA31                   | M1CTRL0_PIN? |
  | TCC2  |  0      | *PA16, PA00                               | Green        |
  | TCC2  |  1      | PA16, *PA17, PA12, PA13, PA01             | Blue         |
  | TC3   |  0      | PA14, *PA18                               | Alpha        |
  | TC3   |  1      | PA15, *PA19                               | White        |
  | TC4   |  0      | *PB08, PA22                               |              |
  | TC4   |  1      | *PB09, PA23                               |              |
  | TC5   |  0      | PB10                                      |              |
  | TC5   |  1      | PB11                                      |              |
  +-------+---------+-------------------------------------------+--------------+
*/

/*
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  | Pin number |  ZERO Board pin  |  PIN   | Label/Name      | Mendeleev      | Comments (* is for default peripheral in use)                                                          |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  |            | Digital Low      |        |                 |                |                                                                                                        |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  | 0          | 0 -> RX          |  PA11  |                 |  M1CTRL0_PIN   | EIC/EXTINT[11] ADC/AIN[19]           PTC/X[3]  SERCOM0/PAD[3]  SERCOM2/PAD[3]  TCC0/WO[3] *TCC1/WO[1]  |
  | 1          | 1 <- TX          |  PA10  |                 |  M2CTRL0_PIN   | EIC/EXTINT[10] ADC/AIN[18]           PTC/X[2]  SERCOM0/PAD[2]                  TCC0/WO[2] *TCC1/WO[0]  |
  | 2          | 2                |  PA14  |                 |  PIN_SPI_CS0   | EIC/EXTINT[14]                                 SERCOM2/PAD[2]  SERCOM4/PAD[2]  TC3/WO[0]   TCC0/WO[4]  |
  | 3          | ~3               |  PA09  |                 |  M2CTRL1_PIN   | EIC/EXTINT[9]  ADC/AIN[17]           PTC/X[1]  SERCOM0/PAD[1]  SERCOM2/PAD[1]  TCC0/WO[1]  TCC1/WO[3]  |
  | 4          | ~4               |  PA08  |                 |  M2CTRL2_PIN   | EIC/NMI        ADC/AIN[16]           PTC/X[0]  SERCOM0/PAD[0]  SERCOM2/PAD[0] *TCC0/WO[0]  TCC1/WO[2]  |
  | 5          | ~5               |  PA15  |                 |  LED_R_PIN     | EIC/EXTINT[15]                                 SERCOM2/PAD[3]  SERCOM4/PAD[3]  TC3/WO[1]  *TCC0/WO[5]  |
  | 6          | ~6               |  PA20  |                 |  LED_UV_PIN    | EIC/EXTINT[4]                        PTC/X[8]  SERCOM5/PAD[2]  SERCOM3/PAD[2]             *TCC0/WO[6]  |
  | 7          | 7                |  PA21  |                 |  PIN_SPI_CS1   | EIC/EXTINT[5]                        PTC/X[9]  SERCOM5/PAD[3]  SERCOM3/PAD[3]              TCC0/WO[7]  |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  |            | Digital High     |        |                 |                |                                                                                                        |
  +------------+------------------+--------+-----------------+-------------------------------------------------------------------------------------------------------------------------+
  | 8          | ~8               |  PA06  |                 |  M1CTRL2_PIN   | EIC/EXTINT[6]  ADC/AIN[6]  AC/AIN[2] PTC/Y[4]  SERCOM0/PAD[2]                  TCC1/WO[0]              |
  | 9          | ~9               |  PA07  |                 |  M1CTRL1_PIN   | EIC/EXTINT[7]  ADC/AIN[7]  AC/AIN[3] PTC/Y[5]  SERCOM0/PAD[3]                  TCC1/WO[1]              |
  | 10         | ~10              |  PA18  |                 |  LED_A_PIN     | EIC/EXTINT[2]                        PTC/X[6] +SERCOM1/PAD[2]  SERCOM3/PAD[2] *TC3/WO[0]    TCC0/WO[2] |
  | 11         | ~11              |  PA16  |                 |  LED_G_PIN     | EIC/EXTINT[0]                        PTC/X[4] +SERCOM1/PAD[0]  SERCOM3/PAD[0] *TCC2/WO[0]   TCC0/WO[6] |
  | 12         | ~12              |  PA19  |                 |  LED_W_PIN     | EIC/EXTINT[3]                        PTC/X[7] +SERCOM1/PAD[3]  SERCOM3/PAD[3] *TC3/WO[1]    TCC0/WO[3] |
  | 13         | ~13              |  PA17  | LED             |  LED_B_PIN     | EIC/EXTINT[1]                        PTC/X[5] +SERCOM1/PAD[1]  SERCOM3/PAD[1] *TCC2/WO[1]   TCC0/WO[7] |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  |            | Analog Connector |        |                 |                |                                                                                                        |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  | 14         | A0               |  PA02  | A0              |  INPUT2_PIN    | EIC/EXTINT[2] *ADC/AIN[0]  DAC/VOUT  PTC/Y[0]                                                          |
  | 15         | A1               |  PB08  | A1              |  INPUT0_PIN    | EIC/EXTINT[8] *ADC/AIN[2]            PTC/Y[14] SERCOM4/PAD[0]                  TC4/WO[0]               |
  | 16         | A2               |  PB09  | A2              |  INPUT1_PIN    | EIC/EXTINT[9] *ADC/AIN[3]            PTC/Y[15] SERCOM4/PAD[1]                  TC4/WO[1]               |
  | 17         | A3               |  PA04  | A3              |  INPUT3_PIN    | EIC/EXTINT[4] *ADC/AIN[4]  AC/AIN[0] PTC/Y[2]  SERCOM0/PAD[0]                  TCC0/WO[0]              |
  | 18         | A4               |  PA05  | A4              |  PROX_PIN      | EIC/EXTINT[5] *ADC/AIN[5]  AC/AIN[1] PTC/Y[5]  SERCOM0/PAD[1]                  TCC0/WO[1]              |
  | 19         | A5               |  PB02  | A5              |  RS485_DIR_PIN | EIC/EXTINT[2] *ADC/AIN[10]           PTC/Y[8]  SERCOM5/PAD[0]                                          |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  |            | Wire             |        |                 |                |                                                                                                        |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  | 20         | SDA              |  PA22  | SDA             |  SDA           | EIC/EXTINT[6]                        PTC/X[10] *SERCOM3/PAD[0] SERCOM5/PAD[0] TC4/WO[0] TCC0/WO[4]     |
  | 21         | SCL              |  PA23  | SCL             |  SCL           | EIC/EXTINT[7]                        PTC/X[11] *SERCOM3/PAD[1] SERCOM5/PAD[1] TC4/WO[1] TCC0/WO[5]     |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  |            |SPI (Legacy ICSP) |        |                 |                |                                                                                                        |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  | 22         | 1                |  PA12  | MISO            |  MISO          | EIC/EXTINT[12] SERCOM2/PAD[0] *SERCOM4/PAD[0] TCC2/WO[0] TCC0/WO[6]                                    |
  |            | 2                |        | 5V0             |                |                                                                                                        |
  | 23         | 4                |  PB10  | MOSI            |  MOSI          | EIC/EXTINT[10]                *SERCOM4/PAD[2] TC5/WO[0]  TCC0/WO[4]                                    |
  | 24         | 3                |  PB11  | SCK             |  SCK           | EIC/EXTINT[11]                *SERCOM4/PAD[3] TC5/WO[1]  TCC0/WO[5]                                    |
  |            | 5                |        | RESET           |                |                                                                                                        |
  |            | 6                |        | GND             |                |                                                                                                        |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  |            | LEDs             |        |                 |                |                                                                                                        |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  | 25         |                  |  PB03  | RX              |                |                                                                                                        |
  | 26         |                  |  PA27  | TX              |                |                                                                                                        |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  |            | USB              |        |                 |                |                                                                                                        |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  | 27         |                  |  PA28  | USB_HOST_ENABLE |                | EIC/EXTINT[8]                                                                                          |
  | 28         |                  |  PA24  | USB_NEGATIVE    |                | *USB/DM                                                                                                |
  | 29         |                  |  PA25  | USB_POSITIVE    |                | *USB/DP                                                                                                |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  |            | EDBG             |        |                 |                |                                                                                                        |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  | 30         |                  |  PB22  | EDBG_UART TX    |  RS485 TX      | *SERCOM5/PAD[2]                                                                                        |
  | 31         |                  |  PB23  | EDBG_UART RX    |  RS485 RX      | *SERCOM5/PAD[3]                                                                                        |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  | 32         |                  |  PA22  | EDBG_SDA        |                | Pin 20 (SDA)                                                                                           |
  | 33         |                  |  PA23  | EDBG_SCL        |                | Pin 21 (SCL)                                                                                           |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  | 34         |                  |  PA19  | EDBG_MISO       |                | EIC/EXTINT[3] *SERCOM1/PAD[3] SERCOM3/PAD[3] TC3/WO[1]  TCC0/WO[3]                                     |
  | 35         |                  |  PA16  | EDBG_MOSI       |                | EIC/EXTINT[0] *SERCOM1/PAD[0] SERCOM3/PAD[0] TCC2/WO[0] TCC0/WO[6]                                     |
  | 36         |                  |  PA18  | EDBG_SS         |                | EIC/EXTINT[2] *SERCOM1/PAD[2] SERCOM3/PAD[2] TC3/WO[0]  TCC0/WO[2]                                     |
  | 37         |                  |  PA17  | EDBG_SCK        |                | EIC/EXTINT[1] *SERCOM1/PAD[1] SERCOM3/PAD[1] TCC2/WO[1] TCC0/WO[7]                                     |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  | 38         | ATN              |  PA13  | EDBG_GPIO0      |  LED_TXT_PIN   | EIC/EXTINT[13] SERCOM2/PAD[1] SERCOM4/PAD[1] TCC2/WO[1] *TCC0/WO[7]                                    |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  |            |                  |        |                 |                |                                                                                                        |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  |            | GND              |        |                 |                |                                                                                                        |
  | 39         | AREF             |  PA03  |                 |                | EIC/EXTINT[3] *[ADC|DAC]/VREFA ADC/AIN[1] PTC/Y[1]                                                     |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  | 40         |                  |  PA02  |                 |                | Alternate use of A0 (DAC output)                                                                       |
  | 41         |                  |  PA30  |                 |                | SWCLK, alternate use EXTINT[10] TCC1/WO[0] SERCOM1/PAD[2]                                              |
  | 42         |                  |  PA31  |                 |                | SWDIO, alternate use EXTINT[11] TCC1/WO[1] SERCOM1/PAD[3]                                              |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  |            |32.768KHz Crystal |        |                 |                |                                                                                                        |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
  |            |                  |  PA00  | XIN32           |                | EIC/EXTINT[0] SERCOM1/PAD[0] TCC2/WO[0]                                                                |
  |            |                  |  PA01  | XOUT32          |                | EIC/EXTINT[1] SERCOM1/PAD[1] TCC2/WO[1]                                                                |
  +------------+------------------+--------+-----------------+----------------+--------------------------------------------------------------------------------------------------------+
*/


#include "variant.h"

/*
 * Pins descriptions
 */
const PinDescription g_APinDescription[]=
{
  // 0..13 - Digital pins
  // ----------------------
  // M1CTRL0_PIN
  { PORTA, 11, PIO_TIMER_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), No_ADC_Channel, PWM1_CH0, TCC1_CH1, EXTERNAL_INT_11 }, // EIC/EXTINT[11] TCC0/WO[3] *TCC1/WO[1]
  // M2CTRL0_PIN
  { PORTA, 10, PIO_TIMER_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), No_ADC_Channel, PWM1_CH0, TCC1_CH0, EXTERNAL_INT_10 }, // EIC/EXTINT[10] TCC0/WO[2] *TCC1/WO[0]

  // 2..12
  // Digital Low
  // PIN_SPI_CS0
  { PORTA, 14, PIO_DIGITAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_14 },
  // M2CTRL1_PIN (No PWM)
  { PORTA,  9, PIO_DIGITAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_9 },
  // M2CTRL2_PIN
  { PORTA,  8, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM0_CH0, TCC0_CH0, EXTERNAL_INT_NMI },  // TCC0/WO[0]
  // LED R
  { PORTA, 15, PIO_TIMER_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), No_ADC_Channel, PWM0_CH5, TCC0_CH5, EXTERNAL_INT_15 }, //  TCC0/WO[5]
  // LED UV
  { PORTA, 20, PIO_TIMER_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), No_ADC_Channel, PWM0_CH6, TCC0_CH6, EXTERNAL_INT_4 }, // TCC0/WO[6]
  // PIN_SPI_CS1
  { PORTA, 21, PIO_DIGITAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_5 },

  // Digital High
  // M1CTRL2_PIN (No PWM)
  { PORTA,  6, PIO_DIGITAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_6 },
  // M1CTRL1_PIN (No PWM)
  { PORTA,  7, PIO_DIGITAL, (PIN_ATTR_DIGITAL), No_ADC_Channel,  NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_7 },
  // LED A
  { PORTA, 18, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM3_CH0, TC3_CH0, EXTERNAL_INT_2 }, // TC3/WO[0]
  // LED G
  { PORTA, 16, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM2_CH0, TCC2_CH0, EXTERNAL_INT_0 }, // TCC2/WO[0]
  // LED W
  { PORTA, 19, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM3_CH1, TC3_CH1, EXTERNAL_INT_3 }, // TC3/WO[1]
  // LED B
  { PORTA, 17, PIO_PWM, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM2_CH1, TCC2_CH1, EXTERNAL_INT_1 }, // TCC2/WO[1]

  // 14..19 - Analog pins
  // --------------------
  { PORTA,  2, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_Channel0, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_2 }, // ADC/AIN[0]
  { PORTB,  8, PIO_ANALOG, (PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel2, PWM4_CH0, TC4_CH0, EXTERNAL_INT_8 }, // ADC/AIN[2]
  { PORTB,  9, PIO_ANALOG, (PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel3, PWM4_CH1, TC4_CH1, EXTERNAL_INT_9 }, // ADC/AIN[3]
  { PORTA,  4, PIO_ANALOG, 0, ADC_Channel4, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_4 }, // ADC/AIN[4]
  { PORTA,  5, PIO_ANALOG, 0, ADC_Channel5, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_5 }, // ADC/AIN[5]
  { PORTB,  2, PIO_ANALOG, 0, ADC_Channel10, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_2 }, // ADC/AIN[10]

  // 20..21 I2C pins (SDA/SCL and also EDBG:SDA/SCL)
  // ----------------------
  { PORTA, 22, PIO_SERCOM, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_6 }, // SDA: SERCOM3/PAD[0]
  { PORTA, 23, PIO_SERCOM, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_7 }, // SCL: SERCOM3/PAD[1]

  // 22..24 - SPI pins (ICSP:MISO,SCK,MOSI)
  // ----------------------
  { PORTA, 12, PIO_SERCOM_ALT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_12 }, // MISO: SERCOM4/PAD[0]
  { PORTB, 10, PIO_SERCOM_ALT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_10 }, // MOSI: SERCOM4/PAD[2]
  { PORTB, 11, PIO_SERCOM_ALT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_11 }, // SCK: SERCOM4/PAD[3]

  // 25..26 - RX/TX LEDS (PB03/PA27)
  // --------------------
  { PORTB,  3, PIO_OUTPUT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // used as output only
  { PORTA, 27, PIO_OUTPUT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // used as output only

  // 27..29 - USB
  // --------------------
  { PORTA, 28, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // USB Host enable
  { PORTA, 24, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // USB/DM
  { PORTA, 25, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // USB/DP

  // 30..41 - EDBG
  // ----------------------
  // 30/31 - EDBG/UART
  { PORTB, 22, PIO_SERCOM_ALT, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // TX: SERCOM5/PAD[2]
  { PORTB, 23, PIO_SERCOM_ALT, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // RX: SERCOM5/PAD[3]

  // 32/33 I2C (SDA/SCL and also EDBG:SDA/SCL)
  { PORTA, 22, PIO_SERCOM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // SDA: SERCOM3/PAD[0]
  { PORTA, 23, PIO_SERCOM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // SCL: SERCOM3/PAD[1]

  // 34..37 - EDBG/SPI
  { PORTA, 19, PIO_SERCOM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // MISO: SERCOM1/PAD[3]
  { PORTA, 16, PIO_SERCOM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // MOSI: SERCOM1/PAD[0]
  { PORTA, 18, PIO_SERCOM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // SS: SERCOM1/PAD[2]
  { PORTA, 17, PIO_SERCOM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // SCK: SERCOM1/PAD[1]

  // 38..41 - EDBG/Digital
  // LED TXT
  { PORTA, 13, PIO_TIMER_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), No_ADC_Channel, PWM0_CH7, TCC0_CH7, EXTERNAL_INT_13 }, // EIC/EXTINT[13] TCC2/WO[1] *TCC0/WO[7]

  // 42 (AREF)
  { PORTA, 3, PIO_ANALOG, PIN_ATTR_ANALOG, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // DAC/VREFP

  // ----------------------
  // 43..45 - Alternate use of A0 (DAC output), 44 SWCLK, 45, SWDIO
  { PORTA,  2, PIO_ANALOG, PIN_ATTR_ANALOG, DAC_Channel0, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_2 }, // DAC/VOUT
  // TODO: NOT_ON_TIMER?
  { PORTA, 30, PIO_PWM, PIN_ATTR_DIGITAL|PIO_SERCOM, No_ADC_Channel, NOT_ON_PWM, TCC1_CH0, EXTERNAL_INT_10 }, // SWCLK
  { PORTA, 31, PIO_PWM, PIN_ATTR_DIGITAL|PIO_SERCOM, No_ADC_Channel, NOT_ON_PWM, TCC1_CH1, EXTERNAL_INT_11 }, // SWDIO
} ;

const void* g_apTCInstances[TCC_INST_NUM+TC_INST_NUM]={ TCC0, TCC1, TCC2, TC3, TC4, TC5 } ;

// Multi-serial objects instantiation
SERCOM sercom0( SERCOM0 ) ;
SERCOM sercom1( SERCOM1 ) ;
SERCOM sercom2( SERCOM2 ) ;
SERCOM sercom3( SERCOM3 ) ;
SERCOM sercom4( SERCOM4 ) ;
SERCOM sercom5( SERCOM5 ) ;

Uart Serial1( &sercom0, PIN_SERIAL1_RX, PIN_SERIAL1_TX, PAD_SERIAL1_RX, PAD_SERIAL1_TX ) ;
Uart Serial( &sercom5, PIN_SERIAL_RX, PIN_SERIAL_TX, PAD_SERIAL_RX, PAD_SERIAL_TX ) ;
void SERCOM0_Handler()
{
  Serial1.IrqHandler();
}

void SERCOM5_Handler()
{
  Serial.IrqHandler();
}
