#include "grammar_bitefficient.h"
/**
    \file gramar_bitefficient.cpp
    \brief delcares instances needed for the grammar
    \author Mircea Cretu Stancu
    
    \version 1.0
    
*/
namespace fipa {

phoenix::function<extractFromCodetableImpl> extractFromCodetable;
phoenix::function<buildStringImpl> buildString;
phoenix::function<printImpl> print;
phoenix::function<convertToTimeImpl> convertToTime;
phoenix::function<convertToNumberTokenImpl> convertToNumberToken;
phoenix::function<convertDigitsToHexImpl> convertDigitsToHex;
phoenix::function<convertToStringImpl> convertToString;
phoenix::function<convertToCharVectorImpl> convertToCharVector;


}