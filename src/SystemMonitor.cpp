#include "sonia_deploy/SystemMonitor.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;
namespace sonia_deploy
{
    SystemMonitor::SystemMonitor()
    : Node("system_monitor"){
        this->declare_parameter("node_list", rclcpp::PARAMETER_STRING_ARRAY);
        _sources = this->get_parameter("node_list").as_string_array();

        _sub_node_status = this->create_subscription<sonia_common_ros2::msg::NodeStatus>("/system_monitor/node_status", 1, std::bind(&SystemMonitor::processNodeStatusCallback,this,_1));
        _pub_system_status = this->create_publisher<sonia_common_ros2::msg::SystemStatus>("/system_monitor/system_status", 1);

        _timerSystemStatus = this->create_wall_timer(500ms, std::bind(&SystemMonitor::publishSystemStatus, this));
    }
    void SystemMonitor::processNodeStatusCallback(const sonia_common_ros2::msg::NodeStatus &msg){
        
        if(std::find(_sources.begin(), _sources.end(), msg.node_name) != _sources.end()){
            _map_nodes[msg.node_name] = msg;

            /*for(const auto& [name, node] : _map_nodes){
                RCLCPP_INFO(this->get_logger(), "message: %s, quality: %d, state: %d,", name.c_str(), node.quality, node.state);
            }*/
        }            
    }

    void SystemMonitor::publishSystemStatus(){
        sonia_common_ros2::msg::SystemStatus _system_status;
        for(const auto& [name, node] : _map_nodes){
            _system_status.nodes.push_back(node);
        }  
        RCLCPP_INFO(this->get_logger(), "system_status_size: %d", _system_status.nodes.size());
        _system_status.stamp = this->get_clock().get()->now();
        _pub_system_status->publish(_system_status);
    }
} //namespace sonia_deploy