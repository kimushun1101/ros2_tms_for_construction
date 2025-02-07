// Copyright 2023, IRVS Laboratory, Kyushu University, Japan.

//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at

//      http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "tms_ts_subtask/ic120/subtask_ic120_release_soil.hpp"
#include <glog/logging.h>


using namespace std::chrono_literals;

SubtaskIc120ReleaseSoil::SubtaskIc120ReleaseSoil() : SubtaskNodeBase("st_ic120_release_soil_node")
{
  // publisher_ = this->create_publisher<std_msgs::msg::Float64>("/ic120/vessel/cmd", 10);
  client_ = this->create_client<com3_msgs::srv::DumpUp>("/ic120/vessel_target_pos");
  this->action_server_ = rclcpp_action::create_server<tms_msg_ts::action::LeafNodeBase>(
      this, "st_ic120_release_soil",
      std::bind(&SubtaskIc120ReleaseSoil::handle_goal, this, std::placeholders::_1, std::placeholders::_2),
      std::bind(&SubtaskIc120ReleaseSoil::handle_cancel, this, std::placeholders::_1),
      std::bind(&SubtaskIc120ReleaseSoil::handle_accepted, this, std::placeholders::_1));
}

rclcpp_action::GoalResponse SubtaskIc120ReleaseSoil::handle_goal(
    const rclcpp_action::GoalUUID& uuid, std::shared_ptr<const tms_msg_ts::action::LeafNodeBase::Goal> goal)
{
  parameters = CustomGetParamFromDB<std::string, double>(goal->model_name, goal->record_name);
  return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

rclcpp_action::CancelResponse SubtaskIc120ReleaseSoil::handle_cancel(const std::shared_ptr<GoalHandle> goal_handle)
{
  RCLCPP_INFO(this->get_logger(), "Received request to cancel subtask node");
  return rclcpp_action::CancelResponse::ACCEPT;
}

void SubtaskIc120ReleaseSoil::handle_accepted(const std::shared_ptr<GoalHandle> goal_handle)
{
  using namespace std::placeholders;
  std::thread{ std::bind(&SubtaskIc120ReleaseSoil::execute, this, _1), goal_handle }.detach();
}

void SubtaskIc120ReleaseSoil::execute(const std::shared_ptr<GoalHandle> goal_handle)
{
  RCLCPP_INFO(this->get_logger(), "subtask is executing...");
  rclcpp::Rate loop_rate(1);
  auto feedback = std::make_shared<tms_msg_ts::action::LeafNodeBase::Feedback>();
  // auto& status = feedback->status;
  auto result = std::make_shared<tms_msg_ts::action::LeafNodeBase::Result>();

  auto request = std::make_shared<com3_msgs::srv::DumpUp::Request>();
  request->vessel_angle = parameters["target_angle"];

  while (!client_->wait_for_service(20s))
  {
    if (!rclcpp::ok())
    {
      RCLCPP_ERROR(this->get_logger(), "Interrupted while waiting for the service. Exiting.");
      result->result = false;
      goal_handle->abort(result);
      return;
    }
    RCLCPP_INFO(this->get_logger(), "Waiting for service to become available...");
  }

  auto future = client_->async_send_request(request);

  if (future.wait_for(60s) == std::future_status::ready)
  {
    auto response = future.get();
    if (response->result)
    {
      result->result = true;
      goal_handle->succeed(result);
      RCLCPP_INFO(this->get_logger(), "subtask execution succeeded");
    }
    else
    {
      result->result = false;
      goal_handle->abort(result);
      RCLCPP_INFO(this->get_logger(), "subtask execution failed");
    }
  }
  else
  {
    result->result = false;
    goal_handle->abort(result);
    RCLCPP_ERROR(this->get_logger(), "Failed to call service.");
  }

}

int main(int argc, char* argv[])
{
  // Initialize Google's logging library.
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SubtaskIc120ReleaseSoil>());
  rclcpp::shutdown();
  return 0;
}
