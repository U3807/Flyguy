# Flyguy - Programmable UAV   
## Introduction  
We hope to equip our UAV with the WE-I PLUS development board which includes a camera and AI processing capabilities. Our goal is to design an intelligent UAV system that can perform object detection and respond in real time. Using ARC iotdk board as the main control module for peripheral sensors and motors. Enabling UAV attitude or showing captured image of person on local PC monitor. 

To enable common features such as hovering, landing, emergency shut down, we will use the ARC iotdk board as the main controller to control peripheral components. The Himax camera on the WE-I development board can capture images of the UAV’s surroundings and feed them into a CNN model, enabling backend AI applications. 

We leverage this exceptional advantages to implement an intelligent UAV, which can be used to perform gestures classification and person detection. Once detect the gestures, the corresponding command is send to main controller and modify UAV attitude. Likewise, if the person is being detected, image of he/she will be transmitted to PC by radio links.   

## Demo Video
* <https://youtu.be/GtNfwzQPovE>
## System Architecture
![image](https://github.com/U3807/FLyguy/blob/main/Pics/system_architecture.png) 
## CNN Model Training Flowchart
![image](https://github.com/U3807/FLyguy/blob/main/Pics/train_cnn.png) 
## Required Hardware
* Main controller : ARC iotdk board
* Object detection board : WE-I Plus
* RC controller : AT9S 
* Radiolink : R9DS
* Ultrasonic sensor : HC-SR04
* Quad frame : QAV250
* Electronic Speed Control: BLHeli-32
* Motor : Velox V2306
## Required Software
* <https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_osp/tree/embarc_mli>
* <https://github.com/HimaxWiseEyePlus/himax_tflm#training-your-own-model>
* Download source code from the link above
* Put our folder Gesture_detection and folder Person_detection_photo_take in ``` himax_tflm\Synopsys_WEI\User_Project\```
## User Manual 
### Gesture data collection and training
* Collect pictures from himax camera for machine learning datasets in  ``` himax_tflm\Synopsys_WEI\User_Project\Camera```
* Press B on keyboard and the pixel value of taken picture will show on the teraterm terminal
* Save those printed value in logfiles, each class should be saved as  ```Python\Gesture_detection\teradata\xxx\xxx.txt```   xxx = *classname*
* Start training in ```Python\Gesture_detection``` and burn the tensorflow_lite model in the himax board   
* Recommend using Jupyter Notebook execute Gesture_himax_int8.IPYNB , or you can still directly execute Gesture_himax_int8.py   
* Press reset button to inference on Himax WE-I Board and the predicted direction will show on teraterm terminal  
![image](https://github.com/U3807/FLyguy/blob/main/Pics/predicted.PNG) 
### Person detection and phototaking
* Setup the radio connection
* Run the code  ```Python\Radio_link\radio_link.py``` to receive UART data
* Modify Radio control com_port setting ```COM_PORT = 'COMxxx' ```
* Compile the code in ```Python\Person_detection_photo_take ``` and burn the imgae file in Himax WE-I board.
* If Himax WE-I borad detect a person's face, the person's picture will be send through UART and displayed on screen.  
![image](https://github.com/U3807/FLyguy/blob/main/Pics/gary.png) 
* The image size we send is 60x80. It needs a long time to send a higher resolution picture since the UART transmission rate is constrained by our flight control system. We will try to solve this problem and upload a new version.
*
## Combine
We combine the above models in one project, users can download the source code in ```Himax\combine```.
