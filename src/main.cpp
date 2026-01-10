#include "rclcpp/rclcpp.hpp"
#include "sonia_deploy/SystemMonitor.hpp"
#include <cstdlib>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto monitor = std::make_shared<sonia_deploy::SystemMonitor>();

    rclcpp::spin(monitor);

    rclcpp::shutdown();
    return EXIT_SUCCESS;
}