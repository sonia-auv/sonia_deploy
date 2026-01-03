#pragma once
#include "rclcpp/rclcpp.hpp"
#include <chrono>
#include "sonia_common_ros2/msg/node_status.hpp"
#include "sonia_common_ros2/msg/system_status.hpp"

namespace sonia_deploy
{
    class SystemMonitor : public rclcpp::Node
    {
    public:
        SystemMonitor();
        ~SystemMonitor() = default; 

    private:
        void processNodeStatusCallback(const sonia_common_ros2::msg::NodeStatus &msg);
        void publishSystemStatus();
        
        rclcpp::Subscription<sonia_common_ros2::msg::NodeStatus>::SharedPtr _sub_node_status;
        rclcpp::Publisher<sonia_common_ros2::msg::SystemStatus>::SharedPtr _pub_system_status;
        rclcpp::TimerBase::SharedPtr _timerSystemStatus;

        std::unordered_map<std::string, sonia_common_ros2::msg::NodeStatus> _map_nodes;
        std::vector<std::string> _sources;
        
    };
} // namespace sonia_deploy