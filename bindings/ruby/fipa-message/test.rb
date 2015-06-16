require 'fipa-message'
require 'test/unit'

include FIPA

class FipaMessageTest < Test::Unit::TestCase

	def test_UserdefinedParameter
	    name = "param_0"
	    value = "value_0"

	    param = UserDefinedParameter.new
	    param.setName name
	    param.setValue value

	    assert_equal(value, param.getValue)
	    assert_equal(name, param.getName)
	end

	def test_AgentId
	    agent_name = "agent_0"
	    address = "address_0"
	    resolver = AgentId.new "resolver_0"

	    id = AgentId.new(agent_name)
	    id.addAddress address
	    id.addResolver resolver

	    assert_equal(agent_name, id.getName)

	    addresses = id.getAddresses
	    assert_equal(addresses.length, 1)
	    assert_equal(addresses[0], address)

	    resolvers = id.getResolvers
	    assert_equal(resolvers.length, 1)
	    assert_equal(resolvers[0].getName, resolver.getName)

	    id.deleteResolver resolver
	    resolvers = id.getResolvers
	    assert_equal(resolvers.length, 0)

	    param = UserDefinedParameter.new
	    param_name = 'param_0'
	    param.setName param_name
	    id.addUserDefinedParameter param 
            params = id.getUserDefinedParameters
	    assert_equal(params.length,1)
	    assert_equal(params[0].getName, param_name)
	    assert_equal(params[0].getName, param.getName)
	end

	def test_ACLMessage
	    msg = ACLMessage.new
	    msg.setPerformative :inform
	    msg.setProtocol "RIMRES"
	end

	def test_Serialization
	    protocol = "RIMRES"
	    performative = :request_whenever

	    msg = ACLMessage.new
	    msg.setProtocol protocol
	    msg.setPerformative performative

	    data = Utils.serialize_message(msg)

	    msg_deserialized = Utils.deserialize_message(data)

	    assert_equal(msg_deserialized.getProtocol, protocol)
	    assert_equal(msg_deserialized.getPerformative, performative)
	end

	def test_Deserialization
	    array = Array.new
	    data = IO.read(File.join(File.dirname(__FILE__),"inform"))
	    msg = Utils.deserialize_message(data, false)

	    assert_equal(msg.getPerformative, :inform)
	end

	def test_Conversation
		agent = "agent_0"
		agentId = AgentId.new(agent)
		protocol = "request"
		language = "content_language"

		conv = Conversation.new (agent)
		conv.setProtocol (protocol)
		conv.setContentLanguage (language)
		conv.setProtocolResourceDir("../../../configuration/protocols")

		assert_equal(protocol,conv.getProtocol)
		assert_equal(language,conv.getContentLanguage)
		assert_equal(agent,conv.getOwner)
		assert_equal(false,conv.hasEnded)
		assert_equal(false,conv.hasMessages)

		id = conv.getConversationId
		
		msg = ACLMessage.new
		msg.setProtocol protocol
		msg.setSender agentId
		
		#conv.update(msg)
	end

	def test_ConversationMonitor
		agent0 = "agent_0"
		agent0Id = AgentId.new(agent0)
		protocol = "request"
		
		monitor = ConversationMonitor.new (agent0Id)
		convPtr = Conversation.new (agent0)
		convPtr.setProtocol (protocol)

		monitor.startConversation("topic")
		assert_equal(convPtr.getProtocol,protocol)

		convPtr.setProtocolResourceDir("../../../configuration/protocols")
		id = convPtr.getConversationId

		myMsg = ACLMessage.new
		myMsg.setProtocol protocol
		agent1 = "agent_1"
		agent1Id = AgentId.new(agent1)
		myMsg.setSender(agent1Id)
		myMsg.setConversationID(id)
		myMsg.setPerformative :request

		assert_equal(convPtr.hasMessages, false)
		#convPtr.update(myMsg)
		monitor.getOrCreateConversation(id)
		convPtr = monitor.updateConversation(myMsg)
		assert_equal(convPtr.hasMessages, true)		

		assert_equal((convPtr.getLastMessage).to_s,myMsg.to_s)

		##monitor.getActiveConversations()
		##newConv = Conversation.new(newAgent)
		##newConv = monitor.startConversation("new_topic")
		##new_id = newConv.getConversationId
		
		#newConv.update(myMsg)
		#assert_equal(newConv.getLastMessage,myMsg)

		##assert_equal(monitor.removeConversation(new_id),true)

		##monitor.getOrCreateConversation(new_id)
		##assert_equal(monitor.removeConversation(new_id),true)
		
		##assert_equal(monitor.removeConversation(id),true)
		monitor.cleanup()		
	end
end
