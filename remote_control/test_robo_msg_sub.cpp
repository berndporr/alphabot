// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
// Copyright 2025 Bernd Porr, mail@berndporr.me.uk
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

#include "brakePubSubTypes.h"
#include "steeringPubSubTypes.h"
#include "throttlePubSubTypes.h"

#include <chrono>
#include <thread>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

using namespace eprosima::fastdds::dds;

class RobotSubscriber
{
private:

    DomainParticipant* participant_ = nullptr;

    Subscriber* subscriber_ = nullptr;

    DataReader* readerSteering = nullptr;
    DataReader* readerBrake = nullptr;
    DataReader* readerThrottle = nullptr;

    Topic* topicSteering = nullptr;
    Topic* topicBrake = nullptr;
    Topic* topicThrottle = nullptr;

    TypeSupport typeSteering;
    TypeSupport typeBrake;
    TypeSupport typeThrottle;

    class SteeringListener : public DataReaderListener
    {
    public:
        SteeringListener() {}
        ~SteeringListener() override {}

        void on_subscription_matched(
	    DataReader*,
	    const SubscriptionMatchedStatus& info) override
	    {
		if (info.current_count_change == 1)
		    std::cout << "Steering subscriber matched." << std::endl;
		else if (info.current_count_change == -1)
		    std::cout << "Steering subscriber unmatched." << std::endl;
	    }

	// callback
        void on_data_available(DataReader* reader) override
	    {
		SampleInfo info;
		SteeringMsg msg;
		if (reader->take_next_sample(&msg, &info) == ReturnCode_t::RETCODE_OK)
		{
		    if (info.valid_data)
		    {
			std::cout << "Steering: " << msg.steering()
				  << " RECEIVED." << std::endl;
		    }
		}
	    }
    } listenerSteering;

    class ThrottleListener : public DataReaderListener
    {
    public:
        ThrottleListener() {}
        ~ThrottleListener() override {}

        void on_subscription_matched(
	    DataReader*,
	    const SubscriptionMatchedStatus& info) override
	    {
		if (info.current_count_change == 1)
		    std::cout << "Throttle subscriber matched." << std::endl;
		else if (info.current_count_change == -1)
		    std::cout << "Throttle subscriber unmatched." << std::endl;
	    }

	// callback
        void on_data_available(DataReader* reader) override
	    {
		SampleInfo info;
		SteeringMsg msg;
		if (reader->take_next_sample(&msg, &info) == ReturnCode_t::RETCODE_OK)
		{
		    if (info.valid_data)
		    {
			std::cout << "Throttle: " << msg.steering()
				  << " RECEIVED." << std::endl;
		    }
		}
	    }
    } listenerThrottle;

    class BrakeListener : public DataReaderListener
    {
    public:
        BrakeListener() {}
        ~BrakeListener() override {}

        void on_subscription_matched(
	    DataReader*,
	    const SubscriptionMatchedStatus& info) override
	    {
		if (info.current_count_change == 1)
		    std::cout << "Brake subscriber matched." << std::endl;
		else if (info.current_count_change == -1)
		    std::cout << "Brake subscriber unmatched." << std::endl;
	    }

	// callback
        void on_data_available(DataReader* reader) override
	    {
		SampleInfo info;
		SteeringMsg msg;
		if (reader->take_next_sample(&msg, &info) == ReturnCode_t::RETCODE_OK)
		{
		    if (info.valid_data)
		    {
			std::cout << "Brake: " << msg.steering()
				  << " RECEIVED." << std::endl;
		    }
		}
	    }
    } listenerBrake;

public:

    RobotSubscriber() : typeSteering(new SteeringMsgPubSubType()),
			typeBrake(new BrakeMsgPubSubType()),
			typeThrottle(new ThrottleMsgPubSubType()) {}

    virtual ~RobotSubscriber()
	{
	    if (readerSteering != nullptr) {
		subscriber_->delete_datareader(readerSteering);
	    }
	    if (topicSteering != nullptr) {
		participant_->delete_topic(topicSteering);
	    }
	    if (subscriber_ != nullptr) {
		participant_->delete_subscriber(subscriber_);
	    }
	    DomainParticipantFactory::get_instance()->delete_participant(participant_);
	}

    //!Initialize the subscriber
    bool init()
	{
	    DomainParticipantQos participantQos;
	    participantQos.name("Participant_subscriber");
	    participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

	    if (participant_ == nullptr)
	    {
		return false;
	    }

	    // Register the Types
	    typeSteering.register_type(participant_);
	    typeBrake.register_type(participant_);
	    typeThrottle.register_type(participant_);

	    // Create the Subscriber
	    subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
	    if (subscriber_ == nullptr) return false;

	    // Topic Steering
	    topicSteering = participant_->create_topic("SteeringTopic", "SteeringMsg", TOPIC_QOS_DEFAULT);
	    if (topicSteering == nullptr) return false;
	    readerSteering = subscriber_->create_datareader(topicSteering,
							    DATAREADER_QOS_DEFAULT,
							    &listenerSteering);
	    if (readerSteering == nullptr) return false;

	    // Topic Throttle
	    topicThrottle = participant_->create_topic("ThrottleTopic", "ThrottleMsg", TOPIC_QOS_DEFAULT);
	    if (topicThrottle == nullptr) return false;
	    readerThrottle = subscriber_->create_datareader(topicThrottle,
							    DATAREADER_QOS_DEFAULT,
							    &listenerThrottle);
	    if (readerThrottle == nullptr) return false;

	    // Topic Brake
	    topicBrake = participant_->create_topic("BrakeTopic", "BrakeMsg", TOPIC_QOS_DEFAULT);
	    if (topicBrake == nullptr) return false;
	    readerBrake = subscriber_->create_datareader(topicBrake,
							 DATAREADER_QOS_DEFAULT,
							 &listenerBrake);
	    if (readerBrake == nullptr) return false;

	    return true;
	}

};

int main(int,
	 char**)
{
    std::cout << "Starting subscriber." << std::endl;

    RobotSubscriber mysub;
    if(!mysub.init())
    {
	std::cerr << "Could not init the subscriber." << std::endl;
	return -1;
    }

    std::cout << "Press any key to stop it." << std::endl;
    // do nothing here
    getchar();

    return 0;
}
