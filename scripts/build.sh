#!/usr/bin/env bash

# Usage: ./scripts/build.sh [DOCKER_CI_DIR]

set -e
set -o pipefail

DOCKER_CI_DIR=$1

sudo apt update

$DOCKER_CI_DIR/scripts/build.sh sonia_common_ros2

cd sonia_monitor

source /build/sonia_common_ros2/INSTALL_BASE/setup.sh

colcon build --cmake-force-configure --install INSTALL_BASE