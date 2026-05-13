# Copyright 2026 Prakarn Jaroonsorn <prakarn.jrs@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import xacro
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():

    bpl_initial_positions_file = os.path.join(
        get_package_share_directory("bravo_7_description"),
        "config",
        "bravo_7_initial_positions.yaml",
    )

    # Process the fake URDF (mock_components/GenericSystem hardware)
    # Pass initial positions path explicitly — $(find ...) in xacro defaults
    # does not resolve reliably when called from Python.
    robot_description_config = xacro.process_file(
        os.path.join(
            get_package_share_directory("bravo_7_description"),
            "urdf",
            "bravo_7_robot.fake.xacro",
        ),
        mappings={"bpl_initial_positions_file": bpl_initial_positions_file},
    )
    robot_description = {"robot_description": robot_description_config.toxml()}

    ros2_controllers_path = os.path.join(
        get_package_share_directory("bravo_7_moveit_config"),
        "config",
        "ros2_controllers.yaml",
    )

    # ros2_control_node gets robot_description from robot_state_publisher via topic
    ros2_control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[ros2_controllers_path],
        remappings=[("/controller_manager/robot_description", "/robot_description")],
        output={"stdout": "screen", "stderr": "screen"},
    )

    # Publishes /robot_description topic and TF
    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="both",
        parameters=[robot_description],
    )

    # Spawners use Node executable — reliable timing, auto-retry until CM is ready
    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster", "--controller-manager", "/controller_manager"],
    )

    bravo_7_position_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["bravo_7_position_controller", "--controller-manager", "/controller_manager"],
    )

    bravo_7_hand_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["bravo_7_hand_controller", "--controller-manager", "/controller_manager"],
    )

    # Servo controller: spawned inactive, activated when switching to servo mode
    forward_position_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["forward_position_controller", "-c", "/controller_manager", "--inactive"],
    )

    return LaunchDescription([
        ros2_control_node,
        robot_state_publisher,
        joint_state_broadcaster_spawner,
        bravo_7_position_controller_spawner,
        bravo_7_hand_controller_spawner,
        forward_position_controller_spawner,
    ])
