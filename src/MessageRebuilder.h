#ifndef _MessageRebuilder_H_
#define _MessageRebuilder_H_


#include "AgentAID.h"
#include "ACLMessage.h"
#include "ACLMessageOutputParser.h"
#include "UserdefParam.h"
#include "../../message-parser/src/grammar_bitefficient.h"

namespace fipa {
    
namespace acl {

class MessageRebuilder {

    private: static const std::string PredefMessageParams[];
    
    public:
        ACLMessage* buildMessage(Message parsedMsg);
        void buildParameters(std::vector<fipa::acl::MessageParameter> parsedParams,ACLMessage* msg);
        int buildPredefMessageParameters(MessageParameter param,ACLMessage* msg);
        void buildSender(MessageParameter param,ACLMessage* msg);
        void buildContent(MessageParameter param,ACLMessage* msg);
};

}
}
#endif