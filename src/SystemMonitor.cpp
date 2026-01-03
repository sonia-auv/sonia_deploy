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

        for(const std::string name : _sources){
            initializeNode(name);
        }
    }
    void SystemMonitor::processNodeStatusCallback(const sonia_common_ros2::msg::NodeStatus &msg){
        
        if(std::find(_sources.begin(), _sources.end(), msg.node_name) != _sources.end()){
            _map_nodes[msg.node_name] = msg;
        }            
    }

    void SystemMonitor::publishSystemStatus(){
        sonia_common_ros2::msg::SystemStatus _system_status;
        const auto temp = _map_nodes;

        for(const auto& [name, node] : temp){
            _system_status.nodes.push_back(node);
        }
        
        _system_status.stamp = this->get_clock().get()->now();
        _pub_system_status->publish(_system_status);
    }

    void SystemMonitor::initializeNode(const std::string node_name){
        
        sonia_common_ros2::msg::NodeStatus node;

        node.node_name = node_name;
        node.stamp = this->get_clock().get()->now();
        node.state = sonia_common_ros2::msg::NodeStatus::STATE_STOPPED;
        node.quality = sonia_common_ros2::msg::NodeStatus::Q_UNKNOWN;

        _map_nodes[node_name] = node;
    }
} //namespace sonia_deploy