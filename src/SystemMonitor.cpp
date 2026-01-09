#include "sonia_deploy/SystemMonitor.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;
namespace sonia_deploy
{
    SystemMonitor::SystemMonitor()
    : Node("system_monitor"){

        //Generate list of node names to be monitored from a config file
        this->declare_parameter("node_list", rclcpp::PARAMETER_STRING_ARRAY);
        _sources = this->get_parameter("node_list").as_string_array();

        //publishers
        _sub_node_status = this->create_subscription<sonia_common_ros2::msg::NodeStatus>("/system_monitor/node_status", 1, std::bind(&SystemMonitor::processNodeStatusCallback,this,_1));
        _pub_system_status = this->create_publisher<sonia_common_ros2::msg::SystemStatus>("/system_monitor/system_status", 1);

        //wall timer to constatly publish system information
        _timerSystemStatus = this->create_wall_timer(500ms, std::bind(&SystemMonitor::publishSystemStatus, this));

        //initialize all monitored nodes on start
        for(const std::string& name : _sources){
            initializeNode(name);
        }
    }
    void SystemMonitor::processNodeStatusCallback(const sonia_common_ros2::msg::NodeStatus &msg){
        
        if(std::find(_sources.begin(), _sources.end(), msg.node_name) != _sources.end()){
            _mapped_nodes[msg.node_name].recieved_stamp = this->now();
            _mapped_nodes[msg.node_name].node_status = msg;

            checkConsistency(msg.node_name);
        }
    }

    void SystemMonitor::publishSystemStatus(){
        sonia_common_ros2::msg::SystemStatus _system_status;
        const auto timeout = rclcpp::Duration::from_seconds(1.5);
        const auto temp = _mapped_nodes;

        for(const auto& [name, monitored_node] : temp){
            if(monitored_node.recieved_stamp.nanoseconds() != 0){
                const auto duration = this->now()-monitored_node.recieved_stamp;
                if(duration > timeout)
                    initializeNode(name);
            }

            _system_status.nodes.push_back(monitored_node.node_status);
        }
        
        _system_status.stamp = this->now();
        _pub_system_status->publish(_system_status);
    }

    void SystemMonitor::initializeNode(const std::string node_name){
        
        sonia_common_ros2::msg::NodeStatus node;

        node.node_name = node_name;
        node.stamp = this->now();
        node.state = sonia_common_ros2::msg::NodeStatus::STATE_STOPPED;
        node.quality = sonia_common_ros2::msg::NodeStatus::Q_UNKNOWN;

        _mapped_nodes[node_name].node_status = node;
    }

    void SystemMonitor::checkConsistency(const std::string node_name){
        if(_mapped_nodes[node_name].last_published_stamp.nanoseconds() == 0){
            return;
        }

        const auto actual = rclcpp::Time(_mapped_nodes[node_name].node_status.stamp) - _mapped_nodes[node_name].last_published_stamp;
        const auto tolerance = EXPECTED_RATE*0.2;
        
        if (actual < (EXPECTED_RATE - tolerance) || actual > EXPECTED_RATE + tolerance){
            _mapped_nodes[node_name].node_status.quality = sonia_common_ros2::msg::NodeStatus::Q_DEGRADE;
            _mapped_nodes[node_name].last_published_stamp = _mapped_nodes[node_name].node_status.stamp;
        }
        
    }
} //namespace sonia_deploy