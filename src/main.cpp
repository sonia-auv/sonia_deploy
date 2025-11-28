#include "rclcpp/rclcpp.hpp"
#include "sonia_deploy/SystemMonitor.hpp"
#include <cstdlib>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto system = std::make_shared<sonia_deploy::SystemMonitor>();

    rclcpp::spin(system);

    rclcpp::shutdown();
    return EXIT_SUCCESS;
}