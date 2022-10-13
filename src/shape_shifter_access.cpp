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


// topic_tools::ShapeShifter does allow access to data buffer
// Unfortunately this means, we can't access data without performing a potentially very
// expensize message by serializing it to a stream
// Since ROS1 is no longer in developement adding a new accessor function is not possible.
// However since ShapeShifter will not change, we can include it by redefining the meaning
// of private

// Include everything that shape_shifter.h does
// This will define header include guards so when shape shifter is included,
// their definitions don't change and cause odd problems.
#include "ros/ros.h"
#include "ros/console.h"
#include "ros/assert.h"
#include <vector>
#include <string>
#include <string.h>
#include <ros/message_traits.h>
#include "topic_tools/macros.h"

// Redefine the meaning of private for just new stuff in shape_shifter header
#define private public
#include <topic_tools/shape_shifter.h>
#undef private

#include "ros1_bridge/shape_shifter_access.hpp"

namespace ros1_bridge
{

const uint8_t* get_data(const topic_tools::ShapeShifter& shape_shifter)
{
  return shape_shifter.msgBuf.data();
}

uint8_t* get_data(topic_tools::ShapeShifter& shape_shifter)
{
  return shape_shifter.msgBuf.data();
}

}  // namespace ros1_bridge
