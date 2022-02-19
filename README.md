# BED: A Real-Time Object Detection System for Edge Devices
<img width="450" height="200" src="https://github.com/datamllab/BED_camera/blob/main/figures/BED_logo.png">


### About this project

This project focuses on end-to-end oBject
detection system for Edge Devices (BED).
BED integrates a deep nerual network (DNN) practiced on [MAX78000](https://www.maximintegrated.com/en/products/microcontrollers/MAX78000.html) with I/O devices, as illustrated in the following figure. 
The DNN model for the detection is deployed on MAX78000; 
and the I/O devices include a camera and a screen for image acquisition and output exhibition, respectively. 

<div align=center>
<img width="450" height="200" src="https://github.com/datamllab/BED_GUI/blob/main/figures/GUI_pipeline.png">
</div>

### Camera Demo ai8x Environment Installation

Before running the camera demo code for real-time object detection, it is necessary to clone and install the software of [MAX78000 Evaluation Kit](https://github.com/MaximIntegratedAI/MaximAI_Documentation/tree/master/MAX78000_Evaluation_Kit).
Once finishing the installation, copy this repo to the root directory of "MAX78000_SDK/Examples/MAX78000/CNN/", and use this command (on Windows use the MinGW Shell) to deploy the code on MAX78000:

````angular2html
openocd -s /your_relative_path/MaximSDK/Tools/OpenOCD/scripts/ -f interface/cmsis-dap.cfg -f target/max78000.cfg -c "program build/MAX78000.elf reset exit"
````

### Real-time Object Detection

Switch on the MAX78000 and click on **Start Demo** on the MAX78000 screen. Point the camera in the direction of the object you want to detect. The images are captured using the camera and the object detection results are displayed in real-time on the screen attached to the MAX78000 board. Here is the experimental setup, named as the **testing bed**, that we used for real-time detection.

<div align=center>
<img width="250" height="200" src="https://github.com/datamllab/BED_camera/blob/main/figures/testing_bed.png">
</div>

Once you have setup the testing bed, you can see the detection results in real-time on the MAX78000 screen, including the inference time and the detected class label, as follows: 

<div align=center>
<img width="250" height="200" src="https://github.com/datamllab/BED_GUI/blob/main/figures/real_results_more.png">
</div>
