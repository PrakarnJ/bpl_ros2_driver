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
from launch.substitutions import LaunchConfiguration, Command
from launch.launch_description_sources import AnyLaunchDescriptionSource
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.conditions import IfCondition, UnlessCondition
from moveit_configs_utils import MoveItConfigsBuilder

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
    ros2_control_hardware_type = DeclareLaunchArgument(
        "ros2_control_hardware_type",
        default_value="mock_components",
        description="ROS 2 control hardware interface type to use for the launch file -- possible values: [mock_components, isaac]",
    )

    moveit_config = (
        MoveItConfigsBuilder("bravo_7")
        .robot_description(
            file_path="config/bravo_7_robot.urdf.xacro",
            mappings={
                "ros2_control_hardware_type": LaunchConfiguration(
                    "ros2_control_hardware_type"
                )
            },
        )
        .robot_description_semantic(file_path="config/bravo_7_robot.srdf")
        .robot_description_kinematics(file_path="config/kinematics.yaml")
        .planning_scene_monitor(
            publish_robot_description=True, publish_robot_description_semantic=True
        )
        .trajectory_execution(file_path="config/moveit_controllers.yaml")
        .planning_pipelines(
            pipelines=["ompl"]
        )
        .to_moveit_configs()
    )

    # Start the actual move_group node/action server
    run_move_group_node = Node(
        package="moveit_ros_move_group",
        executable="move_group",
        output="screen",
        parameters=[moveit_config.to_dict()],
        arguments=["--ros-args", "--log-level", "info"],
    )
 
    parameters_file_name = 'pj_moveit2_adaptor.yaml'

    parameters_file_path = os.path.join(
        get_package_share_directory('bravo_7_bringup'),
        'config',
        parameters_file_name)

    # MoveGroupInterface demo executable
    pj_moveit2_adaptor_node = Node(name='pj_moveit2_adaptor_node',
                               package='pj_moveit2_adaptor',
                               executable='pj_moveit2_adaptor_node',
                               output='screen',
                            #    arguments=['--ros-args', '--log-level', 'DEBUG'],
                            #    prefix=['gdb -ex=r --args'],
                               parameters=[moveit_config.robot_description,
                                           moveit_config.robot_description_semantic,
                                           moveit_config.robot_description_kinematics,
                                           parameters_file_path,
                                           ]
                               )
    


    model = LaunchConfiguration('model')
    declare_model_cmd = DeclareLaunchArgument(
            'model',
            default_value='bravo_7_robot.fake.xacro',
            description='Robot model .xacro')

    # Publish TF
    robot_state_publisher = IncludeLaunchDescription(AnyLaunchDescriptionSource(os.path.join(
            get_package_share_directory('bravo_7_description'), 'launch', 'robot_description.launch.py')),
            launch_arguments={'model': model}.items())

    joy_node = Node(
        package='joy',
        executable='joy_node',
        name='joy_node'
    )

    oru_moveit_path = os.path.join(
        get_package_share_directory('bravo_7_moveit_config'), 'launch/fake')

    moveit_servo = IncludeLaunchDescription(
        AnyLaunchDescriptionSource(os.path.join(
            oru_moveit_path, 'bravo_7_fake_servo_teleop.launch.py'))
    )

    # Warehouse mongodb server
    mongodb_server_node = Node(package='warehouse_ros_mongo',
                               executable='mongo_wrapper_ros.py',
                               parameters=[{'warehouse_port': 33829},
                                           {'warehouse_host': 'localhost'},
                                           {'warehouse_plugin': 'warehouse_ros_mongo::MongoDatabaseConnection'}],
                               output='screen')
    
    ld = LaunchDescription()

    ld.add_action(declare_model_cmd)
    ld.add_action(robot_state_publisher)
    ld.add_action(ros2_control_hardware_type)
    ld.add_action(pj_moveit2_adaptor_node)
    ld.add_action(run_move_group_node)
    # ld.add_action(moveit_servo)
    ld.add_action(joy_node)
    # ld.add_action(mongodb_server_node)

    return ld
