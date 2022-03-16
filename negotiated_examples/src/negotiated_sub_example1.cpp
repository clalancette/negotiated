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

#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_components/register_node_macro.hpp"
#include "std_msgs/msg/int32.hpp"
#include "std_msgs/msg/string.hpp"

#include "negotiated/negotiated_subscription.hpp"

#include "example_type_info.hpp"

namespace negotiated_examples
{

class NegotiatedSubExample1 final : public rclcpp::Node
{
public:
  explicit NegotiatedSubExample1(const rclcpp::NodeOptions & options)
  : rclcpp::Node("negotiated_sub_example1", options)
  {
    bool use_intra_process = this->declare_parameter("use_intra_process", false);
    double string_a_weight = this->declare_parameter("string_a_weight", 1.0);
    double int32_weight = this->declare_parameter("int32_weight", 0.5);

    auto string_user_cb = [this](const std_msgs::msg::String & msg)
      {
        RCLCPP_INFO(this->get_logger(), "String user callback: %s", msg.data.c_str());
      };

    auto int_user_cb = [this](const std_msgs::msg::Int32 & msg)
      {
        RCLCPP_INFO(this->get_logger(), "Int user callback: %d", msg.data);
      };

    negotiated_sub_ = std::make_shared<negotiated::NegotiatedSubscription>(*this, "example");

    rclcpp::SubscriptionOptions sub_options;
    if (use_intra_process) {
      sub_options.use_intra_process_comm = rclcpp::IntraProcessSetting::Enable;
    }

    negotiated_sub_->add_supported_callback<negotiated_examples::StringT>(
      string_a_weight,
      rclcpp::QoS(1),
      string_user_cb,
      sub_options);
    negotiated_sub_->add_supported_callback<negotiated_examples::Int32T>(
      int32_weight,
      rclcpp::QoS(1),
      int_user_cb,
      sub_options);
    negotiated_sub_->start();
  }

private:
  std::shared_ptr<negotiated::NegotiatedSubscription> negotiated_sub_;
};

}  // namespace negotiated_examples

RCLCPP_COMPONENTS_REGISTER_NODE(negotiated_examples::NegotiatedSubExample1)
