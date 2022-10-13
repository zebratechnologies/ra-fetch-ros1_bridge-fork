// Copyright 2022 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef  ROS1_BRIDGE__SHAPE_SHIFTER_ACCESS_HPP_
#define  ROS1_BRIDGE__SHAPE_SHIFTER_ACCESS_HPP_

namespace topic_tools
{
class ShapeShifter;
}

namespace ros1_bridge
{

/**
 * @brief Return const pointer to shape shifter message buffer
 */
const uint8_t* get_data(const topic_tools::ShapeShifter& shape_shifter);

/**
 * @brief Return mutable pointer to shape shifter message buffer
 */
uint8_t* get_data(topic_tools::ShapeShifter& shape_shifter);

}  // namespace ros1_bridge

#endif  // ROS1_BRIDGE__SHAPE_SHIFTER_ACCESS_HPP_
