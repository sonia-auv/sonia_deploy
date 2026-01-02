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
        
        rclcpp::Subscription<sonia_common_ros2::msg::NodeStatus>::SharedPtr _node_status_sub;
        rclcpp::Publisher<sonia_common_ros2::msg::SystemStatus>::SharedPtr _system_status_pub;
        rclcpp::TimerBase::SharedPtr _timerSystemStatus;

        sonia_common_ros2::msg::SystemStatus _systemStatus;
        std::vector<std::string> sources;
        
    };
} // namespace sonia_deploy