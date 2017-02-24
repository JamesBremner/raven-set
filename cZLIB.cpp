#include "zlib/zlib.h"
#include "cZLIB.h"

namespace raven {
    namespace set {



    cZLIB::cZLIB()
        : myInflatedLengthMax( 110000 )
        , myBuffer( new unsigned char[ myInflatedLengthMax ] )
    {

    }

    cZLIB::~cZLIB()
    {
        delete[] myBuffer;
    }


    /** Set maximum inflated data length

        @param[in] max new maximum in bytes

        The default constructor handles up to 100K approximately.
        Call this if you want to change the maximum handled
    */
    void cZLIB::InflatedLengthMax( int max )
    {
        delete[] myBuffer;
        myInflatedLengthMax = 1.1 * max;
        myBuffer = new unsigned char[ myInflatedLengthMax ];
    }

    /** Compress

        @param[in] inflatedBuffer contains data to be comnpressed
        @param[in] inflatedLength length of data to be compressed in bytes

        @return true if successful

        The calling code owns the input buffer and is responsible for freeing it.
    */
    bool cZLIB::Compress(
        const char* inflatedBuffer,
        int inflatedLength )
    {
        if( inflatedLength > 0.9 * myInflatedLengthMax )
            return false;
        myResultLength = myInflatedLengthMax;
        int ret = compress(
                      myBuffer,
                      &myResultLength,
                      (unsigned char*)inflatedBuffer,
                      (long unsigned int)inflatedLength
                  );
        if( ret != Z_OK )
        {
            myResultLength = 0;
            return false;
        }
    }

    /** Inflate

        @param[in] compressedBuffer contains data to be inflated
        @param[in] compressedLength length of input compressed data

        @return true if successful

        The calling code owns the input buffer and is responsible for freeing it.
    */

    bool cZLIB::Inflate(
        const char* compressedBuffer,
        int compressedLength )
    {
        myResultLength = myInflatedLengthMax;
        int ret = uncompress(
                      myBuffer,
                      &myResultLength,
                      (const unsigned char*)compressedBuffer,
                      (long unsigned int) compressedLength
                  );
        if( ret != Z_OK )
        {
            myResultLength = 0;
            return false;
        }

        return true;
    }

    }
}
