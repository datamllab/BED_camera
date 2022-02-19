/*******************************************************************************
* Copyright (C) Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
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
*
******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "mxc_device.h"
#include "board.h"
#include "mxc_delay.h"
#include "uart.h"
#include "rtc.h"
#include "utils.h"
#include "faceID.h"
// #include "cnn.h"

#define CNN_OK 1
/***************************** VARIABLES *************************************/


/************************    PUBLIC FUNCTIONS  *******************************/

uint32_t utils_get_time_ms(void)
{
    int sec;
    double subsec;
    uint32_t ms;

    subsec = MXC_RTC_GetSubSecond() / 4096.0;
    sec = MXC_RTC_GetSecond();

    ms = (sec*1000) +  (int)(subsec*1000);

    return ms;
}

static void utils_send_byte(mxc_uart_regs_t *uart, uint8_t value) {
	while (MXC_UART_WriteCharacter(uart, value) == E_OVERFLOW) { }
}

void utils_send_bytes(mxc_uart_regs_t *uart, uint8_t *ptr, int length) {
	int i;

	for (i = 0; i < length; i++) {
		utils_send_byte(uart, ptr[i]);
	}
}

void inline_softmax_q17p14_q15(q31_t * vec_in, const uint16_t start, const uint16_t end)
{
    q31_t     sum;
    int16_t   i;
    uint8_t   shift;
    q31_t     base;
    base = -1 * 0x80000000;

    for (i = start; i < end; i++)
    {
        if (vec_in[i] > base)
        {
            base = vec_in[i];
        }
    }

    /* we ignore really small values
     * anyway, they will be 0 after shrinking
     * to q15_t
     */

    base = base - (16<<14);

    sum = 0;

    for (i = start; i < end; i++)
    {
        if (vec_in[i] > base)
        {
            shift = (uint8_t)((8192 + vec_in[i] - base) >> 14);
            sum += (0x1 << shift);
        }
    }


    /* This is effectively (0x1 << 32) / sum */
    int64_t div_base = 0x100000000LL;
    int32_t output_base = (int32_t)(div_base / sum);
    int32_t out;

    /* Final confidence will be output_base >> ( 17 - (vec_in[i] - base)>>14 )
     * so 32768 (0x1<<15) -> 100% confidence when sum = 0x1 << 16, output_base = 0x1 << 16
     * and vec_in[i]-base = 16
     */

    for (i = start; i < end; i++)
    {
        if (vec_in[i] > base)
        {
            /* Here minimum value of 17+base-vec[i] will be 1 */
            shift = (uint8_t)(17+((8191 + base - vec_in[i]) >> 14));

            out = (output_base >> shift);

            if (out > 32767)
                out = 32767;

            vec_in[i] = out;


        } else
        {
            vec_in[i] = 0;
        }
    }

}

uint16_t argmax_softmax(q31_t * vec_in, const uint16_t start)
{
    q31_t cls_score = 0;
    uint16_t idx = 0;
    uint16_t i;
    for (i = start; i < start + NUM_CLASSES; ++i) {
        if (vec_in[i] > cls_score) {
            idx = i;
            cls_score = vec_in[i];
        }
        vec_in[i] = sigmoid(vec_in[i]);

    }
//    inline_softmax_q17p14_q15(vec_in, start, start + NUM_CLASSES);
    return idx;
}









void NMS_max1(q31_t * vec_in, const uint16_t dim_vec, q31_t* max_box)
{
    // x1, y1, x2, y2, box_score, cls_score, cls
    // max_box[7] = {0};
    q31_t confident_threshold = -16300;  // 0.55   9011  max 16384

    uint16_t cls_idx, max_i = 0;
    uint16_t i, b;
    uint16_t m, n;

    // uint16_t b_max;

    q31_t pos[4] = {0};

    q31_t gridX, gridY;
    q31_t centerX, centerY, width, height;
    q31_t tmp;
    q31_t tmp2=10;
    max_box[4] = confident_threshold;

    for (i = 0; i < dim_vec; i += NUM_CHANNELS) {
        for (b = 0; b < NUM_BOXES; ++b) {
            // tmp = sigmoid(vec_in[i + 5 * b + 4]);
            tmp = vec_in[i + 5 * b + 4];

            if (tmp > max_box[4])
            {
              m = i / (NUM_GRIDS * NUM_CHANNELS); // row
              n = i / NUM_CHANNELS % NUM_GRIDS; //column
              gridX = GRID_SIZE * m;
              gridY = GRID_SIZE * n;
              pos[0] = vec_in[i + 5 * b];
              pos[1] = vec_in[i + 5 * b + 1];
              pos[2] = vec_in[i + 5 * b + 2];
              pos[3] = vec_in[i + 5 * b + 3];
              centerX = gridX + q_mul(sigmoid(pos[0]), GRID_SIZE);
              centerY = gridY + q_mul(sigmoid(pos[1]), GRID_SIZE);
              width = q_mul(sigmoid(pos[2]), IMG_SIZE); //Z: q_mul to map to the grid/image size i.e, the display
              height = q_mul(sigmoid(pos[3]), IMG_SIZE);

              if ((centerX - (width >> 1) > tmp2) && (centerY - (height >> 1) > tmp2))
              {
                // b_max = b;
                max_i = i;

                max_box[0] = vec_in[i + 5 * b];
                max_box[1] = vec_in[i + 5 * b + 1];
                max_box[2] = vec_in[i + 5 * b + 2];
                max_box[3] = vec_in[i + 5 * b + 3];
                max_box[4] = tmp;
              }
            }
        }
    }
    for (int j = 0; j < NUM_CLASSES; ++j) {
        max_box[5 + j] = vec_in[max_i + BOX_DIMENSION + j];
    }

    //Added these 3 lines for maximum confidence class calculate and commented some linnes below for 10,11 max box
    // for (int i = 0; i < MAX_BOX_SIZE; ++i) {
    //     printf("max_box_before[%d] = %d \n", i, max_box[i]);
    // }
    cls_idx = argmax_softmax(max_box, 5);
    max_box[MAX_BOX_SIZE - 2] = max_box[cls_idx];
    max_box[MAX_BOX_SIZE - 1] = cls_idx - 5;

    // q31_t cls_sum = 0;
    // for (i = 5; i < 5 + NUM_CLASSES; ++i) {
    //     cls_sum += max_box[i];
    // }

    // max_box[10] = max_i / NUM_CHANNELS;

    // max_box[5] = max_box[cls_idx];
    // max_box[7] = cls_sum;           //q_div(max_box[cls_idx], cls_sum);
    // max_box[11] = time_taken; 

    m = max_i / (NUM_GRIDS * NUM_CHANNELS); // row
    n = max_i / NUM_CHANNELS % NUM_GRIDS; //column

    gridX = GRID_SIZE * m;
    gridY = GRID_SIZE * n;

    // max_box[0] = m;
    // max_box[1] = n;
    // max_box[2] = b_max;

    centerX = gridX + q_mul(sigmoid(max_box[0]), GRID_SIZE);
    centerY = gridY + q_mul(sigmoid(max_box[1]), GRID_SIZE);
    width = q_mul(sigmoid(max_box[2]), IMG_SIZE);
    height = q_mul(sigmoid(max_box[3]), IMG_SIZE);

    max_box[0] = max(0, (centerX - (width >> 1)));
    max_box[1] = max(0, (centerY - (height >> 1)));
    max_box[2] = min(IMG_SIZE - 1, (centerX + (width >> 1)));
    max_box[3] = min(IMG_SIZE - 1, (centerY + (height >> 1)));
}


























void NMS_max(q31_t * vec_in, const uint16_t dim_vec, q31_t* max_box)
{
    // x1, y1, x2, y2, box_score, cls_score, cls
    // max_box[7] = {0};
    q31_t confident_threshold =  9011;  // 0.55

    uint16_t cls_idx, max_i = 0;
    uint16_t i, b;
    uint16_t m, n;

    q31_t gridX, gridY;
    q31_t centerX, centerY, width, height;
    q31_t tmp;
    max_box[4] = confident_threshold;

    for (i = 0; i < dim_vec; i += NUM_CHANNELS) {
        for (b = 0; b < NUM_BOXES; ++b) {
            tmp = sigmoid(vec_in[i + 5 * b + 4]);
            if (tmp > max_box[4])
            {
                max_i = i;
                max_box[0] = vec_in[i + 5 * b];
                max_box[1] = vec_in[i + 5 * b + 1];
                max_box[2] = vec_in[i + 5 * b + 2];
                max_box[3] = vec_in[i + 5 * b + 3];
                max_box[4] = tmp;
            }
        }
    }
    for (int j = 0; j < NUM_CLASSES; ++j) {
        max_box[5 + j] = vec_in[max_i + BOX_DIMENSION + j];
    }
    cls_idx = argmax_softmax(max_box, 5);
    max_box[MAX_BOX_SIZE - 2] = max_box[cls_idx];
    max_box[MAX_BOX_SIZE - 1] = cls_idx - 5;

    m = max_i / (NUM_GRIDS * NUM_CHANNELS);
    n = max_i / NUM_CHANNELS % NUM_GRIDS;

    gridX = GRID_SIZE * m;
    gridY = GRID_SIZE * n;

    centerX = gridX + q_mul(sigmoid(max_box[0]), GRID_SIZE);
    centerY = gridY + q_mul(sigmoid(max_box[1]), GRID_SIZE);
    width = q_mul(sigmoid(max_box[2]), WIDTH);
    height = q_mul(sigmoid(max_box[3]), HEIGHT);

    max_box[0] = max(0, (centerX - (width >> 1)));
    max_box[1] = max(0, (centerY - (height >> 1)));
    max_box[2] = min(WIDTH - 1, (centerX + (width >> 1)));
    max_box[3] = min(HEIGHT - 1, (centerY + (height >> 1)));
}


// Custom unload for this network: 32-bit data, shape: [15, 7, 7]
int cus_cnn_unload(uint32_t *out_buf)
// custom unloading to ml_array:15x7x7
{
    // printf("We are here");
	  volatile uint32_t *addr;
    // volatile uint32_t *addr2;
    // addr2 = out_buf;
    
    // uint32_t temp;
	  uint32_t i,j,k;


    static int32_t ml_array[15][7][7];
	  // channels 0-3
	  addr = (volatile uint32_t *) 0x50400000;
	  for (i = 0; i<7; i++)
		  for (j=0;j<7; j++)
			  for(k=0; k<4; k++)
				  ml_array[k][i][j] = *addr++;

	  // channels 4-7
	  addr = (volatile uint32_t *) 0x50408000;
	  for (i = 0; i<7; i++)
		  for (j=0;j<7; j++)
			  for(k=0; k<4; k++)
				  ml_array[k + 4][i][j] = *addr++;

	  // channels 8-11
	  addr = (volatile uint32_t *) 0x50410000;
	  for (i = 0; i<7; i++)
		  for (j=0;j<7; j++)
			  for(k=0; k<4; k++)
				  ml_array[k + 8][i][j] = *addr++;

	  // channels 12-14
	  addr = (volatile uint32_t *) 0x50418000;
	  for (i = 0; i<7; i++)
		  for (j=0;j<7; j++)
		  {
			  for(k=0; k<3; k++)
				  ml_array[k + 12][i][j] = *addr++;

			  *addr++; //skip channel 15
		  }
    
    /// asign 
	  for (i = 0; i<7; i++)
		  for (j=0;j<7; j++)
		  {
			  for(k=0; k<15; k++)
        *out_buf++ = ml_array[k][i][j];
				  // *out_buf++ = (ml_array[k][i][j] + (1<<5)) >> 6;
		  }



    // old
	  // // channels 0-3
	  // addr = (volatile uint32_t *) 0x50400000;
	  // for (i = 0; i<7; i++)
		//   for (j=0;j<7; j++)
		// 	  for(k=0; k<4; k++)
    //     {
		// 		  *out_buf++ = *addr++;
    //       // temp = (((int32_t)*addr++ + (1<<5)) >> 6);
    //       // *out_buf++ = temp; // round and convert 32 bit to similar range as offline script
    //     }

	  // // channels 4-7
	  // addr = (volatile uint32_t *) 0x50408000;
	  // for (i = 0; i<7; i++)
		//   for (j=0;j<7; j++)
		// 	  for(k=0; k<4; k++)
    //     {
		// 		  *out_buf++ = *addr++;
    //       // temp = (((int32_t)*addr++ + (1<<5)) >> 6);
    //       // *out_buf++ = temp; // round and convert 32 bit to similar range as offline script
    //     }

	  // // channels 8-11
	  // addr = (volatile uint32_t *) 0x50410000;
	  // for (i = 0; i<7; i++)
		//   for (j=0;j<7; j++)
		// 	  for(k=0; k<4; k++)
    //     {
		// 		  *out_buf++ = *addr++;
    //       // temp = (((int32_t)*addr++ + (1<<5)) >> 6);
    //       // *out_buf++ = temp; // round and convert 32 bit to similar range as offline script
    //     }

	  // // channels 12-14
	  // addr = (volatile uint32_t *) 0x50418000;
	  // for (i = 0; i<7; i++)
		//   for (j=0;j<7; j++)
		//   {
		// 	  for(k=0; k<3; k++)
    //     {
		// 		  *out_buf++ = *addr++;
    //       // temp = (((int32_t)*addr++ + (1<<5)) >> 6);
    //       // *out_buf++ = temp; // round and convert 32 bit to similar range as offline script
    //     }
		// 	  *addr++; //skip channel 15
		//   }


	  // printf("dump CNN result: \n");
	  // for (k = 0; k<15; k++)
	  // {
		//   // printf("----------------%d-----------------\n",k);
		//   for (i = 0; i<7; i++)
		//   {
		// 	  for (j=0;j<7; j++)
		// 	  {
		// 		  // *addr2 = (((int32_t)*addr2 + (1<<5)) >> 6);  // round and convert 32 bit to similar range as offline script
    //       *addr2++ = (int32_t)*addr2++;
		// 	  }
		// 	  printf("\n");
		//   }
	  // }

    return CNN_OK;
}