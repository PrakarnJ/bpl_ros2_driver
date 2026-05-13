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
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
from moveit_configs_utils import MoveItConfigsBuilder


def generate_launch_description():

    # Use absolute path so MoveItConfigsBuilder loads the same URDF as the
    # fake controller launch (bravo_7_robot.fake.xacro has both arm + gripper
    # joints that the SRDF references; bravo_7_robot.urdf.xacro only has arm).
    fake_urdf_path = os.path.join(
        get_package_share_directory("bravo_7_description"),
        "urdf",
        "bravo_7_robot.fake.xacro",
    )

    moveit_config = (
        MoveItConfigsBuilder("bravo_7", package_name="bravo_7_moveit_config")
        .robot_description(file_path=fake_urdf_path)
        .robot_description_semantic(file_path="config/bravo_7_robot.srdf")
        .robot_description_kinematics(file_path="config/kinematics.yaml")
        .trajectory_execution(file_path="config/moveit_controllers.yaml")
        .planning_pipelines(pipelines=["ompl"])
        .planning_scene_monitor(
            publish_robot_description=False,
            publish_robot_description_semantic=True,
        )
        .to_moveit_configs()
    )

    run_move_group_node = Node(
        package="moveit_ros_move_group",
        executable="move_group",
        output="screen",
        parameters=[moveit_config.to_dict()],
        arguments=["--ros-args", "--log-level", "info"],
    )

    parameters_file_path = os.path.join(
        get_package_share_directory("bravo_7_bringup"),
        "config",
        "pj_moveit2_adaptor.yaml",
    )

    pj_moveit2_adaptor_node = Node(
        name="pj_moveit2_adaptor_node",
        package="pj_moveit2_adaptor",
        executable="pj_moveit2_adaptor_node",
        output="screen",
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            parameters_file_path,
        ],
    )

    switch_controller_node = Node(
        package="pj_moveit2_adaptor",
        executable="switch_controllers.py",
        output="screen",
        parameters=[
            os.path.join(
                get_package_share_directory("bravo_7_bringup"),
                "config",
                "switch_controller.yaml",
            )
        ],
        respawn=True,
        respawn_delay=2.0,
        prefix="bash -c 'sleep 2; $0 $@' ",
    )

    joy_node = Node(
        package="joy",
        executable="joy_node",
        name="joy_node",
    )

    ld = LaunchDescription()
    ld.add_action(run_move_group_node)
    ld.add_action(pj_moveit2_adaptor_node)
    # ld.add_action(switch_controller_node)
    ld.add_action(joy_node)
    return ld
