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

// GTest
#include <gtest/gtest.h>

// C++ Standard Library
#include <string>
#include <type_traits>
#include <tuple>

// ros1_bridge
#include "ros1_bridge/factory.hpp"

// RCLCPP
#include "rclcpp/serialized_message.hpp"

// ROS1 Messsages
#include <geometry_msgs/PoseArray.h>
#include <geometry_msgs/Vector3.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/Imu.h>
#include <std_msgs/Header.h>
#include <std_msgs/String.h>
#include <std_msgs/Time.h>

// ROS2 Messsages
#include <geometry_msgs/msg/pose_array.hpp>
#include <geometry_msgs/msg/vector3.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <std_msgs/msg/header.hpp>
#include <std_msgs/msg/string.hpp>


template<typename ROS1_T_, typename ROS2_T_>
struct GenericTestBase
{
  using ROS1_T = ROS1_T_;
  using ROS2_T = ROS2_T_;
  using FACTORY_T = ros1_bridge::Factory<ROS1_T, ROS2_T>;

  FACTORY_T factory;
  ROS1_T ros1_msg;
  ROS2_T ros2_msg;
  GenericTestBase(const std::string & ros1_type_name, const std::string & ros2_type_name) :
    factory(ros1_type_name, ros2_type_name) { }
};

struct Vector3Test : public GenericTestBase<geometry_msgs::Vector3, geometry_msgs::msg::Vector3>
{
  Vector3Test() : GenericTestBase("geometry_msgs/Vector3", "geometry_msgs/msg/Vector3")
  {
    ros1_msg.x = ros2_msg.x = 1.1;
    ros1_msg.y = ros2_msg.y = 2.2;
    ros1_msg.z = ros2_msg.z = 3.3;
  }
};


struct StringTestEmpty : public GenericTestBase<std_msgs::String,
                                                std_msgs::msg::String>
{
  StringTestEmpty() : GenericTestBase("std_msgs/String", "std_msgs/msg/String") { }
};


struct StringTestHello : public StringTestEmpty
{
  StringTestHello()
  {
    ros1_msg.data = ros2_msg.data = "hello";
  }
};


template <typename TEST_T_>
class ConvertGenericTest : public testing::Test {
public:
  using TEST_T = TEST_T_;
  using ROS1_T = typename TEST_T::ROS1_T;
  using ROS2_T = typename TEST_T::ROS2_T;
  using FACTORY_T = typename TEST_T::FACTORY_T;

  TEST_T test;

  void TestBody()
  {
    // nothing
  }
};


using ConvertGenericTypes = ::testing::Types<
  Vector3Test,  // 0
  StringTestEmpty,  // 1
  StringTestHello  // 2
  >;
TYPED_TEST_SUITE(ConvertGenericTest, ConvertGenericTypes);


// cppcheck-suppress syntaxError
TYPED_TEST(ConvertGenericTest, test_factory_md5)
{
  // Make sure md5sum is available and matches ROS1_T data type
  TestFixture fixture;
  //using TestBase = TestFixture::TYPE;
  //type TestFixture::TYPE
  using ROS1_T = typename TestFixture::ROS1_T;
  EXPECT_EQ(std::string(fixture.test.factory.get_ros1_md5sum()),
            std::string(ros::message_traits::MD5Sum<ROS1_T>::value()));
}


// cppcheck-suppress syntaxError
TYPED_TEST(ConvertGenericTest, test_convert_2_to_1)
{
  // ros2/rclcpp/rclcpp/src/rclcpp/serialized_message.cpp
  rclcpp::SerializedMessage serialized_msg;
  const auto& ros2_msg = this->test.ros2_msg;
  auto ret = rmw_serialize(&ros2_msg, this->test.factory.type_support_,
                           &serialized_msg.get_rcl_serialized_message());
  EXPECT_EQ(RMW_RET_OK, ret);

  topic_tools::ShapeShifter shape_shifter;
  const bool latched = false;
  bool success = this->test.factory.convert_2_to_1_generic(serialized_msg, shape_shifter, latched);
  ASSERT_TRUE(success);

  // Shape shifter's internal buffer is private, so access it by writing it into a stream
  const uint32_t length2 = shape_shifter.size();
  std::vector<uint8_t> buffer2(length2);
  ros::serialization::OStream out_stream2(buffer2.data(), length2);
  shape_shifter.write(out_stream2);

  // Write ROS1 message (which should have same feild values as ROS2 message) in a different stream
  using ROS1_T = typename TestFixture::ROS1_T;
  const ROS1_T& ros1_msg = this->test.ros1_msg;
  const uint32_t length1 = ros::serialization::serializationLength(ros1_msg);
  std::vector<uint8_t> buffer1(length1);
  ros::serialization::OStream out_stream1(buffer1.data(), length1);
  ros::serialization::serialize(out_stream1, ros1_msg);

  // Buffer1 and Buffer2 should match in size and contents
  ASSERT_EQ(length1, length2);

  // The Gtest output from comparing buffers directly is a little hard to
  // understand when there is a few mismatching value
  // Instead use custom loop to make each mismatched byte easier to understand
  // ASSERT_EQ(buffer1, buffer2);
  unsigned mismatch_count = 0;
  const unsigned mismatch_count_limit = 10;
  for (size_t idx = 0; idx < length1; ++idx)
  {
    int val1 = buffer1.at(idx);
    int val2 = buffer2.at(idx);
    EXPECT_EQ(val1, val2) << " idx=" << idx;
    if (val1 != val2)
    {
      ++mismatch_count;
    }
    ASSERT_LE(mismatch_count, mismatch_count_limit) << " stopping comparison after " << mismatch_count_limit << " mismatches";
  }
  ASSERT_EQ(mismatch_count, 0u) << " the output buffers should be exactly the same";
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
