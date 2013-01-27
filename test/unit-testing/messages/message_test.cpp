#include <boost/test/auto_unit_test.hpp>
#include <fipa_acl/bitefficient_message.h>
#include <fipa_acl/message_parser/bitefficient/grammar_bitefficient.h>
#include <string>
#include <limits>

template< template<class> class TIn, typename TOut>
TOut testGrammar(const std::string& storage, bool expectedSuccess = true)
{
    typedef TIn<std::string::const_iterator> grammar_type;
    grammar_type grammar;
    TOut parseTree;

    std::string::const_iterator iter = storage.begin();
    std::string::const_iterator end = storage.end();
    bool r = parse(iter, end, grammar, parseTree);
    if(expectedSuccess)
    {
    	BOOST_REQUIRE_MESSAGE(r, "Test Grammar with string of size: " << storage.size());
    } else {
    	BOOST_REQUIRE_MESSAGE(!r, "Test Grammar with string of size: " << storage.size());
    }
    return parseTree;
}

template< template<class> class TIn, typename TOut>
void testFailGrammar(const std::string& storage)
{
    typedef TIn<std::string::const_iterator> grammar_type;
    grammar_type grammar;
    TOut parseTree;

    std::string::const_iterator iter = storage.begin();
    std::string::const_iterator end = storage.end();
    bool r = parse(iter, end, grammar, parseTree);

    BOOST_REQUIRE(!r);
}

BOOST_AUTO_TEST_SUITE(fipa_message_test_suite)

BOOST_AUTO_TEST_CASE(grammar_test)
{
        namespace fab = fipa::acl::bitefficient;

        typedef fipa::acl::bitefficient::BinStringNoCodetable<std::string::const_iterator> bitefficient_message_grammar;
	bitefficient_message_grammar grammar;
	fipa::acl::ByteSequence parseTree;
        {
            std::string storage;
            storage += char(0x16);
            storage += char(0x0a);
            storage += char(0x00);
            storage += "123456789";

	    std::string::const_iterator iter = storage.begin();
	    std::string::const_iterator end = storage.end();
	    bool r = parse(iter, end, grammar, parseTree);

            BOOST_REQUIRE(r);

            testGrammar<fipa::acl::bitefficient::BinStringNoCodetable, fipa::acl::ByteSequence>(storage);
        }
        {
            std::string storage;
            storage += char(0x14);
            storage += "#1\"A";
            storage += char(0x00);

	    std::string::const_iterator iter = storage.begin();
	    std::string::const_iterator end = storage.end();
	    bool r = parse(iter, end, grammar, parseTree);

            BOOST_REQUIRE(r);
        }

        // Index
        {
            std::string storage;
            storage += char(0x0a);
            uint32_t number = testGrammar<fab::Index,uint_least16_t>(storage);
            BOOST_REQUIRE_MESSAGE(number == 10, "Number is " << number);
        }
        {
            std::string storage;
            storage += char(0x10);
            storage += char(0x00);
            uint32_t number = testGrammar<fab::Index,uint_least16_t>(storage);
            BOOST_REQUIRE_MESSAGE(number == 4096, "Number is " << number);
        }

        // Len8
        {
            std::string storage;
            storage += char(0x0a);
            uint32_t number = testGrammar<fab::Len8,uint_least8_t>(storage);
            BOOST_REQUIRE_MESSAGE(number == 10, "Number is " << number);
        }

        // Len16
        {
            std::string storage;
            storage += char(0x10);
            storage += char(0x00);
            uint32_t number = testGrammar<fab::Len16,uint_least16_t>(storage);
            BOOST_REQUIRE_MESSAGE(number == 4096, "Number is " << number);
        }

        // Len32
        {
            std::string storage;
            storage += char(0x10);
            storage += char(0x00);
            storage += char(0x00);
            storage += char(0x00);
            uint32_t number = testGrammar<fab::Len32,uint_least32_t>(storage);
           BOOST_REQUIRE_MESSAGE(number == 268435456, "Number is " << number);
        }

        // CodedNumber
        {
		std::string tokens[] = { "","0","1","2","3","4","5","6","7","8","9","+","E","-","."};
                // skip the first 
		for(uint32_t i = 1; i < 15; ++i)
		{
		    std::string storage;
                    if(i <= 10)
                    {
		        storage += char(i) << 4;
                    } else {
		        storage += char(i+1) << 4;
                    }
		    std::string token = testGrammar<fab::CodedNumber, std::string>(storage);
		    BOOST_REQUIRE_MESSAGE( token == tokens[i], "CodedNumber is <" << token << "> expected <" << tokens[i] << ">"); 
		}
        }
        // Digits
        {
            std::string storage;
            storage += char(0b01000100);
            // Requires padding due to even number of digits
            storage += char(0x00);
            std::string number = testGrammar<fab::Digits,std::string>(storage);
           BOOST_REQUIRE_MESSAGE(number == "33", "Number is " << number << " expected 33");
        }
        
        {
            std::string storage;
            storage += char(0b01000000);
            // Requires padding within byte due to odd number of digits
            std::string number = testGrammar<fab::Digits,std::string>(storage);
           BOOST_REQUIRE_MESSAGE(number == "3", "Number is " << number << " expected 3");
        }

        {
            std::string storage;
            storage += char(0b01000001);
            storage += char(0b11110011);
            storage += char(0b11101101);
            storage += char(0b10000011);
            storage += char(0x00);
            std::string number = testGrammar<fab::Digits,std::string>(storage);
           BOOST_REQUIRE_MESSAGE(number == "30.2-E72", "Number is " << number << " expected 30.2-E72");
        }
        {
            std::string storage;
            storage += char(0b01000001);
            storage += char(0b11110011);
            storage += char(0b11101101);
            storage += char(0b10000011);
            testFailGrammar<fab::Digits,std::string>(storage);
        }

        // BinNumber
        {
            // decimal number
            std::string storage;
            storage += char(0x12);
            storage += char(0b01000001);
            storage += char(0b11110011);
            storage += char(0b11101101);
            storage += char(0b10000011);
            storage += char(0x00);
            std::string number = testGrammar<fab::BinNumber,std::string>(storage);
           BOOST_REQUIRE_MESSAGE(number == "30.2-E72", "Number is " << number << " expected 30.2-E72");
        }
        {
            // hex number-> 0xb -> decimal 11
            std::string storage;
            storage += char(0x13);
            storage += char(0b00100010);
            storage += char(0x00);
            std::string number = testGrammar<fab::BinNumber,std::string>(storage);
           BOOST_REQUIRE_MESSAGE(number == "0xb", "Number is " << number << " expected 0xb");
        }
        
        // DateTime
        {
            std::string storage;
            storage += char(0b00110001); //20
            storage += char(0b00100100); //13
            storage += char(0b00100000); // 1
            storage += char(0b00100000); // 1
            storage += char(0b00100011); // 12
            storage += char(0b01101010); // 59
            storage += char(0b01101010); // 59
            storage += char(0b00010000); // 0
            storage += char(0b00110001); // 20

            fipa::acl::Time time = testGrammar<fab::DateTime, fipa::acl::Time>(storage);
            BOOST_REQUIRE_MESSAGE(time.tm_msec == 20, "Milliseconds are " << time.tm_msec << " expected 20");
            BOOST_REQUIRE_MESSAGE(time.tm_sec == 59, "Seconds are " << time.tm_sec << " expected 59");
            BOOST_REQUIRE_MESSAGE(time.tm_min == 59, "Minutes are " << time.tm_min << " expected 59");
            BOOST_REQUIRE_MESSAGE(time.tm_hour == 12, "Hours are " << time.tm_hour << " expected 12");
            BOOST_REQUIRE_MESSAGE(time.tm_mday == 1, "Day is " << time.tm_mday << " expected 1");
            BOOST_REQUIRE_MESSAGE(time.tm_mon == 0, "Month is " << time.tm_mon << " expected 0");
            BOOST_REQUIRE_MESSAGE(time.tm_year == 113, "Year is " << time.tm_year << " expected 113");
            //%tm_isdst	Daylight Saving Time flag
        }
        // BinDateTime
        {
            std::string dateTimeStorage;
            dateTimeStorage += char(0b00110001); //20
            dateTimeStorage += char(0b00100100); //13
            dateTimeStorage += char(0b00100000); // 1
            dateTimeStorage += char(0b00100000); // 1
            dateTimeStorage += char(0b00100011); // 12
            dateTimeStorage += char(0b01101010); // 59
            dateTimeStorage += char(0b01101010); // 59
            dateTimeStorage += char(0b00010000); // 0
            dateTimeStorage += char(0b00110001); // 20

            std::string expectedDateTimeString = "2013-01-01T12:59:59";
            {
                std::string storage;
                storage += char(0x20);
                storage += dateTimeStorage;
            	fipa::acl::DateTime time = testGrammar<fab::BinDateTime, fipa::acl::DateTime>(storage);
                std::string expected = " " + expectedDateTimeString; 
                BOOST_REQUIRE_MESSAGE(time.toString() == expected, "DateTime is '" << time.toString() << "' expected '" << expected << "'" << time.toString().size() << " vs " << expected.size());
            }
            {
                std::string storage;
                storage += char(0x21);
                storage += dateTimeStorage;
            	fipa::acl::DateTime time = testGrammar<fab::BinDateTime, fipa::acl::DateTime>(storage);
                std::string expected = "+" + expectedDateTimeString; 
                BOOST_REQUIRE_MESSAGE(time.toString() == expected, "DateTime is '" << time.toString() << "' expected '" << expected << "'");
            }
            {
                std::string storage;
                storage += char(0x22);
                storage += dateTimeStorage;
            	fipa::acl::DateTime time = testGrammar<fab::BinDateTime, fipa::acl::DateTime>(storage);
                std::string expected = "-" + expectedDateTimeString; 
                BOOST_REQUIRE_MESSAGE(time.toString() == expected, "DateTime is '" << time.toString() << "' expected '" << expected << "'");
            }

            //typeDesignator
            dateTimeStorage += 'Z'; 
            {
                std::string storage;
                storage += char(0x24);
                storage += dateTimeStorage;
            	fipa::acl::DateTime time = testGrammar<fab::BinDateTime, fipa::acl::DateTime>(storage);
                std::string expected = " " + expectedDateTimeString + "Z"; 
                BOOST_REQUIRE_MESSAGE(time.toString() == expected, "DateTime is '" << time.toString() << "' expected '" << expected << "'");
            }
            {
                std::string storage;
                storage += char(0x25);
                storage += dateTimeStorage;
            	fipa::acl::DateTime time = testGrammar<fab::BinDateTime, fipa::acl::DateTime>(storage);
                std::string expected = "+" + expectedDateTimeString + "Z"; 
                BOOST_REQUIRE_MESSAGE(time.toString() == expected, "DateTime is '" << time.toString() << "' expected '" << expected << "'");
            }
            {
                std::string storage;
                storage += char(0x26);
                storage += dateTimeStorage;
            	fipa::acl::DateTime time = testGrammar<fab::BinDateTime, fipa::acl::DateTime>(storage);
                std::string expected = "-" + expectedDateTimeString + "Z"; 
                BOOST_REQUIRE_MESSAGE(time.toString() == expected, "DateTime is '" << time.toString() << "' expected '" << expected << "'");
            }
        }
        // StringLiteral
        {
            std::string storage = "\"\\\"ABC_0123456789\"";
            std::string expected = "\"ABC_0123456789";
            fipa::acl::ByteSequence stringLiteral = testGrammar<fab::StringLiteral, fipa::acl::ByteSequence>(storage);
            BOOST_REQUIRE_MESSAGE(stringLiteral.toRawDataString() == expected, "String is '" << stringLiteral.toRawDataString() << "' expected '" << expected << "'");
        }
        // StringLiteralTerminated
        {
            std::string storage = "\"\\\"ABC_0123456789\"";
            storage += char(0x00);
            std::string expected = "\"ABC_0123456789";
            fipa::acl::ByteSequence stringLiteral = testGrammar<fab::StringLiteralTerminated, fipa::acl::ByteSequence>(storage);
            BOOST_REQUIRE_MESSAGE(stringLiteral.toRawDataString() == expected, "String is '" << stringLiteral.toRawDataString() << "' expected '" << expected << "'");
        }

        // NullTerminatedString
        // ByteLengthEncodedStringTerminated
        {
            std::string storage = "#14\"ABC_0123456789";
            storage += char(0x00);
            std::string expected = "ABC_0123456789";
            fipa::acl::ByteSequence stringLiteral = testGrammar<fab::ByteLengthEncodedStringTerminated, fipa::acl::ByteSequence>(storage);
            BOOST_REQUIRE_MESSAGE(stringLiteral.toRawDataString() == expected, "ByteLengthEncodedStringTerminated is '" << stringLiteral.toRawDataString() << "' expected '" << expected << "'");
        }
        // BinStringNoCodetable
        {

            {
                std::string storage;
                storage += char(0x14);
                storage += "\"";
                std::string expected = "TEST";
                storage += expected;
                storage += "\"";
                storage += char(0x00);
                fipa::acl::ByteSequence stringLiteral = testGrammar<fab::BinStringNoCodetable, fipa::acl::ByteSequence>(storage);
                BOOST_REQUIRE_MESSAGE(stringLiteral.toRawDataString() == expected, "BinStringNoCodetable is '" << stringLiteral.toRawDataString() << "' expected '" << expected << "'");
            }
            {
                std::string storage;
                storage += char(0x16);
                storage += char(0x01);
                std::string expected = "a";
                storage += expected;
                fipa::acl::ByteSequence stringLiteral = testGrammar<fab::BinStringNoCodetable, fipa::acl::ByteSequence>(storage);
                BOOST_REQUIRE_MESSAGE(stringLiteral.toRawDataString() == expected, "BinStringNoCodetable is '" << stringLiteral.toRawDataString() << "' expected '" << expected << "'");
            }
            {
                std::string storage;
                storage += char(0x17);
                storage += char(0x00);
                storage += char(0x01);
                std::string expected = "a";
                storage += expected;
                fipa::acl::ByteSequence stringLiteral = testGrammar<fab::BinStringNoCodetable, fipa::acl::ByteSequence>(storage);
                BOOST_REQUIRE_MESSAGE(stringLiteral.toRawDataString() == expected, "BinStringNoCodetable is '" << stringLiteral.toRawDataString() << "' expected '" << expected << "'");
            }
            {
                std::string storage;
                storage += char(0x19);
                storage += char(0x00);
                storage += char(0x00);
                storage += char(0x00);
                storage += char(0x01);
                std::string expected = "a";
                storage += expected;
                fipa::acl::ByteSequence stringLiteral = testGrammar<fab::BinStringNoCodetable, fipa::acl::ByteSequence>(storage);
                BOOST_REQUIRE_MESSAGE(stringLiteral.toRawDataString() == expected, "BinStringNoCodetable is '" << stringLiteral.toRawDataString() << "' expected '" << expected << "'");
            }
        }
        // BinStringCodetable
        {
            {
                std::string storage;
                storage += char(0x15);
                storage += char(0x01);
                try {
                    fipa::acl::ByteSequence stringLiteral = testGrammar<fab::BinStringCodetable, fipa::acl::ByteSequence>(storage);
                    BOOST_REQUIRE_MESSAGE(false, "Expected exception, since codetable is not supported");
                } catch(...)
                {
                    BOOST_REQUIRE_MESSAGE(true, "Codetable not supported");
                }
            }
            {
                std::string storage;
                storage += char(0x18);
                storage += char(0x01);
                try {
                    fipa::acl::ByteSequence stringLiteral = testGrammar<fab::BinStringCodetable, fipa::acl::ByteSequence>(storage);
                    BOOST_REQUIRE_MESSAGE(false, "Expected exception, since codetable is not supported");
                } catch(...)
                {
                    BOOST_REQUIRE_MESSAGE(true, "Codetable not supported");
                }
            }
        }
        // BinString
        // Word
        {
            char falseToken[] = { '(',')', 0x20, 0x00,  }; 
            // disallowed tokens
            for(uint32_t i = 0; i < 4; ++i)
            {
            	std::string storage;
            	storage += falseToken[i];
                std::string stringLiteral = testGrammar<fab::Word, std::string>(storage, false);
            }

            for(uint32_t i = 0; i < 3; ++i)
            {
            	std::string storage;
                std::string expected = "abcdefg";
                storage += expected;
                // will read only till it finds a false token
            	storage += falseToken[i];
                std::string stringLiteral = testGrammar<fab::Word, std::string>(storage, true);
                BOOST_REQUIRE_MESSAGE(stringLiteral == expected, "StringLiteral is '" << stringLiteral << "', but expected '" << expected << "'");

            }

            std::string falseStartToken[] = { "#", "0", "1","-","@"};
            // skip the first 
            for(uint32_t i = 0; i < 5; ++i)
            {
            	std::string storage;
            	storage += falseStartToken[i];
                std::string stringLiteral = testGrammar<fab::Word, std::string>(storage, false);
            }

            // skip the first 
            for(uint32_t i = 0; i < 5; ++i)
            {
            	std::string storage;
                storage += "abcdefg";
            	storage += falseStartToken[i];
                std::string stringLiteral = testGrammar<fab::Word, std::string>(storage, true);
                BOOST_REQUIRE_MESSAGE(stringLiteral == storage, "StringLiteral is '" << stringLiteral << "', but expected '" << storage << "'");
            }
        }
        // BinExpression
        // AgentIdentifier
        {
            std::string agentName = "test_agent_name";
            std::string storage;
            storage += char(0x02);
            storage += char(0x10);
            storage += agentName;
            storage += char(0x00);
            storage += char(0x01);
            fipa::acl::AgentIdentifier agent = testGrammar<fab::AgentIdentifier, fipa::acl::AgentIdentifier>(storage, true);
            BOOST_REQUIRE_MESSAGE( agent.name == agentName, "AgentName is '" << agent.name << "' , but expected '" << agentName);
        }
}

BOOST_AUTO_TEST_CASE(message_test)
{
    using namespace fipa::acl;

    ACLMessageOutputParser outputParser;
    ACLMessage msg("inform");
    AgentID origin("proxy");
    AgentID receiver("crex_0_CREXCORE");

    AgentID resolver0("resolver0");
    AgentID resolver1("resolver1");

    receiver.addResolver(resolver0);
    receiver.addResolver(resolver1);

    msg.setSender(origin);
    msg.addReceiver(receiver);
    msg.addReplyTo(origin);
    msg.setPerformative(ACLMessage::REQUEST);
    msg.setProtocol(std::string("RIMRES"));
    msg.setLanguage(std::string("test language"));
    msg.setEncoding(std::string("test encoding"));
    msg.setOntology(std::string("test ontology"));
    msg.setReplyWith(std::string("test reply_with"));
    base::Time time = base::Time::fromString("20101223-12:00:37", base::Time::Seconds);
    msg.setReplyBy(time);
    BOOST_CHECK_MESSAGE(true, "Setting time " << time.toString());
    msg.setConversationID(std::string("test conversationID"));
    msg.setContent("test content");

    std::vector<AgentID> agents = msg.getAllReceivers();
    BOOST_CHECK_MESSAGE(agents.size() == 1, "Original msg: receiver agent size is one");

    outputParser.setMessage(msg);
    std::string encodedMsg = outputParser.getBitMessage();

    MessageParser inputParser;
    ACLMessage outputMsg;

    BOOST_ASSERT( inputParser.parseData(encodedMsg, outputMsg) );

    BOOST_ASSERT(outputMsg.getPerformative() == PerformativeTxt[ACLMessage::REQUEST]);
    BOOST_ASSERT(outputMsg.getSender() == msg.getSender());


    BOOST_ASSERT(outputMsg.getAllReceivers() == msg.getAllReceivers());
    agents = outputMsg.getAllReceivers();
    BOOST_ASSERT(agents.size() == 1);
    BOOST_ASSERT(agents[0].getResolvers().size() == 2);

    std::vector<AgentID>::iterator it = agents.begin();
    for(; it != agents.end(); ++it)
    {
        BOOST_ASSERT(it->getName() == receiver.getName());
    }
    
    BOOST_ASSERT(outputMsg.getProtocol() == msg.getProtocol());
    BOOST_ASSERT(outputMsg.getLanguage() == msg.getLanguage());
    BOOST_ASSERT(outputMsg.getEncoding() == msg.getEncoding());
    BOOST_ASSERT(outputMsg.getOntology() == msg.getOntology());
    BOOST_ASSERT(outputMsg.getReplyWith() == msg.getReplyWith());
    BOOST_REQUIRE_MESSAGE(outputMsg.getReplyBy() == msg.getReplyBy(), "output: " << outputMsg.getReplyBy().toString() << " vs. msg " << msg.getReplyBy().toString());
    BOOST_ASSERT(outputMsg.getConversationID() == msg.getConversationID());
    BOOST_ASSERT(outputMsg.getContent() == msg.getContent());
}

BOOST_AUTO_TEST_CASE(binary_message_content)
{
    using namespace fipa::acl;

    ACLMessageOutputParser outputParser;
    ACLMessage msg("inform");
    AgentID origin("proxy");
    AgentID receiver("crex_0_CREXCORE");
    std::string content;
    size_t content_size;
    MessageParser inputParser;
    ACLMessage outputMsg;

    // Testing binary content with len8 as size descriptor
    content += '\0';
    content += "012345689";
    content += '\0';
    content += "012345689";
    msg.setContent(content);
    
    content_size = msg.getContent().size();
    BOOST_REQUIRE_MESSAGE(content_size > 0, "Content size > 0");

    outputParser.setMessage(msg);
    std::string encodedMsg = outputParser.getBitMessage();

    for(size_t i = 0; i < encodedMsg.size(); ++i)
    {
        fprintf(stderr,"%x",(int) i);
    }

    BOOST_REQUIRE_MESSAGE( inputParser.parseData(encodedMsg, outputMsg), "Parsing binary content with len8 size field" );
    std::string content_out = outputMsg.getContent();
    for(size_t i = 0; i < content_out.size(); ++i)
    {
        BOOST_CHECK_MESSAGE(1==1, "" << content_out.data()[i]);
    }

    BOOST_REQUIRE_MESSAGE(outputMsg.getContent().size() == content_size, "Check content size output " << outputMsg.getContent().size() << " vs. " << content_size);

    // Testing binary content with len16 as size descriptor
    uint32_t size = std::numeric_limits<uint8_t>::max() + 10;
    {
        std::string msgContent(size, '\0');
        msg.setContent(msgContent);

        content_size = msg.getContent().size();
        BOOST_REQUIRE_MESSAGE(content_size == size, "Content size " << content_size << " vs. size " << size);

        outputParser.setMessage(msg);
        encodedMsg = outputParser.getBitMessage();

        BOOST_REQUIRE_MESSAGE( inputParser.parseData(encodedMsg, outputMsg), "Parsing binary content with len16 size field: size " << content_size);
        uint32_t outputMsgSize = outputMsg.getContent().size();
        BOOST_REQUIRE_MESSAGE(outputMsgSize == size, "Check content size of output for len16: " << size << " expected - contained: " << outputMsgSize);
    }

    {
        // Testing binary content with len32 as size descriptor
        size = std::numeric_limits<uint16_t>::max() + 10;

        std::string msgContent(size, '\0');
        msg.setContent(msgContent);

        content_size = msg.getContent().size();
        BOOST_ASSERT(content_size == size);

        outputParser.setMessage(msg);
        encodedMsg = outputParser.getBitMessage();

        BOOST_REQUIRE_MESSAGE( inputParser.parseData(encodedMsg, outputMsg), "Parsing binary content with len32 size field");

        uint32_t outputMsgSize = outputMsg.getContent().size();
        BOOST_REQUIRE_MESSAGE(outputMsgSize == size, "Check content size of output for len32: " << size << " expected - contained: " << outputMsgSize);
    }

    {
        ACLMessage msg(ACLMessage::REQUEST);
        msg.setContent("content");
        msg.setEncoding("encoding");
        msg.setLanguage("language");
        msg.addReceiver(AgentID("test"));
        msg.setSender(AgentID("sender"));
        msg.addReceiver(AgentID("sender"));

        ACLMessage msgCopy = msg;
        BOOST_REQUIRE(msgCopy == msg);
    }
}

BOOST_AUTO_TEST_SUITE_END()

