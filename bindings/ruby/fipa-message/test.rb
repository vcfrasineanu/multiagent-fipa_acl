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

	def test_ConversationMonitor
		agent0 = "agent_0"
		agent0Id = AgentId.new(agent0)
		agent1 = "agent_1"
		agent1Id = AgentId.new(agent1)
		protocol = "request"

		monitor = ConversationMonitor.new (agent0Id)
		#convPtr = Conversation.new (agent0)
		#id = convPtr.getConversationId

		convPtr = monitor.startConversation("topic")
		convPtr.setProtocol (protocol)
		assert_equal(protocol,convPtr.getProtocol)
		id = convPtr.getConversationId

		convPtr.setProtocolResourceDir("../../../configuration/protocols")

		myMsg = ACLMessage.new
		myMsg.setProtocol protocol
		myMsg.setSender(agent1Id)
		myMsg.setConversationID(id)
		myMsg.setPerformative :request

		assert_equal(false,convPtr.hasMessages)

		convPtr = monitor.updateConversation(myMsg)
		assert_equal(true,convPtr.hasMessages)		

		assert_equal(myMsg.to_s,(convPtr.getLastMessage).to_s) 
		
		conversations = monitor.getActiveConversations()
		ok = false
		for x in conversations
			if x==id
				ok = true
			end
		end
		assert_equal(true,ok)
		assert_equal(nil,conversations[1])		
		
		newConv = monitor.startConversation("new_topic")
		newConv.setProtocol (protocol)
		new_id = newConv.getConversationId
		
		myMsg1 = ACLMessage.new
		myMsg1.setProtocol protocol
		myMsg1.setSender(agent1Id)
		myMsg1.setConversationID(new_id)
		myMsg1.setPerformative :request
		newConv = monitor.updateConversation(myMsg1)
		assert_equal(myMsg1.to_s,(newConv.getLastMessage).to_s)

		conversations = monitor.getActiveConversations()
		assert_equal(nil,conversations[2])
		ok = false
		for x in conversations
			if x==id
				ok = true
			end
		end
		assert_equal(true,ok)
		ok = false
		for x in conversations
			if x==new_id
				ok = true
			end
		end
		assert_equal(true,ok)

		assert_equal(true,monitor.removeConversation(new_id))

		monitor.getOrCreateConversation(new_id)

		assert_equal(true,monitor.removeConversation(new_id))
		
		assert_equal(true,monitor.removeConversation(id))
		assert_equal(false,monitor.removeConversation(new_id))

		conversations = monitor.getActiveConversations()
		assert_equal(nil,conversations[0])

		monitor.cleanup()		
	end
end
