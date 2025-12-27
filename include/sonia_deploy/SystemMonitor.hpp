#pragma once
#include <yaml-cpp/yaml.h>
#include "rclcpp/rclcpp.hpp"
#include "sonia_common_ros2/msg/node_status.hpp"

namespace sonia_deploy
{
    class SystemMonitor : public rclcpp::Node
    {
    public:
        SystemMonitor();
        ~SystemMonitor() = default; 

    private:
        void processNodeStatusCallback(const sonia_common_ros2::msg::NodeStatus &msg);
        
        rclcpp::Subscription<sonia_common_ros2::msg::NodeStatus>::SharedPtr _node_status_sub;
        
    };
} // namespace sonia_deploy