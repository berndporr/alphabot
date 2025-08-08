// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
// Copyright 2025 Bernd Porr
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

/**
 * @file HelloWorldPublisher.cpp
 *
 */

#include "brakePubSubTypes.h"
#include "steeringPubSubTypes.h"
#include "throttlePubSubTypes.h"

#include <chrono>
#include <thread>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

#include "logiwheel.h"

using namespace eprosima::fastdds::dds;

class RemotePublisher
{
private:

    DomainParticipant* participant_ = nullptr;

    Publisher* publisher_ = nullptr;

    Topic* topicSteering = nullptr;
    Topic* topicBraking = nullptr;
    Topic* topicThrottle = nullptr;

    DataWriter* writerSteering = nullptr;
    DataWriter* writerBraking = nullptr;
    DataWriter* writerThrottle = nullptr;

    TypeSupport type_;

    class PubListener : public DataWriterListener
    {
    public:

        PubListener()
            : matched_(0)
        {
        }

        ~PubListener() override
        {
        }

        void on_publication_matched(
                DataWriter*,
                const PublicationMatchedStatus& info) override
        {
            if (info.current_count_change == 1)
            {
                matched_ = info.total_count;
                std::cout << "Publisher matched." << std::endl;
            }
            else if (info.current_count_change == -1)
            {
                matched_ = info.total_count;
                std::cout << "Publisher unmatched." << std::endl;
            }
            else
            {
                std::cout << info.current_count_change
                        << " is not a valid value for PublicationMatchedStatus current count change." << std::endl;
            }
        }

        std::atomic_int matched_;

    } listener_;

public:

    RemotePublisher() : type_(new SteeringMsgPubSubType()) {}

    virtual ~RemotePublisher()
    {
        if (writerSteering != nullptr)
        {
            publisher_->delete_datawriter(writerSteering);
        }
        if (publisher_ != nullptr)
        {
            participant_->delete_publisher(publisher_);
        }
        if (topicSteering != nullptr)
        {
            participant_->delete_topic(topicSteering);
        }
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

    //!Initialize the publisher
    bool init()
    {
        DomainParticipantQos participantQos;
        participantQos.name("Participant_publisher");
        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

        if (participant_ == nullptr)
        {
            return false;
        }

        // Register the Type
        type_.register_type(participant_);

        // Create the Publisher
        publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);
        if (publisher_ == nullptr)
        {
            return false;
        }

        // Create the publications Topic
        topicSteering = participant_->create_topic("SteeringTopic", "SteeringMsg", TOPIC_QOS_DEFAULT);
        if (topicSteering == nullptr) {
            return false;
        }

        // Create the DataWriter
        writerSteering = publisher_->create_datawriter(topicSteering, DATAWRITER_QOS_DEFAULT, &listener_);

        if (writerSteering == nullptr)
        {
            return false;
        }
        return true;
    }

    //!Send a publication
    bool publishSteering(SteeringMsg& msg)
    {
        if (listener_.matched_ > 0)
        {
            writerSteering->write(&msg);
            return true;
        }
        return false;
    }
};


int main(int argc, char *argv[]) {
	LogiWheel logiwheel;
	RemotePublisher remotePublisher;
	logiwheel.registerSteeringCallback([&](float v){
	    SteeringMsg msg;
	    msg.steering(v);
	    if (remotePublisher.publishSteering(msg)) {
		std::cout << v << " SENT" << std::endl;
	    } else {
		std::cout << "No messages sent as there is no listener." << std::endl;
	    }	    
	});
	if(!remotePublisher.init())
	{
	    std::cerr << "Pub not init'd." << std::endl;
	    return -1;
	}
	logiwheel.start();
	printf("Press any key to stop it.\n");
	getc(stdin);
	logiwheel.stop();
	return 0;
}
