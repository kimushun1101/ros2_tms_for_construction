# ROS2-TMS-FOR-CONSTRUCTION

ROS2-TMS-FOR-CONSTRUCTION is an IoRT library for construction applications based on ROS2-TMS.

## ROS2-TMS

ROS2-TMS is an IoRT (Internet of Robotic Things) library for TMS (Town Management System), which is the management system of an informationally structured environment (ISE). ROS2-TMS is newly constructed on the basis of [ROS-TMS](https://github.com/irvs/ros_tms/wiki) and adopts the state-of-the-art robot middleware, ROS2. This system integrates various information gathered by distributed sensors, stores them to an on-line database, plans proper service tasks, and manages and executes robot motion.

Wiki page : [https://github.com/irvs/ros2_tms/wiki/ROS2-TMS](https://github.com/irvs/ros2_tms/wiki/ROS2-TMS)

ROS2-TMS is being developed as a part of "**MyIoT Store**" in "**MyIoT Project**" supported by the Cabinet Office (CAO), **Cross-ministerial Strategic Innovation Promotion Program (SIP)**, “An intelligent knowledge processing infrastructure, integrating physical and virtual domains” (funding agency: NEDO).

## ROS2-TMS-FOR-CONSTRUCTION

ROS2-TMS-FOR-CONSTRUCTION is developed as an IoRT library for construction applications based on ROS2-TMS with the support of JST Moonshot R&D, Grant Number JPMJPS2032 entitled “Collaborative AI robots for adaptation of diverse environments and innovation of infrastructure construction” in “Moonshot Goal 3: Realization of Artificial Intelligence (AI) robots that autonomously learn, adapt to their environment, evolve itself in intelligence, and act alongside human beings, by 2050.”

Project page: [https://moonshot-cafe-project.org/en/](https://moonshot-cafe-project.org/en/)

## Install

### ROS2 Foxy

https://index.ros.org/doc/ros2/Installation/Foxy/

### Git

https://git-scm.com/book/en/v2/Getting-Started-Installing-Git

### MongoDB

https://www.mongodb.com/docs/manual/tutorial/install-mongodb-on-ubuntu/

### Related packages for ROS2-TMS-FOR-CONSTRUCTION
- pymongo 4.1.1
- open3d 0.13.0
- numpy 1.22.3

```
python3 -m pip install pymongo==4.1.1 open3d==0.13.0 numpy==1.22.3
```


## Setup

### Create a workspace

```
mkdir -p ~/ros2-tms-for-constructoin_ws/src
```

### Clone this repository

```
cd ~/ros2-tms-for-constructoin_ws/src/
git clone　https://github.com/irvs/ros2_tms_for_construction.git
```

### Build the workspace

```
cd ~/ros2-tms-for-constructoin_ws
colcon build
```

## How to use

ROS2-TMS-FOR-CONSTRUCTION has the following packages. You can see detail descriptions from the links.

### tms_db

-  [tms_db_manager](https://github.com/irvs/ros2_tms_for_construction/tree/main/tms_db/tms_db_manager)

   ROS2-TMS database manager. This package has tms_db_reader(_gridfs) and tms_db_writer(_gridfs) nodes.

### tms_mc

- [tms_mc_odom](https://github.com/irvs/ros2_tms_for_construction/tree/main/tms_mc/tms_mc_odom)

  tms_mc_odom is a package for formatting Odometry msg to Tmsdb msg and sending it to tms_db_writer.

  The received Odometry msg is the location data of the construction machine.

### tms_sd

- [tms_sd_ground](https://github.com/irvs/ros2_tms_for_construction/tree/main/tms_sd/tms_sd_ground)

  tms_sd_ground is a package for formatting OccupancyGrid msg to Tmsdb msg and sending it to tms_db_writer.

  The received OccupancyGrid msg is a heatmap showing the hardness of the ground.

- [tms_sd_terrain](https://github.com/irvs/ros2_tms_for_construction/tree/main/tms_sd/tms_sd_terrain)

  tms_sd_terrain is a package for converting PointCloud2 msg to .pcd file and sending the file info to tms_db_writer_gridfs.

  The received PointCloud2 msg is a point cloud data of terrain.

### tms_ur

- [tms_ur_construction](https://github.com/irvs/ros2_tms_for_construction/tree/main/tms_ur/tms_ur_construction)

  tms_ur_construction is a package for getting construction data (ex. machine's location, terrain, hardness of ground) from tms_db_reader(_gridfs) and publishing them.

## Version Information

* Date : 2022.8.19 (since 2022.8.19 ROS2-TMS-FOR-CONSTRUCTION / since 2019.2.14 ROS2-TMS / since 2012.5.1 ROS-TMS / since 2005.11.1 TMS)  
  
* Url : https://index.ros.org/doc/ros2/Installation/Foxy/     

* Ubuntu 20.04 LTS 64BIT  

* ROS2 Foxy Fitzroy  

* mongodb 5.0  

* pymongo 4.1.1  

* open3d 0.13.0

* numpy 1.22.3
