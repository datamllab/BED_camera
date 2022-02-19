/*******************************************************************************
* Copyright (C) Maxim Integrated Products, Inc., All rights Reserved.
*
* This software is protected by copyright laws of the United States and
* of foreign countries. This material may also be protected by patent laws
* and technology transfer regulations of the United States and of foreign
* countries. This software is furnished under a license agreement and/or a
* nondisclosure agreement and may only be used or reproduced in accordance
* with the terms of those agreements. Dissemination of this information to
* any party or parties not specified in the license agreement and/or
* nondisclosure agreement is expressly prohibited.
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************
*/

#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include "uart.h"

typedef int32_t q31_t;
typedef int16_t q15_t;

/*****************************     MACROS    *********************************/
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })


/*****************************     VARIABLES *********************************/


/*****************************     FUNCTIONS *********************************/

uint32_t utils_get_time_ms(void);

void utils_send_bytes(mxc_uart_regs_t *uart, uint8_t *ptr, int length);

q31_t q_div(q31_t a, q31_t b);

q31_t q_mul(q31_t a, q31_t b);

q31_t sigmoid(q31_t in);

void inline_softmax_q17p14_q15(q31_t * vec_in, const uint16_t start, const uint16_t end);

void NMS_max1(q31_t * vec_in, const uint16_t dim_vec, q31_t* max_box); //Z:
void NMS_max(q31_t * vec_in, const uint16_t dim_vec, q31_t* max_box);
int cus_cnn_unload(uint32_t *out_buf);
#endif // _UTILS_H_
