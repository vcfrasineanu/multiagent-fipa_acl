/**
 *
 * \file transition.h
 * \author Mircea Cretu Stancu
 * \brief describes the structure and operation of a transition of the state machine
 * 
 * \version 1.0
 */

#ifndef _FIPAACL_CONVMONITOR_TRANSITION_H_
#define _FIPAACL_CONVMONITOR_TRANSITION_H_

#include <fipa_acl/conversation_monitor/types.h>
#include <fipa_acl/bitefficient_message.h>
#include <algorithm>

namespace fipa {
namespace acl {
    /** \brief forward declaration of the StateMachine class needed to solve a circular dependancy */
    class StateMachine;
    /** \brief forward declaration of the State class needed to solve a circular dependency */
    class State;
    
class Transition 
{
    private:
        /** \param from string definig the role of the agent expected to be the sender of a message for this transition */
        std::string from;
        
        /** \param to string definig the role of the agent expected to be the receiver of a message for this transition */
        std::string to;
        
        /** \param expectedPerf string definig the performative of a message for this transition */
        Performative expectedPerf;
        
        /** \param nextStateName string definig the name(uid) of the state that this transition will take the state machine to */
        std::string nextStateName;
        //bool messageParity;
        
        /** \param machine pointer to the state machine indirectly owning this transition */
        StateMachine *machine;
        
        /**
	  \param nextState pointer to the state that this transition will take the state machine to
	  \param owningState pointer to the state owning this transition
	  \param precedingState pointer to the state from which the state machine transitioned to the the state that this transition
			    belongs to
        */
        State *nextState,*owningState, *precedingState;
        
        /**
	  \param expectedSenders vector of agentID's that have the role indicated by the from field
        */
        std::vector<AgentID> expectedSenders;
        
        /**
	  \param expectedRecepients vector of agentID's that have the role indicated by the to field
        */
        std::vector<AgentID> expectedRecepients;
        
        /** \brief StateMachineTest class declared as friend for the testing phase only; NOT in the final version*/
        friend class StateMachineTest;
        
        
        
    public:
        /**
	  \brief empty constructor for the transition class; initializes fields
        */
        Transition();
        
        Transition(const Transition&);
        
        /**
	  \brief checks whether a fed messge is valid and acts accordingly
	  \param msg: message to processed
	  \return 0 if message is validated 1 otherwise
        */
        int consumeMessage(const ACLMessage &msg);
        
        /**
	  \brief method that validates all parameters of a message
	  \param msg message passed as parameter to be checked
	  \return true if message is valid false otherwise
        */
        bool validateMessage(const ACLMessage &msg);
        
        /** \brief method that generates usable parameters from the generic defined ones at build time(i.e: nextStateName) */
        void loadParameters();
        
        /** \brief method that populates(overwrites) expectedRecepients and expectedSenders fields from the involvedAgents field 
	  \brief of the StateMachine class and based on the generic from and to roles set
        */
        void updateRoles();
        
        /** \brief setter method for the precedingState field of the class */
        void setPrecedingState(State*);
        
        /** \brief setter methods for various fields of the class */
        void setExpectedPerformative	(const std::string&);
        void setNextStateName		(const std::string&);
        void setFrom		(const std::string&);
        void setTo			(const std::string&);
        void setOwningState		(State*);
        void setMachine		(StateMachine*);
        //void setNextState		(State*);
        
        /** \brief getter methods for various fields of the class*/
        std::string 	getExpectedPerformative() const;
        std::string 	getNextStateName() const;
        std::string 	getFrom() const;
        std::string 	getTo() const;
        State* 		getNextState() const;
        std::vector<AgentID> 	getExpectedSenders() const;
        std::vector<AgentID> 	getExpectedRecepients() const;
        StateMachine* 	getMachine() const;
        State* 		getPrecedingState() const;
        State*		getOwningState() const;
        
         /**
	  \brief misc method added to easily visualize a built stateMachine;
        */
        void print();

    private:
        /** \brief helper method to initialize the fields of the class; is to be called by all constructors */
        void initializeFields();
        
        /** 
	  \brief method that populates(overwrites) expectedRecepients and expectedSenders fields from the involvedAgents field 
	  \brief of the StateMachine class and based on the generic from and to roles set; if a certain role is not yet assigned
	  \brief it uses the agents from the message passed as argument to initialize them
	  
	  \param: ACLMessage to initialize agent roles from
        */
        bool updateRoles(const ACLMessage&);
        
        /**
	  \brief method that takes the necessary actions when a message has been accepted as valid but before the jump to another
	  state condition has been checked
	  \param msg: the validated message passed as argument
        */
        void performWithoutStateExit(const ACLMessage &msg);
        
        /**
	  \brief method that takes the necessary actions when a message has been accepted as valid and the jump to another state
	  \brief condition has been met(i.e: all involvedAgents are ticked in the owning state)
	  \param msg: the validated message passed as argument
        */
        void performOnStateExit(const ACLMessage &msg);
        
        /**
	  \brief checks whether all agents assigned to the expectedSenders vector are present in the message
        */
        bool checkAllExpectedSendersAccountedFor   (const ACLMessage &msg);
        
        /** \brief checks whether all agents assigned to the expectedRecepients vector are present in the message */
        bool checkAllExpectedRecepientsAccountedFor(const ACLMessage &msg);
        
        /** \brief checks whether the conversation id parameter is valid(checks from state machine) */
        bool validateConvID   	(const ACLMessage &msg);
        
        /** \brief checks whether the in reply to paramenter of the message is valid(by searching the archive of the preceding state) */
        bool validateInReplyTo	(const ACLMessage &msg);
        
        /** \brief checks whether the language parameter of the message is valid(checks from state machine) */
        bool validateLanguage 	(const ACLMessage &msg);
        
        /** \brief checks whether the ontology parameter of the message is valid(checks from state machine) */
        bool validateOntology 	(const ACLMessage &msg);
        
        /** \brief checks whether the protocol parameter of the message is valid(checks from state machine) */
        bool validateProtocol 	(const ACLMessage &msg);
        
        /** \brief checks whether the reply by parameter of the message is valid(NOT IMPLEMENTED) */
        bool validateReplyBy  	(const ACLMessage &msg);
        
        /** \brief checks whether the encoding parameter of the message is valid(checks from state machine) */
        bool validateEncoding 	(const ACLMessage &msg);
        
        /** \brief checks whether the performative parameter of the message is valid(checks from local variable) */
        bool validatePerformative	(const ACLMessage &msg);
        
        /** \brief checks whether the sender parameter of the message is valid(checks from expectedSenders vector) */
        bool validateSender 		(const ACLMessage &msg);
        
        /** \brief checks whether the receiver parameter of the message is valid(checks from expectedRecepients vector) */
        bool validateRecepients 	(const ACLMessage &msg);
        
        //void removeAllRecepientsBut(AgentID&);
        void removeAllAgentsBut(const AgentID&,std::vector<AgentID>&);
        
};

extern bool unloadedEqual(Transition &a, Transition &b);
    
} // end of acl
} // end of fipa

#endif