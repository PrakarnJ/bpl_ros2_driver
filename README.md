# bpl_ros2_driver

> **Supported ROS2 distribution:** Jazzy

## Packages in the repository
- `bpl_robot` - Meta-package that provides a single point of installation for the released packages.
- `bravo_7_bringup` - Launch file and run-time configurations, e.g. controllers (DEPRECATED).
- `bpl_protocol` - Reach communication protocol via udp socket.
- `bpl_controllers` - Joystick input controller for MoveIt2 Servo teleoperation.
- `bravo_7_moveit_config` - Moveit2 configuration for bravo_7 robot.
- `bpl_robot_driver` - driver / hardware interface for communication with bravo_7 robots.
- `bpl_ros2_control_test_nodes` - Test bravo_7 hardware interface node.

## Getting Started
1. Create a new ROS2 workspace:
   ```
   mkdir -p bpl_ws/src
   ```
2. Pull relevant packages, install dependencies, compile, and source the workspace by using:
   ```
   cd bpl_ws/src
   git clone git@github.com:PrakarnJ/bpl_ros2_driver.git
   chmod +x bpl_ros2_driver/install.sh
   ./bpl_ros2_driver/install.sh
   vcs import < bpl_ros2_driver/bpl_ros2_driver.repos
   cd ..
   rosdep install --from-paths src --ignore-src -r -y 
   colcon build --symlink-install
   source install/setup.bash
   ```

## Network Setup
This section describes a good example using static IP addresses and a direct connection from the PC to the Robot to minimize latency introduced by network hardware. Though a good network switch usually works fine, as well.
1. Connect the bravo_7 control box directly to the remote PC with an ethernet cable.
2. On the remote PC, turn off all network devices except the "wired connection", e.g. turn off wifi.
3. Open Network Settings and create a new Wired connection with these settings. You may want to name this new connection `bravo_7` or something similar:
    ```
    IPv4
    Manual
    Address: 192.168.2.100
    Netmask: 255.255.255.0
    Gateway: 192.168.2.1
    ```
4. Verify the connection from the PC with e.g. ping.
    ```
    ping 192.168.2.3
    ```
## Usage
For starting the driver there are main launch files in the `bravo_7_bringup` package.
- `bpl_robot.launch.py` - starts ros2_control node including hardware interface, joint state broadcaster and a controller.

For starting the `pj_moveit2_adaptor` interface there are two main launch files in the `bravo_7_moveit_config` package.
- `bravo_7_move_group.launch.py` - starts moveit2 node including move_group interface, and pj_moveit2_adaptor_node.
- `bravo_7_robot_visualization.launch.py` - starts rviz2.

For joystick-based servo teleoperation there are launch files in the `bravo_7_moveit_config` package.
- `bravo_7_servo_teleop.launch.py` - starts MoveIt2 Servo with joystick input (hardware).
- `bravo_7_servo_teleop_gazebo.launch.py` - starts MoveIt2 Servo with joystick input (simulation).
- `bravo_7_fake_servo_teleop.launch.py` - starts MoveIt2 Servo with joystick input (fake controller).

## Command for testing the driver
##### 1. Start hardware, simulator or mockup
- To do test with hardware, use:
  ```
  ros2 launch bravo_7_bringup bravo_7_robot_bringup.launch.py
  ```
- To do test with gazebo, use:
  ```
  ros2 launch bravo_7_gazebo bravo_7_gazebo.launch.py
  ```
- To do test with fake controller, use:
  ```
  ros2 launch bravo_7_moveit_config bravo_7_fake_controller.launch.py
  ```
##### 2. Start pj_moveit2_adaptor interface
- To do test with hardware, use:
  ```
  ros2 launch bravo_7_moveit_config bravo_7_move_group.launch.py
  ```
- To do test with gazebo, use:
  ```
  ros2 launch bravo_7_moveit_config bravo_7_move_group_gazebo.launch.py
  ```
- To do test with fake controller, use:
  ```
  ros2 launch bravo_7_moveit_config bravo_7_fake_move_group.launch.py
  ```
##### 3. Start robot visualization
- To do test with hardware, use:
  ```
  ros2 launch bravo_7_moveit_config bravo_7_robot_visualization.launch.py
  ```
- To do test with gazebo, use:
  ```
  ros2 launch bravo_7_moveit_config bravo_7_robot_visualization_gazebo.launch.py
  ```
- To do test with fake controller, use:
  ```
  ros2 launch bravo_7_moveit_config bravo_7_fake_robot_visualization.launch.py
  ```
##### 4. Switch controller (select Motion Planning or Servoing)
- To perform MoveIt Motion Planning, use:
  ```
  ros2 control switch_controllers --stop forward_position_controller --start bravo_7_position_controller
  ```
- To perform MoveIt Arm Servoing, *BUG!!!!* user need to move the analog joystick before activating "forward_position_controller" to prevent undesired joint movements (such as: go to home posture without warning due to the "Servo_server" getting old joint values). After you move the analog joystick, use:
  ```
  ros2 control switch_controllers --stop bravo_7_position_controller --start forward_position_controller
  ```
##### 5. Start servo teleoperation (joystick)
- To do test with hardware, use:
  ```
  ros2 launch bravo_7_moveit_config bravo_7_servo_teleop.launch.py
  ```
- To do test with gazebo, use:
  ```
  ros2 launch bravo_7_moveit_config bravo_7_servo_teleop_gazebo.launch.py
  ```
- To do test with fake controller, use:
  ```
  ros2 launch bravo_7_moveit_config bravo_7_fake_servo_teleop.launch.py
  ```
