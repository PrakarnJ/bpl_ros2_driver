# Copyright 2021 Stogl Robotics Consulting UG (haftungsbeschränkt)
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


from launch import LaunchDescription
from launch.substitutions import Command, FindExecutable, PathJoinSubstitution

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess, IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PythonExpression
from launch_ros.actions import Node
from launch.actions import ExecuteProcess



def generate_launch_description():
    # Get URDF via xacro
    robot_description_content = Command(
        [
            PathJoinSubstitution([FindExecutable(name="xacro")]),
            " ",
            PathJoinSubstitution(
                [
                    FindPackageShare("bravo_7_description"),
                    "urdf",
                    "bravo_7_robot.xacro",
                ]
            ),
        ]
    )
    robot_description = {"robot_description": robot_description_content}

    robot_controllers = PathJoinSubstitution(
        [
            FindPackageShare("bravo_7_bringup"),
            "config",
            "bravo_7_controllers.yaml",
        ]
    )

    control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[robot_description, robot_controllers],
        output={
            "stdout": "screen",
            "stderr": "screen",
        },
        # prefix=['valgrind --tool=memcheck --leak-check=yes'],
        # prefix=['gdb -ex=r --args'],
    )
    robot_state_pub_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="both",
        parameters=[robot_description],
    )

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

    forward_position_controller_spawner_stopped = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["forward_position_controller", "-c", "/controller_manager", "--inactive"],
    )

    #########################################################################
    ##################### Create launch description #########################
    ld = LaunchDescription()

    ld.add_action(control_node)
    ld.add_action(robot_state_pub_node)
    ld.add_action(joint_state_broadcaster_spawner)
    ld.add_action(bravo_7_position_controller_spawner)
    ld.add_action(bravo_7_hand_controller_spawner)
    ld.add_action(forward_position_controller_spawner_stopped)
    #########################################################################
    return ld
