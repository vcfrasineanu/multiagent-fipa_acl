#include "bitefficient_format.h"
#include <boost/algorithm/string.hpp>

namespace fipa {
namespace acl {

std::string BitefficientFormat::getString(const std::string& sword)
{
    // StringLiteral | ByteLengthEncodedString
    return getByteLengthEncodedString(sword);
}

std::string BitefficientFormat::getByteLengthEncodedString(const std::string& sword)
{
    char digitString[100];
    int digit = sword.size();	
    snprintf(digitString,100, "#%d%c",digit,'\"');
    
    return digitString + sword;
}

std::string BitefficientFormat::getNullTerminatedString(const std::string& string)
{
    return getByteLengthEncodedString(string) + char(0x00);
}

std::string BitefficientFormat::getBinDateTimeToken(const std::string& date1)
{
    return char(0x20) + getBinDate(date1);
}

std::string BitefficientFormat::getBinDateTimeToken(const base::Time& time)
{
    return char(0x20) + getBinDate(time);
}

std::string BitefficientFormat::getBinDate(const base::Time& baseTime)
{

    std::string time = baseTime.toString(base::Time::Milliseconds);

    // Input format should be "%Y%m%d-%H:%M:%S"
    // Strip ':' and '-' to allow from_iso_string to work
    boost::erase_all(time,":");
    boost::erase_all(time,"-");

    // extend millisecond field to 4 digits
    time.insert(14,sizeof(char),'0');

    return getBinDate(time);
}

std::string BitefficientFormat::getBinDate(const std::string& date1)
{
    std::string retstr;
    unsigned int i;
    for (i = 0; i < date1.length(); i = i + 2)
    {
        retstr = retstr + getCodedNaturalNumber(date1.substr(i,2));
    }
    
    return retstr;
}

std::string BitefficientFormat::getCodedNumber(const std::string& cn)
{
    std::string retstr;
    unsigned int i;
    char code(0x00);
    size_t decimal_size = cn.length();

    for(i = 0; i < decimal_size; i++)
    {
        if ((cn[i]>='0') && (cn[i]<='9')) code = char(code + 1 + int(cn[i]) - 48);
        if (cn[i] == '+') code = char(code + 12);
        if ((cn[i] == 'E') || (cn[i] == 'e')) code = char(code + 13);
        if (cn[i] == '-') code = char(code + 14);
        if (cn[i] == '.') code = char(code + 15);
        
        if (i%2 == 0)
        {
            code = code<<4;
        } else {
            retstr += code;
            code = char(0x00);
        }
    }

    if (decimal_size % 2 != 0)
    {
        retstr += code;
    } else {
        retstr += char(0x00);
    }
     
    return retstr;
}

std::string BitefficientFormat::getCodedNaturalNumber(const std::string& cn)
{
    std::string retstr;

    unsigned int i;
    char code(0x00);
    size_t decimal_size = cn.length();

    for(i = 0; i < decimal_size; ++i)
    {
        switch(cn[i])
        {
            // Ignoring leading zeros here, 
            case '0': code += char(0x01); break;
            case '1': code += char(0x02); break;
            case '2': code += char(0x03); break;
            case '3': code += char(0x04); break;
            case '4': code += char(0x05); break;
            case '5': code += char(0x06); break;
            case '6': code += char(0x07); break;
            case '7': code += char(0x08); break;
            case '8': code += char(0x09); break;
            case '9': code += char(0x0a); break;
            //case '+': code += char(0x0c); break;
            //case 'e': 
            //case 'E': code += char(0x0d); break;
            //case '-': code += char(0x0e); break;
            //case '.': code += char(0x0f); break;
            default: 
                      assert(false);
        }

          
        if (i%2 == 0)
        { 
            code = code<<4;
        } else {
            retstr += code; 
            code = char(0x00); 
        }
    }

    if (i%2 != 0)
    {
        retstr += code;
    }
     
    return retstr;
}

std::string BitefficientFormat::getACLRepresentation(const representation::Type& type)
{
    std::string msg = "";
    switch(type)
    {
        case representation::BITEFFICIENT: 
            msg += char(0x10); break;
        case representation::STRING:
            msg += char(0x11); break;
        case representation::XML:
            msg += char(0x12); break;
        default:
            throw std::runtime_error("Cannot encode unknown acl representation");
    }

    return msg;
}

} // end namespace acl
} // end namespace fipa
