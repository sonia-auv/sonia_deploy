#pragma once

#include <rclcpp/rclcpp.hpp>
#include <rosbag2_transport/recorder.hpp>
#include <chrono>
#include <thread>

#include <sonia_common_ros2/msg/node_status.hpp>
#include <sonia_common_ros2/srv/record_bag_service.hpp>

namespace sonia_deploy
{
    
    /**
     * @class BagServer
     * @brief Handles ros bags recording on the system.
     */
    class BagServer : public rclcpp::Node
    {
        public:
            BagServer();
            ~BagServer() override = default;

            void setExecutor(std::shared_ptr<rclcpp::executors::MultiThreadedExecutor> executor);

        private:
            /**
             * @brief Process client requests for ros bag recordings.
             * @param request Request information from the client.
             * @param response Response from the server.
             */
            void processRecordRequest(const std::shared_ptr<sonia_common_ros2::srv::RecordBagService::Request> request, std::shared_ptr<sonia_common_ros2::srv::RecordBagService::Response> response);
            
            /**
             * @brief Publishes node information of its state and quality.
             */
            void publishStatus();

            std::shared_ptr<rosbag2_transport::Recorder> recorder_;
            std::shared_ptr<rclcpp::executors::MultiThreadedExecutor> executor_;
            std::thread recording_thread_;

            rclcpp::TimerBase::SharedPtr timer_node_status_;
            rclcpp::Publisher<sonia_common_ros2::msg::NodeStatus>::SharedPtr pub_node_status_;
            rclcpp::Service<sonia_common_ros2::srv::RecordBagService>::SharedPtr bag_service_;

            std::string save_path_;
            std::string filename_;
            std::atomic<bool> is_recording_;
            sonia_common_ros2::msg::NodeStatus node_status_;

            const uint16_t SPLIT_DURATION = 180; //3 minutes

    };
}//namespace sonia_deploy