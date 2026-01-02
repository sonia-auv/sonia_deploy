#include "sonia_deploy/SystemMonitor.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;
namespace sonia_deploy
{
        SystemMonitor::SystemMonitor()
        : Node("system_monitor"){
            this->declare_parameter("node_list", rclcpp::PARAMETER_STRING_ARRAY);
            sources = this->get_parameter("node_list").as_string_array();

            _node_status_sub = this->create_subscription<sonia_common_ros2::msg::NodeStatus>("/system_monitor/node_status", 1, std::bind(&SystemMonitor::processNodeStatusCallback,this,_1));
            _system_status_pub = this->create_publisher<sonia_common_ros2::msg::SystemStatus>("/system_monitor/system_status", 1);

            _timerSystemStatus = this->create_wall_timer(500ms, std::bind(&SystemMonitor::publishSystemStatus, this));
        }
        void SystemMonitor::processNodeStatusCallback(const sonia_common_ros2::msg::NodeStatus &msg){
            
            if(std::find(sources.begin(), sources.end(), msg.node_name) != sources.end()){
                RCLCPP_INFO(this->get_logger(), "message: %s, quality: %d, state: %d,", msg.node_name.c_str(), msg.quality, msg.state);
            }
            //RCLCPP_INFO(this->get_logger(), "nahhh");
            
        }

        void SystemMonitor::publishSystemStatus(){

            _systemStatus.stamp = this->get_clock().get()->now();
            _system_status_pub->publish(_systemStatus);
        }
} //namespace sonia_deploy