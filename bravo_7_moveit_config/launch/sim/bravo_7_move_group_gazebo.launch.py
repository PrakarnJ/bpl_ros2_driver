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

    # Trajectory Execution Functionality
    controllers_yaml = load_yaml('bravo_7_moveit_config', 'config/bravo_7_moveit_controllers.yaml')
    moveit_controllers = { 'moveit_simple_controller_manager' : controllers_yaml,
                           'moveit_controller_manager': 'moveit_simple_controller_manager/MoveItSimpleControllerManager'}

    trajectory_execution = {'moveit_manage_controllers': True,
                            'trajectory_execution.allowed_execution_duration_scaling': 10.0,
                            'trajectory_execution.execution_duration_monitoring': False,
                            'trajectory_execution.allowed_goal_duration_margin': 0.5,
                            'trajectory_execution.allowed_start_tolerance': 0.01}

    planning_scene_monitor_parameters = {"publish_planning_scene": True,
                 "publish_geometry_updates": True,
                 "publish_state_updates": True,
                 "publish_transforms_updates": True}

    octomap_config = {'octomap_frame': 'base_link', 
                    'octomap_resolution': 0.05,
                    'max_range': 1.5}

    octomap_updater_config = load_yaml('bravo_7_moveit_config', 'config/sensors_3d.yaml')

    joint_limits_yaml = {
    "robot_description_planning": load_yaml(
        "bravo_7_moveit_config", "config/joint_limits.yaml"
    )
    }

    # Start the actual move_group node/action server
    run_move_group_node = Node(package='moveit_ros_move_group',
                               executable='move_group',
                               output='screen',
                               parameters=[robot_description,
                                           robot_description_semantic,
                                           kinematics_yaml,
                                           ompl_planning_pipeline_config,
                                           trajectory_execution,
                                           moveit_controllers,
                                           planning_scene_monitor_parameters,
                                           joint_limits_yaml,
                                           {'use_sim_time': True},
                                        #    octomap_config,
                                        #    octomap_updater_config
                                           ])

    parameters_file_name= 'pj_moveit2_adaptor.yaml'

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
                               parameters=[robot_description,
                                           robot_description_semantic,
                                           parameters_file_path,
                                           kinematics_yaml]
                               )


    model = LaunchConfiguration('model')
    declare_model_cmd = DeclareLaunchArgument(
            'model',
            default_value='bravo_7_robot.gazebo.xacro',
            description='Robot model .xacro')

    joy_node = Node(
        package='joy',
        executable='joy_node',
        name='joy_node'
    )

    oru_moveit_path = os.path.join(
        get_package_share_directory('bravo_7_moveit_config'), 'launch/sim')

    moveit_servo = IncludeLaunchDescription(
        AnyLaunchDescriptionSource(os.path.join(
            oru_moveit_path, 'bravo_7_servo_teleop_gazebo.launch.py'))
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
    ld.add_action(pj_moveit2_adaptor_node)
    ld.add_action(run_move_group_node)
    # ld.add_action(moveit_servo)
    # ld.add_action(joy_node)
    # ld.add_action(mongodb_server_node)

    return ld
