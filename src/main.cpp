#include "rclcpp/rclcpp.hpp"
#include "sonia_deploy/SystemMonitor.hpp"
#include "sonia_deploy/BagServer.hpp"
#include <cstdlib>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto monitor = std::make_shared<sonia_deploy::SystemMonitor>();
    auto bag = std::make_shared<sonia_deploy::BagServer>();

    auto executor = std::make_shared<rclcpp::executors::MultiThreadedExecutor>();
    bag->setExecutor(executor);
    
    executor->add_node(monitor);
    executor->add_node(bag);

    executor->spin();

    rclcpp::shutdown();
    return EXIT_SUCCESS;
}