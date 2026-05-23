#include "rclcpp/rclcpp.hpp"
#include "sonia_monitor/SystemMonitor.hpp"
#include "sonia_monitor/BagServer.hpp"
#include <cstdlib>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto monitor = std::make_shared<sonia_monitor::SystemMonitor>();
    auto bag = std::make_shared<sonia_monitor::BagServer>();

    auto executor = std::make_shared<rclcpp::executors::MultiThreadedExecutor>();
    bag->setExecutor(executor);
    
    executor->add_node(monitor);
    executor->add_node(bag);

    executor->spin();

    rclcpp::shutdown();
    return EXIT_SUCCESS;
}