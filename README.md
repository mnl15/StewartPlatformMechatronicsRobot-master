# StewartPlatformMechatronicsRobot-master

### Work Done Under Michelle Lopez, Penn Design
#### Authors: Sanjana Rao, Sydney Miller, Mona Lee

Heavily references code by Michelle Rosa (https://github.com/michaeljrosa/stewart-platform) and Leroy Sibanda

Our summer project involved working on a robotics-based component of a larger sculpturing project under Professor Michelle Lopez at Penn Design. The final installation will include a digital video animation and a dance choreography for robot set to a musical composition.  

The goal for the summer was to write Arduino code to improve the nuanced motions and dexterity of a Stuart Platform robot to be used in the musical choreography. The pre-existing program utilized an XYZ-coordinate system allowed the actuators to move to specified points but would stop the robot rigidly at each target location. Starting with this basic system, we aimed to modify the motions of the robot to a fluid motion that could run concurrently with a soundtrack.

## DEVELOPMENT

This program was built in Arduino, C++. Mona was responsible for understanding the physical requirements and limitations of the robot and Sydney and Sanjana were responsible for writing the code to achieve the desired motion.

The working model involved redesigning the pre-existing code architecture and rebuilding the program to incorporate fluid motion at every level of the design. In the end, we were able to achieve a fluid motion by controlling the extend and retract of the actuators individually in relationship to time, rather than having the actuators move in unison to a specific point. We set the speed of the actuators to increase and decrease with parabolic equations. This method eliminates the stopping of the robot when it reached a point, allowing for a fluid motion.

In the final model, the Robot class prescribes the desired motion for the robot and initializes a Platform object. The Platform object consists of six Actuator instances. The physics based calculations occur at the actuator level. In order to achieve a visually fluid motion as the actuators move through a series of points, the speed of each actuator is controlled by a parabolic equation.

Video of Final Motion: https://www.youtube.com/watch?v=iQmkbgB0hc0

Video of Motion Time Lapse: https://www.youtube.com/watch?v=SVgW7NmqUDE

** Final work from the end of the summer of 2019 can be found at https://github.com/sanjanaxrao/StewartPlatformMechatronicsRobot, this file has been updated by Mona to calibrate initial platform height and with comments to transfer to new interns working on the project. 

## NEXT STEPS

Immediate next steps include creating a series of  black-boxed “dance moves”  for the robot so that a user unfamiliar with the Arduino code can utilize the robot. This will be useful in the larger scope of the project, specifically when creating a choreography for the robot in relation to the musical composition.
