/**
 * Copyright 2026 Prakarn Jaroonsorn <prakarn.jrs@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef BPL_CONTROLLER__JOYSTICK_BLUEPRINT_CONTROL_HPP_
#define BPL_CONTROLLER__JOYSTICK_BLUEPRINT_CONTROL_HPP_

#include <tf2_eigen/tf2_eigen.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2_ros/static_transform_broadcaster.h>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_ros/transform_listener.h>

#include <pj_moveit2_msgs/msg/spherical_coord.hpp>
#include <pj_moveit2_msgs/srv/frame_goal.hpp>
#include <pj_moveit2_msgs/srv/pre_origin.hpp>
#include <pj_moveit2_msgs/srv/spherical_goal.hpp>
#include <chrono>
#include <control_msgs/msg/joint_jog.hpp>
#include <control_msgs/msg/joint_trajectory_controller_state.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joy.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <thread>
#include <trajectory_msgs/msg/joint_trajectory.hpp>

#include "tf2_ros/create_timer_ros.h"

// We'll just set up parameters here
const std::string JOY_TOPIC = "/joy";
const std::string TWIST_TOPIC = "/servo_node/delta_twist_cmds";
const std::string JOINT_TOPIC = "/servo_node/delta_joint_cmds";
const std::string GRIP_PUB_TOPIC = "/bravo_7_hand_controller/joint_trajectory";
const std::string GRIP_STATE_TOPIC = "/bravo_7_hand_controller/state";
const std::string TORSO_TOPIC = "/torso_goal";
const size_t ROS_QUEUE_SIZE = 10;
const std::string ARM_BASE_FRAME_ID = "bravo_tcp";
const std::string ROBOT_BASE_FRAME_ID = "base_link";
const std::string GRIPPER_JOINT_NAME_ID = "bravo_axis_a";

// Enums for button names -> axis/button array index
// For XBOX 1 controller
enum Axis {
  LEFT_STICK_X = 0,
  LEFT_STICK_Y = 1,
  LEFT_TRIGGER = 2,
  RIGHT_STICK_X = 3,
  RIGHT_STICK_Y = 4,
  RIGHT_TRIGGER = 5,
  D_PAD_X = 6,
  D_PAD_Y = 7
};
enum Button {
  A = 0,
  B = 1,
  X = 2,
  Y = 3,
  LB = 4,
  RB = 5,
  BACK = 6,
  START = 7,
  HOME = 8,
  LEFT_STICK_CLICK = 9,
  RIGHT_STICK_CLICK = 10
};

// Some axes have offsets (e.g. the default trigger position is 1.0 not 0)
// This will map the default values for the axes
std::map<Axis, double> AXIS_DEFAULTS = {{LEFT_TRIGGER, 1.0},
                                        {RIGHT_TRIGGER, 1.0}};
std::map<Button, double> BUTTON_DEFAULTS;

bool convert_joy_to_cmd(
    const std::vector<float> &axes, const std::vector<int32_t> &buttons,
    std::unique_ptr<geometry_msgs::msg::TwistStamped> &twist,
    std::unique_ptr<control_msgs::msg::JointJog> &joint);

namespace moveit_servo {

class BplJoyToServoPub : public rclcpp::Node {
public:
  BplJoyToServoPub(const rclcpp::NodeOptions &options);
  void send_service(const std::vector<int32_t> &buttons,
                    const std::vector<float> &axes);
  void select_mode(const std::vector<int32_t> &buttons,
                   const std::vector<float> &axes);
  void valve_control_mode(const std::vector<int32_t> &buttons,
                          const std::vector<float> &axes);
  void joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg);
  void grip_state_callback(
      const control_msgs::msg::JointTrajectoryControllerState::SharedPtr msg);
  pj_moveit2_msgs::msg::SphericalCoord
  rectan_to_sphere_feedback(geometry_msgs::msg::Pose pose);
  geometry_msgs::msg::Pose
  sphere_to_rectan(pj_moveit2_msgs::msg::SphericalCoord sph);
  void valve_tf(std::string valve_frame);
  void pre_origin();

private:
  void init_publisher();
  void init_subscribers();
  void init_service();

private:
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_sub_;
  rclcpp::Subscription<control_msgs::msg::JointTrajectoryControllerState>::
      SharedPtr grip_state_sub_;

  rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr twist_pub_;
  rclcpp::Publisher<control_msgs::msg::JointJog>::SharedPtr joint_pub_;
  rclcpp::Publisher<trajectory_msgs::msg::JointTrajectory>::SharedPtr
      gripper_pub_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr tor_pub_;

  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr servo_start_client_;
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr servo_stop_client_;
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr neutral_client_;
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr home_client_;
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr preside_client_;
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr execute_client_;
  rclcpp::Client<pj_moveit2_msgs::srv::PreOrigin>::SharedPtr
      pre_origin_client_;
  rclcpp::Client<pj_moveit2_msgs::srv::FrameGoal>::SharedPtr
      frame_goal_client_;
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr remove_tf_client_;
  rclcpp::Client<pj_moveit2_msgs::srv::SphericalGoal>::SharedPtr arc_client_;

  std::shared_ptr<rclcpp::Node> node;
  std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
  std::shared_ptr<tf2_ros::TransformListener> transform_listener_{nullptr};

  geometry_msgs::msg::TransformStamped valve_tran_;
  geometry_msgs::msg::TransformStamped current_tran_;
  geometry_msgs::msg::Pose target_;
  geometry_msgs::msg::Pose valve_pose_;
  pj_moveit2_msgs::msg::SphericalCoord sphere;
  trajectory_msgs::msg::JointTrajectory traj_;
  std_msgs::msg::Float64 grip_;

  std_msgs::msg::Float64 tor_data;

  bool send_start;
  bool send_back;
  bool send_home;
  bool send_neutral;
  bool send_preside;
  bool send_execute;
  bool send_close;
  bool send_open;
  bool send_hold;
  bool send_release;
  bool send_mode;
  bool send_origin;
  bool send_pframe;
  bool send_frame;
  bool send_gpframe;
  bool send_gframe;
  bool send_remove;
  bool send_tick;

  bool mode_operate;

  std::string frame_to_publish_;
  std::string base_frame_;
  std::string valve_name_;

}; // class BplJoyToServoPub

} // namespace moveit_servo

// Register the component with class_loader
#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(moveit_servo::BplJoyToServoPub)
#endif /* BPL_CONTROLLER__JOYSTICK_BLUEPRINT_CONTROL_HPP_ */
