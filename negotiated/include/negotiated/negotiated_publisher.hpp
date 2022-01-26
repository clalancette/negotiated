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

#ifndef NEGOTIATED__NEGOTIATED_PUBLISHER_HPP_
#define NEGOTIATED__NEGOTIATED_PUBLISHER_HPP_

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "rclcpp/rclcpp.hpp"

#include "negotiated_interfaces/msg/new_topic_info.hpp"
#include "negotiated_interfaces/msg/supported_type.hpp"
#include "negotiated_interfaces/msg/supported_types.hpp"

#include "negotiated/supported_type_map.hpp"

namespace negotiated
{

class NegotiatedPublisher
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(NegotiatedPublisher)

  explicit NegotiatedPublisher(
    rclcpp::Node::SharedPtr node,
    const SupportedTypeMap & supported_type_map,
    const std::string & topic_name,
    const rclcpp::QoS final_qos = rclcpp::QoS(10));

  template<typename MessageT>
  void publish(const MessageT & msg)
  {
    std::shared_ptr<rclcpp::SerializationBase> serializer = supported_type_map_.get_serializer(ros_type_name_);
    if (serializer == nullptr) {
      RCLCPP_INFO(node_->get_logger(), "Skipping publish since no registered type name");
      return;
    }
    rclcpp::SerializedMessage serialized_message;
    serializer->serialize_message(&msg, &serialized_message);
    publisher_->publish(serialized_message);
  }

private:
  void negotiate();

  rclcpp::Node::SharedPtr node_;
  SupportedTypeMap supported_type_map_;
  negotiated_interfaces::msg::SupportedTypes pub_supported_types_;
  std::string topic_name_;
  std::string ros_type_name_;
  rclcpp::QoS final_qos_;
  rclcpp::Publisher<negotiated_interfaces::msg::NewTopicInfo>::SharedPtr neg_publisher_;
  std::shared_ptr<rclcpp::GenericPublisher> publisher_;
  rclcpp::Subscription<negotiated_interfaces::msg::SupportedTypes>::SharedPtr supported_types_sub_;
  negotiated_interfaces::msg::SupportedTypes sub_supported_types_;
};

}  // namespace negotiated

#endif  // NEGOTIATED__NEGOTIATED_PUBLISHER_HPP_
