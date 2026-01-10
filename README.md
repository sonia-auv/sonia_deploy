# sonia_deploy

The **SystemMonitor** node is a monitoring tool used to observe, evaluate and showcase the operational status of the **ROS2** nodes. The monitored nodes periodically publish their current state and health quality. The tool provides a summarized system health status of all the nodes it monitors. 

---

## Dependencies

### ROS 2 Distro

* Humble

### ROS 2 Packages

* `ament_cmake`
* `rclcpp`
* `std_msgs`
* `std_srvs`

### Sonia packages

* `sonia_common_ros2`

---

## Node

* Name: `system_monitor`

---

## Registered Topics / Services / Actions

| Type             | Name                            | Direction     | Message/Service Type                    | Description                                                   |
| ---------------- | ------------------------------- | ------------- | --------------------------------------- | ------------------------------------------------------------- |
| Topic            | `/system_monitor/node_status`   | Subscribed    | `sonia_common_ros2/msg/NodeStatus`      | Message contains stats from a node                            |
| Topic            | `/system_monitor/system_status` | Published     | `sonia_common_ros2/msg/SystemStatus`    | Message contains a list of all monitored nodes and their stats|

---
## Build Instructions
To build the project, the following commands should be run directly from your ROS2 workspace.

```bash
colcon build --packages-select sonia_deploy --symlink-install
source install/setup.bash
```

---

## Launch Instructions
Required environment variables to launch the project

```bash
export AUV={prototype_identifier}
```
replace `{prototype_identifier}` with available options: `AUV8` | `LITE1`.

### Default launch
The command launchs the mission server 

```bash
ros2 launch sonia_deploy launch.py
```

---

## Useful ROS 2 Commands

```bash
ros2 node list
ros2 node info /system_monitor
ros2 topic echo /system_monitor/node_status
ros2 param list /system_monitor
```

---

## References

* [sonia_common_ros2](https://github.com/sonia-auv/sonia_common_ros2)
---