/**
 * \file statemachine_reader.h
 * \author Mircea Cretu Stancu
 * \author Thomas Roehr, thomas.roehr@dfki.de
 * \brief defines the StateMachineReader Class, used to generate a state macine from a spec. file
 */

#ifndef FIPAACL_CONVERSATIONMONITOR_STATEMACHINE_READER_H
#define FIPAACL_CONVERSATIONMONITOR_STATEMACHINE_READER_H

#include <fipa_acl/conversation_monitor/statemachine.h>
#include <fipa_acl/conversation_monitor/state.h>
#include <vector>

class TiXmlElement;

namespace fipa {
namespace acl {

/**
    \class StateMachineReader
    \brief implements a XML parser that generates a state machine from a given spec. file; uses the tinyXML library
*/
class StateMachineReader {
        
public:
    // node attribute names used in the spec standard; most general ones such as "state", "transition" are hard-implement
    static const std::string transition;
    static const std::string from;
    static const std::string to;
    static const std::string target;
    static const std::string id;
    static const std::string final;
    static const std::string performative;
    static const std::string initial;
    static const std::string subprotocol;
    static const std::string mapping;
    static const std::string name;
    static const std::string proxiedTo;

    /**
     * Read the xml structure defining a statemachine
     * \param statemachineElement the element describing the stateMachine
     * \param protocolSpec name of the file containing the specification for the
     * statemachine
     * \return the parsed state machine
     */
    StateMachine parseStateMachineNode(TiXmlElement* statemachineElement);
    
    /**
        \brief method that parses the <state> element of the spec. file
        \param stateElement
        \return the parsed state
    */
    State parseStateNode(TiXmlElement* stateElement);
    
    /**
        \brief method that parses <transition> element of the spec. file
        \param transitionElement
        \return transition 
    */
    Transition parseTransitionNode(TiXmlElement* transitionElement);
        
public:
    /**
     * Load the specification and return the corresponding state
     * machine for it
     * \param file name of the file containing the specification for the
     * statemachine
     * \return the statemachine build from spec
     */
    StateMachine loadSpecification(const std::string& file);
    
};


} //end of acl
} //end of fipa

#endif
