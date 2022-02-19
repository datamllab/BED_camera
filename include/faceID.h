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

#ifndef _FACEID_H_
#define _FACEID_H_

#define CAPTURE_X	50
#define CAPTURE_Y	290
#define	SKIP_X		60
#define SKIP_Y		290
#define RUN_X		160
#define RUN_Y		290
#define BACK_X		0
#define BACK_Y		280

#define IMAGE_XRES  224 // 200
#define IMAGE_YRES  224 // 150

#define HEIGHT		224 // 160
#define WIDTH		224 // 120
#define THICKNESS	4
#define IMAGE_H		224 // 150
#define IMAGE_W		224 // 200
#define FRAME_COLOR	0x535A
#define IMG_SIZE 224  // need to change
// Show Image from Camera
#define X_START		8
#define Y_START		30

#define BYTE_PER_PIXEL	2

// Data input: HWC (little data)
#define DATA_SIZE_IN (HEIGHT * WIDTH * 3)

#define NUM_CLASSES 5  // need to change
#define NUM_GRIDS 7
#define NUM_BOXES 2
#define BOX_DIMENSION (5 * NUM_BOXES)
#define GRID_SIZE (WIDTH / NUM_GRIDS)
#define NUM_CHANNELS (BOX_DIMENSION + NUM_CLASSES)
#define NUM_LUT_BITS 8
#define MAX_BOX_SIZE (5 + NUM_CLASSES + 2)

#endif // _FACEID_H_
