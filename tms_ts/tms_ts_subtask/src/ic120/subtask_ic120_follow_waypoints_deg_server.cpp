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

#include "tms_ts_subtask/ic120/subtask_ic120_follow_waypoints_deg.hpp"
// #include <glog/logging.h>

using std::placeholders::_1;
using std::placeholders::_2;

SubtaskIc120FollowWaypointsDeg::SubtaskIc120FollowWaypointsDeg() : SubtaskNodeBase("st_ic120_follow_waypoints_deg_node")
{
    this->action_server_ = rclcpp_action::create_server<tms_msg_ts::action::LeafNodeBase>(
        this, "st_ic120_follow_waypoints_deg",
        std::bind(&SubtaskIc120FollowWaypointsDeg::handle_goal, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&SubtaskIc120FollowWaypointsDeg::handle_cancel, this, std::placeholders::_1),
        std::bind(&SubtaskIc120FollowWaypointsDeg::handle_accepted, this, std::placeholders::_1));

    action_client_ = rclcpp_action::create_client<FollowWaypoints>(this, "/ic120/follow_waypoints");
}

rclcpp_action::GoalResponse SubtaskIc120FollowWaypointsDeg::handle_goal(
    const rclcpp_action::GoalUUID& uuid, std::shared_ptr<const tms_msg_ts::action::LeafNodeBase::Goal> goal)
{
    parameters = CustomGetParamFromDB<std::pair<std::string, std::string>, double>(goal->model_name, goal->record_name);
    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

rclcpp_action::CancelResponse SubtaskIc120FollowWaypointsDeg::handle_cancel(const std::shared_ptr<GoalHandle> goal_handle)
{
    RCLCPP_INFO(this->get_logger(), "Received request to cancel subtask node");
    if (client_future_goal_handle_.valid() &&
        client_future_goal_handle_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        auto goal_handle = client_future_goal_handle_.get();
        action_client_->async_cancel_goal(goal_handle);
    }
    return rclcpp_action::CancelResponse::ACCEPT;
}

void SubtaskIc120FollowWaypointsDeg::handle_accepted(const std::shared_ptr<GoalHandle> goal_handle)
{
    using namespace std::placeholders;
    std::thread{ std::bind(&SubtaskIc120FollowWaypointsDeg::execute, this, _1), goal_handle }.detach();
}

void SubtaskIc120FollowWaypointsDeg::execute(const std::shared_ptr<GoalHandle> goal_handle)
{
    RCLCPP_INFO(this->get_logger(), "subtask(st_ic120_follow_waypoints_node) is executing...");
    auto result = std::make_shared<tms_msg_ts::action::LeafNodeBase::Result>();
    auto handle_error = [&](const std::string& message) {
        if (goal_handle->is_active())
        {
        result->result = false;
        goal_handle->abort(result);
        RCLCPP_INFO(this->get_logger(), message.c_str());
        }
        else
        {
        RCLCPP_INFO(this->get_logger(), "Goal is not active");
        }
    };

    RCLCPP_INFO(this->get_logger(), "Get waypoints from DB.");

    std::vector<geometry_msgs::msg::PoseStamped> poses;
    auto goal_msg = FollowWaypoints::Goal();
    auto pose = geometry_msgs::msg::PoseStamped();
    int point_num = parameters.size() / 4;
    std::cout << "Total number of points: " << point_num << std::endl;
    pose.header.stamp = this->now();
    pose.header.frame_id = "map";

    for (int i=0; i < point_num; i++){
        pose.pose.position.x = parameters[std::make_pair("x", std::to_string(i))];
        pose.pose.position.y = parameters[std::make_pair("y", std::to_string(i))];
        pose.pose.position.z = parameters[std::make_pair("z", std::to_string(i))];

        double yaw_rad = parameters[std::make_pair("yaw", std::to_string(i))] * M_PI / 180.0;

        double roll_rad = 0.0;
        double pitch_rad = 0.0;

        double cy = cos(yaw_rad * 0.5);
        double sy = sin(yaw_rad * 0.5);
        double cp = cos(pitch_rad * 0.5);  // = 1.0
        double sp = sin(pitch_rad * 0.5);  // = 0.0
        double cr = cos(roll_rad * 0.5);   // = 1.0
        double sr = sin(roll_rad * 0.5);   // = 0.0

        pose.pose.orientation.w = cr * cp * cy + sr * sp * sy;
        pose.pose.orientation.x = sr * cp * cy - cr * sp * sy;  // = 0.0
        pose.pose.orientation.y = cr * sp * cy + sr * cp * sy;  // = 0.0
        pose.pose.orientation.z = cr * cp * sy - sr * sp * cy;
        poses.push_back(pose);
        std::cout << "Point " << i << ": " << pose.pose.position.x << ", " << pose.pose.position.y << ", " << pose.pose.position.z << std::endl;
        std::cout << "Pose " << i << ": " << pose.pose.orientation.x << ", " << pose.pose.orientation.y << ", " << pose.pose.orientation.z << ", " << pose.pose.orientation.w << std::endl;
    }

    // goal_msg.number_of_loops = 1;
    // goal_msg.goal_index = 0;
    goal_msg.poses = poses;

    auto send_goal_options = rclcpp_action::Client<FollowWaypoints>::SendGoalOptions();
    send_goal_options.goal_response_callback = [this](const auto& goal_handle) { goal_response_callback(goal_handle); };
    send_goal_options.feedback_callback = [this](const auto tmp, const auto feedback) {
        feedback_callback(tmp, feedback);
    };
    send_goal_options.result_callback = [this, goal_handle](const auto& result) { result_callback(goal_handle, result); };

    RCLCPP_INFO(this->get_logger(), "Sending goal");
    client_future_goal_handle_ = action_client_->async_send_goal(goal_msg, send_goal_options);
}

void SubtaskIc120FollowWaypointsDeg::goal_response_callback(const GoalHandleFollowWaypoints::SharedPtr& goal_handle)
{
  if (!goal_handle)
  {
    RCLCPP_ERROR(this->get_logger(), "Goal was rejected by server");
  }
  else
  {
    RCLCPP_INFO(this->get_logger(), "Goal accepted by server, waiting for result");
  }
}

void SubtaskIc120FollowWaypointsDeg::feedback_callback(
    const GoalHandleFollowWaypoints::SharedPtr,
    const std::shared_ptr<const GoalHandleFollowWaypoints::Feedback> feedback)
{
  RCLCPP_INFO(this->get_logger(), "Received feedback");
  // std::cout << "Feedback: " << feedback->current_waypoint << std::endl;
}

void SubtaskIc120FollowWaypointsDeg::result_callback(const std::shared_ptr<GoalHandle> goal_handle,
                                             const GoalHandleFollowWaypoints::WrappedResult& result)
{
  if (!goal_handle->is_active())
  {
    RCLCPP_WARN(this->get_logger(), "Attempted to succeed an already succeeded goal");
    return;
  }

  auto result_to_leaf = std::make_shared<tms_msg_ts::action::LeafNodeBase::Result>();
  switch (result.code)
  {
    case rclcpp_action::ResultCode::SUCCEEDED:
      result_to_leaf->result = true;
      goal_handle->succeed(result_to_leaf);
      RCLCPP_INFO(this->get_logger(), "Subtask execution is succeeded");
      break;
    case rclcpp_action::ResultCode::ABORTED:
      result_to_leaf->result = false;
      goal_handle->abort(result_to_leaf);
      RCLCPP_INFO(this->get_logger(), "Subtask execution is aborted");
      break;
    case rclcpp_action::ResultCode::CANCELED:
      result_to_leaf->result = false;
      goal_handle->canceled(result_to_leaf);
      RCLCPP_INFO(this->get_logger(), "Subtask execution is canceled");
      break;
    default:
      result_to_leaf->result = false;
      goal_handle->abort(result_to_leaf);
      RCLCPP_INFO(this->get_logger(), "Unknown result code");
      break;
  }
}

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SubtaskIc120FollowWaypointsDeg>());
    rclcpp::shutdown();
    return 0;
}
