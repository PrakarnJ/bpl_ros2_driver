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
// ROS
#include <bpl_controllers/joystick_bpl_control.hpp>

bool convert_joy_to_cmd(
    const std::vector<float> &axes, const std::vector<int32_t> &buttons,
    std::unique_ptr<geometry_msgs::msg::TwistStamped> &twist,
    std::unique_ptr<std_msgs::msg::Float64> &joint) {
  // Give joint jogging priority because it is only buttons
  // If any joint jog command is requested, we are only publishing joint
  // commands
  if (buttons[A] || buttons[Y]) {
    joint->data = (buttons[Y] - buttons[A]) * 0.0001;
    return false;
  }
  // The bread and butter: map buttons to twist commands
  twist->twist.linear.x = axes[LEFT_STICK_Y];
  twist->twist.linear.y = axes[LEFT_STICK_X];

  double lin_x_right =
      -0.5 * (axes[RIGHT_TRIGGER] - AXIS_DEFAULTS.at(RIGHT_TRIGGER));
  double lin_x_left =
      0.5 * (axes[LEFT_TRIGGER] - AXIS_DEFAULTS.at(LEFT_TRIGGER));
  twist->twist.linear.z = lin_x_right + lin_x_left;

  twist->twist.angular.z = axes[RIGHT_STICK_X];
  twist->twist.angular.y = axes[RIGHT_STICK_Y];

  double roll_positive = buttons[B];
  double roll_negative = -1 * (buttons[X]);
  twist->twist.angular.x = roll_positive + roll_negative;
  return true;
}

namespace moveit_servo {

static const rclcpp::Logger LOGGER =
    rclcpp::get_logger("bpl_controllers.moveit_servo");

BplJoyToServoPub::BplJoyToServoPub(const rclcpp::NodeOptions &options)
    : Node("bpl_joy_to_twist_publisher", options),
      frame_to_publish_(ARM_BASE_FRAME_ID), base_frame_(ROBOT_BASE_FRAME_ID) {
  init_publisher();
  init_subscribers();
  init_service();

  mode_operate = false;

  node = rclcpp::Node::make_shared("bpl_joy_to_twist_publisher");
  tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
  transform_listener_ =
      std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
  // Initialize the transform broadcaster
  tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
  tor_data.data = 0.0;

  traj_.joint_names.resize(1);
  traj_.points.resize(1);
  traj_.points[0].positions.resize(1);
}

void BplJoyToServoPub::init_publisher() {
  twist_pub_ = this->create_publisher<geometry_msgs::msg::TwistStamped>(
      TWIST_TOPIC, ROS_QUEUE_SIZE);
  joint_pub_ = this->create_publisher<control_msgs::msg::JointJog>(
      JOINT_TOPIC, ROS_QUEUE_SIZE);
  gripper_pub_ = this->create_publisher<trajectory_msgs::msg::JointTrajectory>(
      GRIP_PUB_TOPIC, ROS_QUEUE_SIZE);
  tor_pub_ = this->create_publisher<std_msgs::msg::Float64>(TORSO_TOPIC,
                                                            ROS_QUEUE_SIZE);
}

void BplJoyToServoPub::init_subscribers() {
  joy_sub_ = this->create_subscription<sensor_msgs::msg::Joy>(
      JOY_TOPIC, ROS_QUEUE_SIZE,
      std::bind(&BplJoyToServoPub::joy_callback, this, std::placeholders::_1));

  grip_state_sub_ = this->create_subscription<
      control_msgs::msg::JointTrajectoryControllerState>(
      GRIP_STATE_TOPIC, ROS_QUEUE_SIZE,
      std::bind(&BplJoyToServoPub::grip_state_callback, this,
                std::placeholders::_1));
}

void BplJoyToServoPub::init_service() {
  // Create a service client to start the ServoServer
  servo_start_client_ =
      this->create_client<std_srvs::srv::Trigger>("/servo_node/start_servo");
  servo_stop_client_ =
      this->create_client<std_srvs::srv::Trigger>("/servo_node/stop_servo");
  neutral_client_ = this->create_client<std_srvs::srv::Trigger>(
      "/pj_moveit2_adaptor/move_to_neutral");
  home_client_ = this->create_client<std_srvs::srv::Trigger>(
      "/pj_moveit2_adaptor/move_to_home");
  preside_client_ = this->create_client<std_srvs::srv::Trigger>(
      "/pj_moveit2_adaptor/move_to_preside");
  execute_client_ = this->create_client<std_srvs::srv::Trigger>(
      "/pj_moveit2_adaptor/execute_goal");
  pre_origin_client_ = this->create_client<pj_moveit2_msgs::srv::PreOrigin>(
      "/pj_moveit2_adaptor/prepare_origin");
  frame_goal_client_ = this->create_client<pj_moveit2_msgs::srv::FrameGoal>(
      "/pj_moveit2_adaptor/frame_goal");
  remove_tf_client_ = this->create_client<std_srvs::srv::Trigger>(
      "/pj_moveit2_adaptor/remove_tf");
  arc_client_ = this->create_client<pj_moveit2_msgs::srv::SphericalGoal>(
      "/pj_moveit2_adaptor/arc_goal");
}

void BplJoyToServoPub::grip_state_callback(
    const control_msgs::msg::JointTrajectoryControllerState::SharedPtr msg) {
  grip_.data = msg->reference.positions[0];
}

void BplJoyToServoPub::send_service(const std::vector<int32_t> &buttons,
                                    const std::vector<float> &axes) {
  if (!mode_operate) {
    if (!buttons[BACK]) {
      send_back = true;
    } else if (buttons[BACK] && send_back == true) {
      RCLCPP_INFO(LOGGER, "TORSO DOWN");
      tor_data.data = -0.1;
      tor_pub_->publish(tor_data);
      send_back = false;
    }

    if (!buttons[START]) {
      send_start = true;
    } else if (buttons[START] && send_start == true) {
      RCLCPP_INFO(LOGGER, "TORSO UP");
      tor_data.data = 0.1;
      tor_pub_->publish(tor_data);
      send_start = false;
    }

    if (!buttons[HOME]) {
      send_tick = true;
    } else if (buttons[HOME] && send_tick == true) {
      static bool toggle = false;
      if (toggle) {
        servo_start_client_->wait_for_service(std::chrono::seconds(1));
        servo_start_client_->async_send_request(
            std::make_shared<std_srvs::srv::Trigger::Request>());
        RCLCPP_INFO(LOGGER, "SERVO START");
      } else {
        servo_stop_client_->wait_for_service(std::chrono::seconds(1));
        servo_stop_client_->async_send_request(
            std::make_shared<std_srvs::srv::Trigger::Request>());
        RCLCPP_INFO(LOGGER, "SERVO STOP");
      }
      toggle = !toggle;
      send_tick = false;
    }

    if (axes[D_PAD_Y] == 0) {
      send_execute = true;
    } else if (axes[D_PAD_Y] == 1 && send_execute == true) {
      execute_client_->async_send_request(
          std::make_shared<std_srvs::srv::Trigger::Request>());
      RCLCPP_INFO(LOGGER, "execute goal");
      send_execute = false;
    }

    if (axes[D_PAD_Y] == 0) {
      send_home = true;
    } else if (axes[D_PAD_Y] == -1 && send_home == true) {
      home_client_->async_send_request(
          std::make_shared<std_srvs::srv::Trigger::Request>());
      RCLCPP_INFO(LOGGER, "move to home");
      send_home = false;
    }

    if (axes[D_PAD_X] == 0) {
      send_preside = true;
    } else if (axes[D_PAD_X] == 1 && send_preside == true) {
      preside_client_->async_send_request(
          std::make_shared<std_srvs::srv::Trigger::Request>());
      RCLCPP_INFO(LOGGER, "move to preside");
      send_preside = false;
    }

    if (axes[D_PAD_X] == 0) {
      send_neutral = true;
    } else if (axes[D_PAD_X] == -1 && send_neutral == true) {
      neutral_client_->async_send_request(
          std::make_shared<std_srvs::srv::Trigger::Request>());
      RCLCPP_INFO(LOGGER, "move to neutral");
      send_neutral = false;
    }
  } else {
    if (axes[D_PAD_Y] == 0) {
      send_execute = true;
    } else if (axes[D_PAD_Y] == 1 && send_execute == true) {
      execute_client_->async_send_request(
          std::make_shared<std_srvs::srv::Trigger::Request>());
      RCLCPP_INFO(LOGGER, "execute goal");
      send_execute = false;
    }

    if (axes[D_PAD_X] == 0) {
      send_frame = true;
    } else if (axes[D_PAD_X] == -1 && send_frame == true) {
      RCLCPP_INFO(LOGGER, "frame goal");
      auto valve_pose =
          std::make_shared<pj_moveit2_msgs::srv::FrameGoal::Request>();
      valve_pose->frame_id.data = "origin";
      valve_pose->offset_x.data = 0.12;
      valve_pose->offset_y.data = 0.0;
      valve_pose->offset_z.data = 0.0;
      valve_pose->offset_roll.data = 0.0;
      valve_pose->offset_pitch.data = M_PI_2;
      valve_pose->offset_yaw.data = 0.0;
      auto success = frame_goal_client_->async_send_request(valve_pose);
      send_frame = false;
    }

    if (axes[D_PAD_X] == 0) {
      send_pframe = true;
    } else if (axes[D_PAD_X] == 1 && send_pframe == true) {
      RCLCPP_INFO(LOGGER, "pre frame goal");
      auto valve_pose =
          std::make_shared<pj_moveit2_msgs::srv::FrameGoal::Request>();
      valve_pose->frame_id.data = "origin";
      valve_pose->offset_x.data = 0.12;
      valve_pose->offset_y.data = 0.0;
      valve_pose->offset_z.data = 0.1;
      valve_pose->offset_roll.data = 0.0;
      valve_pose->offset_pitch.data = M_PI_2;
      valve_pose->offset_yaw.data = 0.0;
      auto success = frame_goal_client_->async_send_request(valve_pose);
      send_pframe = false;
    }

    if (!buttons[BACK]) {
      send_remove = true;
    } else if (buttons[BACK] && send_remove == true) {
      RCLCPP_INFO(LOGGER, "remove tf");
      valve_name_.clear();
      send_remove = false;
    }

    if (!buttons[START]) {
      send_origin = true;
    } else if (buttons[START] && send_origin == true) {
      RCLCPP_INFO(LOGGER, "pre origin");
      valve_name_ = "ar_frame_12";
      current_tran_ = valve_tran_;
      send_origin = false;
    }

    if (!buttons[A]) {
      send_close = true;
    } else if (buttons[A] && send_close == true) {
      auto request =
          std::make_shared<pj_moveit2_msgs::srv::SphericalGoal::Request>();
      request->sph.frame_id.data = "origin";
      request->sph.radius.data = 0.0;
      request->sph.phi.data = 0.0;
      request->sph.theta.data = 1.62;
      arc_client_->async_send_request(request);
      RCLCPP_INFO(LOGGER, "close valve");
      send_close = false;
    }

    if (!buttons[Y]) {
      send_open = true;
    } else if (buttons[Y] && send_open == true) {
      auto request =
          std::make_shared<pj_moveit2_msgs::srv::SphericalGoal::Request>();
      request->sph.frame_id.data = "origin";
      request->sph.radius.data = 0.0;
      request->sph.phi.data = 0.0;
      request->sph.theta.data = -1.62;
      arc_client_->async_send_request(request);
      RCLCPP_INFO(LOGGER, "open valve");
      send_open = false;
    }

    if (!buttons[LEFT_STICK_CLICK]) {
      send_gpframe = true;
    } else if (buttons[LEFT_STICK_CLICK] && send_gpframe == true) {
      RCLCPP_INFO(LOGGER, "pre frame goal");
      auto valve_pose =
          std::make_shared<pj_moveit2_msgs::srv::FrameGoal::Request>();
      valve_pose->frame_id.data = "origin";
      valve_pose->offset_x.data = 0.0;
      valve_pose->offset_y.data = 0.0;
      valve_pose->offset_z.data = 0.1;
      valve_pose->offset_roll.data = 0.0;
      valve_pose->offset_pitch.data = M_PI_2;
      valve_pose->offset_yaw.data = 0.0;
      auto success = frame_goal_client_->async_send_request(valve_pose);
      send_gpframe = false;
    }

    if (!buttons[RIGHT_STICK_CLICK]) {
      send_gframe = true;
    } else if (buttons[RIGHT_STICK_CLICK] && send_gframe == true) {
      RCLCPP_INFO(LOGGER, "frame goal");
      auto valve_pose =
          std::make_shared<pj_moveit2_msgs::srv::FrameGoal::Request>();
      valve_pose->frame_id.data = "origin";
      valve_pose->offset_x.data = 0.0;
      valve_pose->offset_y.data = 0.0;
      valve_pose->offset_z.data = 0.0;
      valve_pose->offset_roll.data = 0.0;
      valve_pose->offset_pitch.data = M_PI_2;
      valve_pose->offset_yaw.data = 0.0;
      auto success = frame_goal_client_->async_send_request(valve_pose);
      send_gframe = false;
    }
  }
}

void BplJoyToServoPub::select_mode(const std::vector<int32_t> &buttons,
                                   const std::vector<float> &axes) {
  if (!buttons[LB] && !buttons[RB]) {
    send_mode = true;
  } else if (buttons[LB] && buttons[RB] && send_mode == true) {
    if (!mode_operate) {
      RCLCPP_INFO(LOGGER, "mode valve operation");
      mode_operate = true;
    } else {
      RCLCPP_INFO(LOGGER, "mode arm operation");
      mode_operate = false;
    }
    send_mode = false;
  }
}

void BplJoyToServoPub::valve_control_mode(const std::vector<int32_t> &buttons,
                                          const std::vector<float> &axes) {
  if (mode_operate) {
    current_tran_.transform.translation.x += axes[LEFT_STICK_X] * 0.005;
    current_tran_.transform.translation.y += -axes[LEFT_STICK_Y] * 0.005;
    double lin_x_right =
        -0.5 * (axes[RIGHT_TRIGGER] - AXIS_DEFAULTS.at(RIGHT_TRIGGER));
    double lin_x_left =
        0.5 * (axes[LEFT_TRIGGER] - AXIS_DEFAULTS.at(LEFT_TRIGGER));
    current_tran_.transform.translation.z += (lin_x_right + lin_x_left) * 0.005;
    double roll_positive = buttons[B];
    double roll_negative = -1 * (buttons[X]);

    tf2::Quaternion q_orig, q_rot, q_new;
    tf2::convert(current_tran_.transform.rotation, q_orig);
    double r = axes[RIGHT_STICK_X] * 0.01,
           p = (roll_positive + roll_negative) * 0.01,
           y = axes[RIGHT_STICK_Y] * 0.01;
    q_rot.setRPY(r, p, y);

    q_new = q_orig * q_rot; // Calculate the new orientation
    q_new.normalize();
    tf2::convert(q_new, current_tran_.transform.rotation);
  }
}

void BplJoyToServoPub::joy_callback(
    const sensor_msgs::msg::Joy::SharedPtr msg) {
  // Create the messages we might publish
  auto twist_msg = std::make_unique<geometry_msgs::msg::TwistStamped>();
  auto joint_msg = std::make_unique<std_msgs::msg::Float64>();

  joint_msg->data = grip_.data;

  if (valve_name_ != "") {
    valve_tf(valve_name_);
    pre_origin();
  }
  send_service(msg->buttons, msg->axes);
  select_mode(msg->buttons, msg->axes);
  valve_control_mode(msg->buttons, msg->axes);
  if (msg->buttons[4] != 1 && msg->buttons[5] != 1 && !mode_operate) {
    // Convert the joystick message to Twist or JointJog and publish
    if (convert_joy_to_cmd(msg->axes, msg->buttons, twist_msg, joint_msg)) {
      // publish the TwistStamped
      twist_msg->header.frame_id = frame_to_publish_;
      twist_msg->header.stamp = this->now();
      twist_pub_->publish(std::move(twist_msg));
    } else {
      // publish the JointJog
      traj_.header.frame_id = ROBOT_BASE_FRAME_ID;
      traj_.joint_names[0] = GRIPPER_JOINT_NAME_ID;
      if (traj_.points[0].positions[0] > 0.01) {
        traj_.points[0].positions[0] = 0.01;
      } else if (traj_.points[0].positions[0] < 0.002) {
        traj_.points[0].positions[0] = 0.002;
      } else {
        traj_.points[0].positions[0] += joint_msg->data;
      }
      gripper_pub_->publish(traj_);
    }
  }
}

pj_moveit2_msgs::msg::SphericalCoord
BplJoyToServoPub::rectan_to_sphere_feedback(geometry_msgs::msg::Pose pose) {
  pj_moveit2_msgs::msg::SphericalCoord msg;
  msg.radius.data = sqrt((pose.position.x * pose.position.x) +
                         (pose.position.y * pose.position.y) +
                         (pose.position.z * pose.position.z));
  msg.theta.data = atan2(pose.position.y, pose.position.x);
  msg.phi.data = acos(pose.position.z / msg.radius.data);
  return msg;
}

geometry_msgs::msg::Pose
BplJoyToServoPub::sphere_to_rectan(pj_moveit2_msgs::msg::SphericalCoord sph) {
  geometry_msgs::msg::Pose msg;
  tf2::Quaternion q;
  q.setRPY(0.0, sph.phi.data, sph.theta.data);
  msg.position.x = sph.radius.data * sin(sph.phi.data) * cos(sph.theta.data);
  msg.position.y = sph.radius.data * sin(sph.phi.data) * sin(sph.theta.data);
  msg.position.z = sph.radius.data * cos(sph.phi.data);
  msg.orientation.x = q.x();
  msg.orientation.y = q.y();
  msg.orientation.z = q.z();
  msg.orientation.w = q.w();
  return msg;
}

void BplJoyToServoPub::valve_tf(std::string valve_frame) {
  try {
    valve_tran_ = tf_buffer_->lookupTransform(
        base_frame_, valve_frame, rclcpp::Time(0), rclcpp::Duration(5, 0));
  } catch (tf2::TransformException &e) {
    RCLCPP_DEBUG(LOGGER, "%s", e.what());
  }
}

void BplJoyToServoPub::pre_origin() {
  try {
    geometry_msgs::msg::TransformStamped static_trans;
    tf2::Quaternion q_orig, q_rot, q_new;
    static_trans.header.stamp = this->get_clock()->now();
    static_trans.header.frame_id = base_frame_;
    static_trans.child_frame_id = "origin";
    static_trans.transform.translation.x =
        current_tran_.transform.translation.x;
    static_trans.transform.translation.y =
        current_tran_.transform.translation.y;
    static_trans.transform.translation.z =
        current_tran_.transform.translation.z;
    // Get the original orientation of 'commanded_pose'
    tf2::convert(current_tran_.transform.rotation, q_orig);
    double r = 0.0, p = -M_PI_2, y = 0.0;
    q_rot.setRPY(r, p, y);

    q_new = q_orig * q_rot; // Calculate the new orientation
    q_new.normalize();
    tf2::convert(q_new, static_trans.transform.rotation);
    tf_broadcaster_->sendTransform(static_trans);
  } catch (tf2::TransformException &e) {
    RCLCPP_DEBUG(LOGGER, "%s", e.what());
  }
}

} // namespace moveit_servo

// Register the component with class_loader
#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(moveit_servo::BplJoyToServoPub)
