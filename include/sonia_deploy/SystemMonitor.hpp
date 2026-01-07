#pragma once
#include "rclcpp/rclcpp.hpp"
#include <chrono>
#include "sonia_common_ros2/msg/node_status.hpp"
#include "sonia_common_ros2/msg/system_status.hpp"

namespace sonia_deploy
{
    /**
     * @class SystemMonitor
     * @brief Monitors a defined number of ROS2 nodes on the system.
     */
    class SystemMonitor : public rclcpp::Node
    {
    public:
        SystemMonitor();
        ~SystemMonitor() = default; 

    private:
        /**
         * @brief Process information returned by the subscriber about the node's status.
         * @param msg The message containing information of a node.
         */
        void processNodeStatusCallback(const sonia_common_ros2::msg::NodeStatus &msg);

        /**
         * @brief Publishes a summurized status array of all nodes the system monitors.
         */
        void publishSystemStatus();

        /**
         * @brief Initializes the status of ROS nodes the system monitors.
         * @param node_name The name of the ROS node.
         */
        void initializeNode(const std::string node_name);
        
        rclcpp::Subscription<sonia_common_ros2::msg::NodeStatus>::SharedPtr _sub_node_status;
        rclcpp::Publisher<sonia_common_ros2::msg::SystemStatus>::SharedPtr _pub_system_status;
        rclcpp::TimerBase::SharedPtr _timerSystemStatus;

        std::unordered_map<std::string, sonia_common_ros2::msg::NodeStatus> _mapped_nodes;
        std::vector<std::string> _sources;
        
    };
} // namespace sonia_deploy