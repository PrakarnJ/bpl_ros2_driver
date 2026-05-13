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
    # planning_context
    robot_description_config = os.path.join(get_package_share_directory('bravo_7_description'), 'urdf', 'bravo_7_robot.gazebo.xacro')
    robot_description = {'robot_description' : Command(['xacro', ' ', robot_description_config])}

    robot_description_semantic_config = load_file('bravo_7_moveit_config', 'config/bravo_7_robot.srdf')
    robot_description_semantic = {'robot_description_semantic' : robot_description_semantic_config}

    kinematics_yaml = load_yaml('bravo_7_moveit_config', 'config/kinematics.yaml')
    robot_description_kinematics = { 'robot_description_kinematics' : kinematics_yaml }
    
    # Planning Functionality
    ompl_planning_pipeline_config = { 'move_group' : {
        'planning_plugin' : 'ompl_interface/OMPLPlanner',
        'request_adapters' : """default_planner_request_adapters/AddTimeOptimalParameterization default_planner_request_adapters/FixWorkspaceBounds default_planner_request_adapters/FixStartStateBounds default_planner_request_adapters/FixStartStateCollision default_planner_request_adapters/FixStartStatePathConstraints""" ,
        'start_state_max_bounds_error' : 0.1 
        } }
    ompl_planning_yaml = load_yaml('bravo_7_moveit_config', 'config/ompl_planning.yaml')
    ompl_planning_pipeline_config['move_group'].update(ompl_planning_yaml)

    # RViz
    rviz_config_file = get_package_share_directory('bravo_7_moveit_config') + "/rviz/bravo_7_gazebo_move_group.rviz"

    rviz_node = Node(package='rviz2',
                     executable='rviz2',
                     name='rviz2',
                     output='log',
                     arguments=['-d', rviz_config_file],
                     parameters=[robot_description,
                                 robot_description_semantic,
                                 ompl_planning_pipeline_config,
                                 kinematics_yaml])

    ld = LaunchDescription()
    ld.add_action(rviz_node)

    return ld