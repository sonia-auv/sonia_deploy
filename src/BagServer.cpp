#include "sonia_deploy/BagServer.hpp"

using namespace std::placeholders;
namespace sonia_deploy
{
    BagServer::BagServer() : Node("Bag_recorder")
    {
        save_path = getpwuid(getuid())->pw_dir;

        const char *auv = std::getenv("AUV");
        if(strcmp(auv, "AUV8") == 0 || strcmp(auv, "LITE1") == 0)
            save_path.append("/ssd/bags/");
        else
            save_path.append("/bags/");

        bag_service_ = this->create_service<sonia_common_ros2::srv::RecordBagService>(
            "/bag_recorder/record", std::bind(&BagServer::processBag, this, _1, _2));

        RCLCPP_INFO(this->get_logger(), "Bag Server up running");
    }

    void BagServer::processBag(const std::shared_ptr<sonia_common_ros2::srv::RecordBagService::Request> request, const std::shared_ptr<sonia_common_ros2::srv::RecordBagService::Response> response)
    {
        switch (request->cmd)
        {
            case sonia_common_ros2::srv::RecordBagService::Request::CMD_START:
            {
                auto writer = std::make_shared<rosbag2_cpp::Writer>();
                rosbag2_storage::StorageOptions options;
                options.uri = save_path.append(request->filename);
                options.storage_id = "sqlite3";

                rosbag2_transport::RecordOptions record_options;
                record_options.all = false;
                record_options.topics = request->topic_list;
                record_options.rmw_serialization_format = "cdr";
                
                recorder_ = std::make_shared<rosbag2_transport::Recorder>(writer, options, record_options);

                recorder_->record();
                response->state = "Recording started";
                RCLCPP_INFO(this->get_logger(), "Recording");
                break; 
            }
            case sonia_common_ros2::srv::RecordBagService::Request::CMD_PAUSE:
            {
                recorder_->pause();
                response->state = "Recording paused";
                break; 
            }
            case sonia_common_ros2::srv::RecordBagService::Request::CMD_RESUME:
            {
                recorder_->resume();
                response->state = "Recording resumed";
                break; 
            }
            case sonia_common_ros2::srv::RecordBagService::Request::CMD_STOP:
            {
                recorder_->stop();
                response->state = "Recording stopped";
                break; 
            }          
            default:
                break;
        }
    }
  
    void BagServer::publishStatus(){

    }

} //namespace sonia_deploy