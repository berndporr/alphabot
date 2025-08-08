cmake_minimum_required(VERSION 3.10)

project(Steering)

find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)

set (CMAKE_CXX_STANDARD 17)

# Create a library for the message / topic
add_custom_target(
    messages
    COMMAND fastddsgen brake.idl  steering.idl  throttle.idl
)

add_library(FastDDSMsg STATIC
  SteeringMsg.cxx SteeringMsgPubSubTypes.cxx
  BrakeMsg.cxx BrakeMsgPubSubTypes.cxx
  ThrottleMsg.cxx ThrottleMsgPubSubTypes.cxx
  )
  
add_dependencies(FastDDSMsg messages)


# Publisher
add_executable(remote remote.cpp)
target_link_libraries(remote fastcdr fastrtps FastDDSMsg)


# Subscriber
add_executable(robot robot.cpp)
target_link_libraries(robot fastcdr fastrtps FastDDSMsg)
