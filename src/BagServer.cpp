#include <pwd.h>
#include <unistd.h>
#include <filesystem>
#include <rosbag2_storage/storage_options.hpp>
#include <rosbag2_transport/record_options.hpp>
#include <sonia_monitor/BagServer.hpp>

using namespace std::chrono_literals;
using namespace std::placeholders;

namespace sonia_monitor
{
    BagServer::BagServer() : Node("bag_server"), is_recording_{false}
    {
        auto pwuid = getpwuid(getuid());
        if (pwuid == nullptr)
        {
            throw std::runtime_error("Can't find HOME directory");
        }

        std::string path = pwuid->pw_dir;
        std::string ssd_path = path + "/ssd";

        if (std::filesystem::exists(ssd_path) && std::filesystem::is_directory(ssd_path))
            save_path_ = path + "/ssd/bags/";
        else
            save_path_ = path + "/bags/";

        pub_node_status_ = this->create_publisher<sonia_common_ros2::msg::NodeStatus>("/system_monitor/node_status", 1);
        bag_service_ = this->create_service<sonia_common_ros2::srv::RecordBagService>(
            "/bag_server/record", std::bind(&BagServer::processRecordRequest, this, _1, _2));

        timer_node_status_ = this->create_wall_timer(500ms, std::bind(&BagServer::publishStatus, this));

        node_status_.node_name = this->get_name();
        node_status_.quality = sonia_common_ros2::msg::NodeStatus::Q_OK;
        node_status_.state = sonia_common_ros2::msg::NodeStatus::STATE_IDLE;

        RCLCPP_INFO(this->get_logger(), "Bag Server up running");
    }

    void BagServer::setExecutor(std::shared_ptr<rclcpp::executors::MultiThreadedExecutor> executor)
    {
        executor_= executor;
    }

    void BagServer::processRecordRequest(
        const std::shared_ptr<sonia_common_ros2::srv::RecordBagService::Request> request,
        std::shared_ptr<sonia_common_ros2::srv::RecordBagService::Response> response)
    {
        switch (request->cmd)
        {
            case sonia_common_ros2::srv::RecordBagService::Request::CMD_START:
            {
                size_t active_topics = 0;
                auto path = save_path_ + request->filename;
                if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
                {
                    response->message = "Error!! A bag with the same name already exists";
                    break;
                }
                if (is_recording_)
                {
                    response->message = "There is a recording in progress, send CMD to stop before beginning a new one";
                    break;
                }

                auto writer = std::make_shared<rosbag2_cpp::Writer>();
                rosbag2_storage::StorageOptions options;
                options.uri = path;
                options.max_bagfile_duration = SPLIT_DURATION;
                filename_ = request->filename;
                options.storage_id = "mcap";

                rosbag2_transport::RecordOptions record_options;
                record_options.all = false;
                record_options.topics = request->topic_list;
                record_options.rmw_serialization_format = "cdr";

                recorder_ = std::make_shared<rosbag2_transport::Recorder>(writer, options, record_options, RECORDER_NODE_NAME);
                executor_->add_node(recorder_);

                recorder_->record();
                active_topics = countRecordedTopics(record_options.topics);

                std::this_thread::sleep_for(RECORDER_WAIT); //sleep to allow recorder to safely complete start 

                is_recording_ = true;
                std::ostringstream oss;
                oss <<"Recording started with " << active_topics << " active topics";
                response->message = oss.str();

                node_status_.state = sonia_common_ros2::msg::NodeStatus::STATE_RUNNING;
                break;
            }
            case sonia_common_ros2::srv::RecordBagService::Request::CMD_PAUSE:
            {
                if(recorder_ && is_recording_){
                    recorder_->pause();
                    response->message = "Recording paused";
                }   
                break;
            }
            case sonia_common_ros2::srv::RecordBagService::Request::CMD_RESUME:
            {
                if(is_recording_)
                {
                    recorder_->resume();
                    response->message = "Recording resumed";
                }
                break;
            }
            case sonia_common_ros2::srv::RecordBagService::Request::CMD_STOP:
            {
                if(is_recording_){
                    recorder_->stop();
                    std::this_thread::sleep_for(RECORDER_WAIT); //sleep to allow recorder to stop correctly
                    executor_->remove_node(recorder_->get_node_base_interface());
                    recorder_.reset();
                    is_recording_ = false;
                    response->message = "Recording stopped. The saved rosbag : " + filename_;

                    node_status_.state = sonia_common_ros2::msg::NodeStatus::STATE_IDLE;
                } 
                break;
            }
            default:
            {
                RCLCPP_INFO(this->get_logger(), "Unknown CMD, verify available commands for the server");
                break;
            }
        }
    }

    size_t BagServer::countRecordedTopics(const std::vector<std::string> topics)
    {
        size_t counter = 0;
        for (const auto & topic : topics) {
            auto infos = recorder_->get_subscriptions_info_by_topic(topic);

            for (const auto & info : infos) {
                if (!info.node_name().compare(RECORDER_NODE_NAME)) {
                    counter++;
                }
            }
        } 
        return counter;       
    }

    void BagServer::publishStatus()
    {
        node_status_.stamp = this->now();
        pub_node_status_->publish(node_status_);
    }

}  // namespace sonia_monitor