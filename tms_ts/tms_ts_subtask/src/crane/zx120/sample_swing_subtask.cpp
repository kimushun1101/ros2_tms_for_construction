// Copyright 2023, IRVS Laboratory, Kyushu University, Japan.
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
#include <thread>
#include "std_msgs/msg/float64.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "tms_msg_ts/action/leaf_node_base.hpp"
#include "tms_ts_subtask/subtask_node_base.hpp"

#include <glog/logging.h>

using namespace std::chrono_literals;

class SubtaskSampleZx120Swing : public SubtaskNodeBase
{
public:
  using GoalHandle = rclcpp_action::ServerGoalHandle<tms_msg_ts::action::LeafNodeBase>;


  SubtaskSampleZx120Swing(): SubtaskNodeBase("subtask_sample_zx120_swing")
  {
    publisher_ = this->create_publisher<std_msgs::msg::Float64>("/zx120/swing/cmd",10);
    this->action_server_ = rclcpp_action::create_server<tms_msg_ts::action::LeafNodeBase>(
      this,
      "sample_zx120_swing",
      std::bind(&SubtaskSampleZx120Swing::handle_goal, this,std::placeholders::_1, std::placeholders::_2),
      std::bind(&SubtaskSampleZx120Swing::handle_cancel, this, std::placeholders::_1),
      std::bind(&SubtaskSampleZx120Swing::handle_accepted, this, std::placeholders::_1));
  }

private:
  rclcpp_action::Server<tms_msg_ts::action::LeafNodeBase>::SharedPtr action_server_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher_;
  std::map<std::string, int> parameters;

  rclcpp_action::GoalResponse handle_goal(const rclcpp_action::GoalUUID & uuid, std::shared_ptr<const tms_msg_ts::action::LeafNodeBase::Goal> goal)
  {
    RCLCPP_INFO(this->get_logger(), "Received goal request with parameter_id = %s", goal->parameter_id.c_str());
    parameters = GetParamFromDB(goal->parameter_id);
    RCLCPP_INFO(this->get_logger(), "param = %d", parameters["zx120_swing_goal_pos"]);
    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<GoalHandle> goal_handle)
  {
    RCLCPP_INFO(this->get_logger(), "Received request to cancel subtask node");
    sleep(5);
    return rclcpp_action::CancelResponse::ACCEPT;
  }

  void handle_accepted(const std::shared_ptr<GoalHandle> goal_handle)
  {
    using namespace std::placeholders;
    std::thread{std::bind(&SubtaskSampleZx120Swing::execute, this, _1), goal_handle}.detach();
  }

  void execute(const std::shared_ptr<GoalHandle> goal_handle)
  {
    RCLCPP_INFO(this->get_logger(), "subtask is executing...");
    rclcpp::Rate loop_rate(1);
    const auto goal_pos = parameters["zx120_swing_goal_pos"];
    auto feedback = std::make_shared<tms_msg_ts::action::LeafNodeBase::Feedback>();
    auto & current_pos = feedback->current_pos;
    auto result = std::make_shared<tms_msg_ts::action::LeafNodeBase::Result>();
    int deg = 0;
    std_msgs::msg::Float64 msg_rad;

    while(deg <= goal_pos){
        if (goal_handle->is_canceling()) {
            result->result = false;
            goal_handle->canceled(result);
            RCLCPP_INFO(this->get_logger(), "subtask execution is canceled");
            return;
        }
        deg += goal_pos / float(20.0);
        msg_rad.data = float(deg * float(M_PI / 180));
        current_pos = deg;
        goal_handle->publish_feedback(feedback); 
        publisher_->publish(msg_rad);
        RCLCPP_INFO_STREAM(this->get_logger(), "Publishing swing position: " << deg << " [deg]");
        sleep(1); 
    }

    if(rclcpp::ok()){
        result->result = true;
        goal_handle->succeed(result);
        RCLCPP_INFO(this->get_logger(), "subtask execution is succeeded");
    }
  }
};

int main(int argc, char *argv[])
{
	// Initialize Google's logging library.
	google::InitGoogleLogging(argv[0]);
	google::InstallFailureSignalHandler();
  
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SubtaskSampleZx120Swing>());
  rclcpp::shutdown();
  return 0;
}




