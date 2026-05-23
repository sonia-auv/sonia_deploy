import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    auv = os.getenv("AUV", "")
    
    config = os.path.join(
        get_package_share_directory("sonia_monitor"), "config", f"{auv}_system.yaml"
    )
    if not os.path.exists(config):
        raise Exception("Config file unknown, check AUV env")
    
    return LaunchDescription(
        [
            Node(
                package = "sonia_monitor",
                executable = "sonia_monitor",
                parameters = [config]
            )
        ]
    )