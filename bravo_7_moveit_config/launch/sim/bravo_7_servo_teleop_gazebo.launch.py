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
import yaml
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode
from launch.substitutions import LaunchConfiguration, Command, PythonExpression
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.conditions import IfCondition, UnlessCondition
import xacro

def load_file(package_name, file_path):
    package_path = get_package_share_directory(package_name)
    absolute_file_path = os.path.join(package_path, file_path)

    try:
        with open(absolute_file_path, 'r') as file:
            return file.read()
    except EnvironmentError: # parent of IOError, OSError *and* WindowsError where available
        return None

def load_yaml(package_name, file_path):
    package_path = get_package_share_directory(package_name)
    absolute_file_path = os.path.join(package_path, file_path)

    try:
        with open(absolute_file_path, 'r') as file:
            return yaml.safe_load(file)
    except EnvironmentError: # parent of IOError, OSError *and* WindowsError where available
        return None

def generate_launch_description():
    servo_yaml = load_yaml('bpl_controllers', 'config/bravo_7_servo_config_sim.yaml')
    servo_params = {'moveit_servo' : servo_yaml}

    # planning_context
    robot_description_config = os.path.join(get_package_share_directory('bravo_7_description'), 'urdf', 'bravo_7_robot.gazebo.xacro')
    robot_description= {'robot_description' : Command(['xacro', ' ', robot_description_config])}

    robot_description_semantic_config = load_file('bravo_7_moveit_config', 'config/bravo_7_robot.srdf')
    robot_description_semantic = {'robot_description_semantic' : robot_description_semantic_config}

    # Launch as much as possible in components
    container = ComposableNodeContainer(
        name="moveit_servo_demo_container",
        namespace="/",
        package="rclcpp_components",
        executable="component_container",
        composable_node_descriptions=[
            ComposableNode(
                package="moveit_servo",
                plugin="moveit_servo::ServoServer",
                name="servo_server",
                parameters=[
                    servo_params,
                    robot_description,
                    robot_description_semantic,
                ],
                extra_arguments=[{"use_intra_process_comms": True}],
                
            ),
            ComposableNode(
                package="bpl_controllers",
                plugin="moveit_servo::BplJoyToServoPub",
                name="controller_to_servo_node",
                extra_arguments=[{"use_intra_process_comms": True}],
            )
        ],
        output="screen"
    )

    ld = LaunchDescription()

    ld.add_action(container)

    return ld
