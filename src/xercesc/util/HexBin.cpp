/*
 * Copyright 2001,2004 The Apache Software Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Log$
 * Revision 1.7  2005/01/07 15:12:10  amassari
 * Removed warnings
 *
 * Revision 1.6  2004/12/10 10:37:56  cargilld
 * Fix problem with hexbin::decode and use XMLByte instead of XMLCh for output of decoding.
 *
 * Revision 1.5  2004/09/08 13:56:22  peiyongz
 * Apache License Version 2.0
 *
 * Revision 1.4  2004/08/11 16:46:54  peiyongz
 * Decoding and getCanRep
 *
 * Revision 1.3  2002/11/04 15:22:03  tng
 * C++ Namespace Support.
 *
 * Revision 1.2  2002/04/18 14:55:38  peiyongz
 * Bug#7301: Redundant range-check in HexBin.cpp, patch from Martin Kalen
 *
 * Revision 1.1.1.1  2002/02/01 22:22:10  peiyongz
 * sane_include
 *
 * Revision 1.1  2001/05/16 15:25:38  tng
 * Schema: Add Base64 and HexBin.  By Pei Yong Zhang.
 *
 */

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/util/HexBin.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------
//  constants
// ---------------------------------------------------------------------------
static const int BASELENGTH = 255;

// ---------------------------------------------------------------------------
//  class data member
// ---------------------------------------------------------------------------
XMLByte HexBin::hexNumberTable[BASELENGTH];
bool HexBin::isInitialized = false;

int HexBin::getDataLength(const XMLCh* const hexData)
{
    if (!isArrayByteHex(hexData))
        return -1;

    return XMLString::stringLen(hexData)/2;
}

bool HexBin::isArrayByteHex(const XMLCh* const hexData)
{
    if ( !isInitialized )
        init();

    if (( hexData == 0 ) || ( *hexData == 0 )) // zero length
        return false;

    int strLen = XMLString::stringLen(hexData);
    if ( strLen%2 != 0 )
        return false;

    for ( int i = 0; i < strLen; i++ )
        if( !isHex(hexData[i]) )
            return false;

    return true;
}

XMLCh* HexBin::getCanonicalRepresentation(const XMLCh*          const hexData
                                        ,       MemoryManager*  const manager)
{

    if (getDataLength(hexData) == -1)
        return 0;

    XMLCh* retStr = XMLString::replicate(hexData, manager);
    XMLString::upperCase(retStr);

    return retStr;
}

XMLCh* HexBin::decode(const XMLCh*          const   hexData
                    ,       MemoryManager*  const   manager)
{
    if (( hexData == 0 ) || ( *hexData == 0 )) // zero length
        return 0;

    int strLen = XMLString::stringLen(hexData);
    if ( strLen%2 != 0 )
        return 0;

    if ( !isInitialized )
        init();

    //prepare the return string
    int decodeLength = strLen/2;
    XMLCh *retVal = (XMLCh*) manager->allocate( (decodeLength + 1) * sizeof(XMLCh));
    ArrayJanitor<XMLCh> janFill(retVal, manager);
    
    XMLByte temp1, temp2;
    for( int i = 0; i<decodeLength; i++ ) {
        temp1 = hexNumberTable[hexData[i*2]];
        if (temp1 == (XMLByte) -1)
            return 0;
        temp2 = hexNumberTable[hexData[i*2+1]];
        if (temp2 == (XMLByte) -1)
            return 0;
        retVal[i] = (XMLCh)((temp1 << 4) | temp2);
    }

    janFill.release();
    retVal[decodeLength] = 0;
    return retVal;
}

XMLByte* HexBin::decodeToXMLByte(const XMLCh*          const   hexData
                    ,       MemoryManager*  const   manager)
{
    if (( hexData == 0 ) || ( *hexData == 0 )) // zero length
        return 0;

    int strLen = XMLString::stringLen(hexData);
    if ( strLen%2 != 0 )
        return 0;

    if ( !isInitialized )
        init();

    //prepare the return string
    int decodeLength = strLen/2;
    XMLByte *retVal = (XMLByte*) manager->allocate( (decodeLength + 1) * sizeof(XMLByte));
    ArrayJanitor<XMLByte> janFill(retVal, manager);
    
    XMLByte temp1, temp2;
    for( int i = 0; i<decodeLength; i++ ) {
        temp1 = hexNumberTable[hexData[i*2]];
        if (temp1 == (XMLByte) -1)
            return 0;
        temp2 = hexNumberTable[hexData[i*2+1]];
        if (temp2 == (XMLByte) -1)
            return 0;
        retVal[i] = ((temp1 << 4) | temp2);
    }

    janFill.release();
    retVal[decodeLength] = 0;
    return retVal;
}


// -----------------------------------------------------------------------
//  Helper methods
// -----------------------------------------------------------------------
bool HexBin::isHex(const XMLCh& octet)
{
    // sanity check to avoid out-of-bound index
    if ( octet >= BASELENGTH )
        return false;

    return (hexNumberTable[octet] != (XMLByte) -1);
}

void HexBin::init()
{
    if ( isInitialized )
        return;

    int i;
    for ( i = 0; i < BASELENGTH; i++ )
        hexNumberTable[i] = (XMLByte)-1;

    for ( i = chDigit_9; i >= chDigit_0; i-- )
        hexNumberTable[i] = (XMLByte) (i - chDigit_0);

    for ( i = chLatin_F; i >= chLatin_A; i-- )
        hexNumberTable[i] = (XMLByte) (i - chLatin_A + 10);
 
    for ( i = chLatin_f; i >= chLatin_a; i-- )
        hexNumberTable[i] = (XMLByte) (i - chLatin_a + 10);    

    isInitialized = true;
}

XERCES_CPP_NAMESPACE_END
