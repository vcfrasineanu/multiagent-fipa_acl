#include "acl_envelope.h"

#include <algorithm>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <fipa_acl/message_parser/message_parser.h>
#include <fipa_acl/message_generator/message_generator.h>

using namespace boost::phoenix;
using namespace boost::phoenix::arg_names;
using namespace std;
using namespace fipa::acl::envelope;

namespace fipa {
namespace acl {

ACLBaseEnvelope::ACLBaseEnvelope()
    : mParameters(NONE)
    , mACLRepresentation(representation::UNKNOWN)
    , mPayloadEncoding("US-ASCII")
{}

void ACLBaseEnvelope::addTo(const AgentID& agentId)
{
    AgentID agent = agentId;
    AgentIDList::const_iterator it = find_if(mTo.begin(), mTo.end(), arg1 == agentId );
    if(it == mTo.end())
    {
        mTo.push_back(agentId);
    }
}

void ACLBaseEnvelope::setTo(const AgentIDList& receivers)
{
    mParameters = TO;
    mTo = receivers;
}

bool ACLBaseEnvelope::removeTo(const AgentID& agentId)
{
    AgentIDList::iterator it = find_if(mTo.begin(), mTo.end(), arg1 == agentId);
    if(it != mTo.end())
    {
        mTo.erase(it);
        return true;
    }
    return false;
}


void ACLBaseEnvelope::setFrom(const AgentID& from)
{
    mParameters = (ParameterId) (mParameters | FROM);
    mFrom = from;
}


void ACLBaseEnvelope::setComments(const Comments& comments)
{
    mParameters = (ParameterId) (mParameters | COMMENTS);
    mComments = comments;
}

void ACLBaseEnvelope::setACLRepresentation(representation::Type representation)
{
    mParameters = (ParameterId) (mParameters | ACL_REPRESENTATION);
    mACLRepresentation = representation;
}

void ACLBaseEnvelope::setPayloadLength(const PayloadLength& length)
{
    mParameters = (ParameterId) (mParameters | PAYLOAD_LENGTH);
    mPayloadLength = length;
}

void ACLBaseEnvelope::setPayloadEncoding(const PayloadEncoding& encoding)
{
    mParameters = (ParameterId) (mParameters | PAYLOAD_ENCODING);
    mPayloadEncoding = encoding;
}

void ACLBaseEnvelope::setDate(const base::Time& date)
{
    mParameters = (ParameterId) (mParameters | DATE);
    mDate = date;
}

void ACLBaseEnvelope::setIntendedReceivers(const AgentIDList& receivers)
{
    mParameters = (ParameterId) (mParameters | INTENDED_RECEIVERS);
    mIntendedReceivers = receivers;
}

void ACLBaseEnvelope::addIntendedReceiver(const AgentID& agentId)
{
     AgentIDList::const_iterator it = find_if(mIntendedReceivers.begin(), mIntendedReceivers.end(), arg1 == agentId);
     if(it == mIntendedReceivers.end())
     {
         mIntendedReceivers.push_back(agentId);
     }
}

bool ACLBaseEnvelope::removeIntendedReceiver(const AgentID& agentId)
{
     AgentIDList::iterator it = find_if(mIntendedReceivers.begin(), mIntendedReceivers.end(), arg1 == agentId);
     if(it != mIntendedReceivers.end())
     {
         mIntendedReceivers.erase(it);
         return true;
     }
     return false;
}

void ACLBaseEnvelope::setReceivedObject(const ReceivedObject& receivedObject)
{
    mParameters = (ParameterId) (mParameters | RECEIVED_OBJECT);
    mReceivedObject = receivedObject;
}

bool ACLBaseEnvelope::hasReceivedObject(const ReceivedObject& receivedObject) const
{
    if (mParameters & RECEIVED_OBJECT)
    {
        return receivedObject.getBy() == mReceivedObject.getBy();
    }

    return false;
}

void ACLBaseEnvelope::setTransportBehaviour(const TransportBehaviour& transportBehaviour)
{
    mParameters = (ParameterId) (mParameters | TRANSPORT_BEHAVIOUR);
    mTransportBehaviour = transportBehaviour;
}

void ACLBaseEnvelope::setUserdefinedParameters(const UserdefinedParameterList& parameters)
{
    mParameters = (ParameterId) (mParameters | USERDEFINED_PARAMETERS);
    mUserdefinedParameters = parameters;
}

ACLBaseEnvelope ACLBaseEnvelope::merge(const ACLBaseEnvelope& other) const
{
    ACLBaseEnvelope envelope(*this);
    if(!envelope.contains(TO))
    {
        envelope.setTo(other.getTo());
    }

    if(!envelope.contains(FROM))
    {
        envelope.setFrom(other.getFrom());
    }

    if(!envelope.contains(COMMENTS))
    {
        envelope.setComments(other.getComments());
    }

    if(!envelope.contains(ACL_REPRESENTATION))
    {
        envelope.setACLRepresentation(other.getACLRepresentation());
    }

    if(!envelope.contains(PAYLOAD_LENGTH))
    {
        envelope.setPayloadLength(other.getPayloadLength());
    }

    if(!envelope.contains(PAYLOAD_ENCODING))
    {
        envelope.setPayloadEncoding(other.getPayloadEncoding());
    }

    if(!envelope.contains(PAYLOAD_ENCODING))
    {
        envelope.setDate(other.getDate());
    }

    if(!envelope.contains(INTENDED_RECEIVERS))
    {
        envelope.setIntendedReceivers(other.getIntendedReceivers());
    }

    if(!envelope.contains(RECEIVED_OBJECT))
    {
        envelope.setReceivedObject(other.getReceivedObject());
    }

    if(!envelope.contains(TRANSPORT_BEHAVIOUR))
    {
        envelope.setTransportBehaviour(other.getTransportBehaviour());
    }

    if(!envelope.contains(USERDEFINED_PARAMETERS))
    {
        envelope.setUserdefinedParameters(other.getUserdefinedParameters());
    }

    return envelope;
}

ACLBaseEnvelope ACLBaseEnvelope::flatten(const ACLBaseEnvelopeList& extraEnvelopes) const
{
    ACLBaseEnvelope envelope = *this;
    ACLBaseEnvelopeList::const_iterator cit = extraEnvelopes.begin();
    for(; cit != extraEnvelopes.end(); ++cit)
    {
        envelope.merge(*cit);
    }

    return envelope;
}

ACLEnvelope::ACLEnvelope()
{}

ACLEnvelope::ACLEnvelope(const fipa::acl::ACLMessage& message, const fipa::acl::representation::Type& representation)
{
    using namespace fipa::acl;
    mPayload = MessageGenerator::create(message, representation);
}

fipa::acl::ACLMessage ACLEnvelope::getACLMessage() const
{
    using namespace fipa::acl;
    ACLMessage msg;
    MessageParser mp;
    mp.parseData(mPayload, msg);
    return msg;
}

void ACLEnvelope::stamp(const fipa::acl::AgentID& id)
{
    ReceivedObject receivedObject;

    // Mandatory fields
    receivedObject.setBy(id.getName());
    receivedObject.setDate(base::Time::now());

    ACLBaseEnvelope extraEnvelope;
    extraEnvelope.stamp(receivedObject);

    mExtraEnvelopes.push_back(extraEnvelope);
}

bool ACLEnvelope::hasStamp(const fipa::acl::AgentID id) const
{
    ReceivedObject receivedObject;
    receivedObject.setBy(id.getName());

    ACLBaseEnvelopeList::const_iterator cit = mExtraEnvelopes.begin();
    for(; cit != mExtraEnvelopes.end(); ++cit)
    {
        if(cit->hasStamp(receivedObject))
        {
            return true;
        }
    }
    return false;
}


} // end namespace acl
} // end namespace fipa