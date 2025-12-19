#include "sonia_deploy/SystemMonitor.hpp"

using std::placeholders::_1;
namespace sonia_deploy
{
        SystemMonitor::SystemMonitor()
        : Node("system_monitor"){
            _lifesign_sub = this->create_subscription<sonia_common_ros2::msg::NodeStatus>("/system_monitor/node_status", 1, std::bind(&SystemMonitor::processNodeStatusCallback,this,_1));
        }
        void SystemMonitor::processNodeStatusCallback(const sonia_common_ros2::msg::NodeStatus &msg){
                RCLCPP_INFO(this->get_logger(), "message: %s, level: %d, state: %d,", msg.message.c_str(), msg.level, msg.state);
        }
} //namespace sonia_deploy