#include <pwd.h>
#include <chrono>
#include <filesystem>
#include <sonia_deploy/BagServer.hpp>
#include <rosbag2_storage/storage_options.hpp>
#include <rosbag2_transport/record_options.hpp>

using namespace std::chrono_literals;
using namespace std::placeholders;

namespace sonia_deploy
{
    BagServer::BagServer() : Node("Bag_recorder"), is_recording_{false}
    {
        std::string path = getpwuid(getuid())->pw_dir;
        std::string ssd_path = path + "/ssd";

        if (std::filesystem::exists(ssd_path) && std::filesystem::is_directory(ssd_path))
            save_path_ = path + "/ssd/bags/";
        else
            save_path_ = path + "/bags/";

        pub_node_status_ = this->create_publisher<sonia_common_ros2::msg::NodeStatus>("/system_monitor/node_status", 1);
        bag_service_ = this->create_service<sonia_common_ros2::srv::RecordBagService>(
            "/bag_recorder/record", std::bind(&BagServer::processRecordRequest, this, _1, _2));

        timer_node_status_ = this->create_wall_timer(500ms, std::bind(&BagServer::publishStatus, this));

        node_status_.node_name = this->get_name();
        node_status_.quality = sonia_common_ros2::msg::NodeStatus::Q_OK;
        node_status_.state = sonia_common_ros2::msg::NodeStatus::STATE_IDLE;

        RCLCPP_INFO(this->get_logger(), "Bag Server up running");
    }

    void BagServer::processRecordRequest(
        const std::shared_ptr<sonia_common_ros2::srv::RecordBagService::Request> request,
        std::shared_ptr<sonia_common_ros2::srv::RecordBagService::Response> response)
    {
        switch (request->cmd)
        {
            case sonia_common_ros2::srv::RecordBagService::Request::CMD_START:
            {
                auto path = save_path_ + request->filename;
                if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
                {
                    response->message = "Error!! A bag with the same name already exists";
                    break;
                }
                if(is_recording_){
                    RCLCPP_INFO(this->get_logger(), "There is a recording in progress, send CMD to stop before beginning a new one");
                    break;
                }

                auto writer = std::make_shared<rosbag2_cpp::Writer>();
                rosbag2_storage::StorageOptions options;
                options.uri = path;
                filename_ = request->filename;
                options.storage_id = "sqlite3";

                rosbag2_transport::RecordOptions record_options;
                record_options.all = false;
                record_options.topics = request->topic_list;
                record_options.rmw_serialization_format = "cdr";

                recorder_ = std::make_shared<rosbag2_transport::Recorder>(writer, options, record_options);

                recorder_->record();
                is_recording_ = true;
                response->message = "Recording started";

                node_status_.state = sonia_common_ros2::msg::NodeStatus::STATE_RUNNING;
                break;
            }
            case sonia_common_ros2::srv::RecordBagService::Request::CMD_PAUSE:
            {
                recorder_->pause();
                response->message = "Recording paused";

                break;
            }
            case sonia_common_ros2::srv::RecordBagService::Request::CMD_RESUME:
            {
                recorder_->resume();
                response->message = "Recording resumed";
                break;
            }
            case sonia_common_ros2::srv::RecordBagService::Request::CMD_STOP:
            {
                recorder_->stop();
                is_recording_ = false;
                response->message = "Recording stopped, rosbag saved : " + filename_;

                node_status_.state = sonia_common_ros2::msg::NodeStatus::STATE_IDLE;
                break;
            }
            default:
            {
                RCLCPP_INFO(this->get_logger(), "Unknown CMD, verify available commands for the server");
                break;
            }
        }
    }

    void BagServer::publishStatus()
    {
        node_status_.stamp = this->now();
        pub_node_status_->publish(node_status_);
    }

}  // namespace sonia_deploy