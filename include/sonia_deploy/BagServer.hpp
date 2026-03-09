#pragma once

#include "rclcpp/rclcpp.hpp"
#include <rosbag2_transport/recorder.hpp>
#include <rosbag2_transport/record_options.hpp>
#include <rosbag2_storage/storage_options.hpp>
#include <chrono>
#include <filesystem>
#include <pwd.h>

#include "sonia_common_ros2/srv/record_bag_service.hpp"

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
            ~BagServer() = default;

        private:

            void processBag(const std::shared_ptr<sonia_common_ros2::srv::RecordBagService::Request> request, const std::shared_ptr<sonia_common_ros2::srv::RecordBagService::Response> response);
            
            /**
             * @brief Publishes node information of its state and quality.
             */
            void publishStatus();

            std::shared_ptr<rosbag2_transport::Recorder> recorder_;
            rclcpp::Service<sonia_common_ros2::srv::RecordBagService>::SharedPtr bag_service_;
            std::string save_path;

    };
}//namespace sonia_deploy