# ROS2-TMS-FOR-CONSTRUCTION

ROS2-TMS-FOR-CONSTRUCTION is an application of ROS2-TMS to construction.

## ROS2-TMS

ROS2-TMS is an IoRT (Internet of Robotic Things) library for TMS (Town Management System), which is the management system of an informationally structured environment (ISE). ROS2-TMS is newly constructed on the basis of [ROS-TMS](https://github.com/irvs/ros_tms/wiki) and adopts the state-of-the-art robot middleware, ROS2. This system integrates various information gathered by distributed sensors, stores them to an on-line database, plans proper service tasks, and manages and executes robot motion.

Wiki page : [https://github.com/irvs/ros2_tms/wiki/ROS2-TMS](https://github.com/irvs/ros2_tms/wiki/ROS2-TMS)

ROS2-TMS is being developed as a part of "**MyIoT Store**" in "**MyIoT Project**" supported by the Cabinet Office (CAO), **Cross-ministerial Strategic Innovation Promotion Program (SIP)**, “An intelligent knowledge processing infrastructure, integrating physical and virtual domains” (funding agency: NEDO).

## Install

- ROS2 Foxy: https://index.ros.org/doc/ros2/Installation/Foxy/
- Git: https://git-scm.com/book/en/v2/Getting-Started-Installing-Git
- mongodb: https://www.mongodb.com/docs/manual/tutorial/install-mongodb-on-ubuntu/
- related packages for ROS2-TMS-FOR-CONSTRUCTION
    - pymongo 4.1.1
    - open3d
    - numpy

    ```
    python3 -m pip install pymongo==4.1.1 open3d numpy
    ```


## Setup

### Clone

```
mkdir -p ~/ros2-tms-for-constructoin_ws/src
cd ~/ros2-tms-for-constructoin_ws/src/
git clone　https://github.com/irvs/ros2_tms_for_construction.git
```

### Build

```
cd ~/ros2-tms-for-constructoin_ws
colcon build
```

## Version Information

* Date : 2022.8.14 (since 2022.8.14 ROS2-TMS-FOR-CONSTRUCTION / since 2019.2.14 ROS2-TMS / since 2012.5.1 ROS-TMS / since 2005.11.1 TMS)  
  
* Url : https://index.ros.org/doc/ros2/Installation/Foxy/     

* Ubuntu 20.04 LTS 64BIT  

* ROS2 Foxy Fitzroy  

* mongodb 4.0  

* pymongo 4.1.1  

* numpy

* open3d
