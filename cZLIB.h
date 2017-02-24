namespace raven {
    namespace set {

/** ZLIB C++ wrapper

Usage:

<pre>

    #include "cZLIB.h"

    {
    // compress data in bigbuffer
    raven::set::cZLIB ZLIB;
    ZLIB.Compress( bigbuffer, sizebigbuffer );

    // use compressed buffer, before ZLIB goes out of scope
    use( ZLIB.Buffer(), ZLIB.Length() );

    }

    ...

    {

    // decompress data in smallbuffer
    raven::set::cZLIB ZLIB;
    ZLIB.Inflate( smallbuffer, sizesmallbuffer )

    // use decompressed data, before ZLIB goes out of scope
    use( ZLIB.Buffer(), ZLIB.Length() );

    }

</pre>

Build:

Download this code ( cZLIB.h and cZLIB.cpp )
and install somewhere in your compiler search path.
Let's assume you install it in folder .../src.

Download the zlib source code from http://www.zlib.net/
Copy all the .c and .h files from the root folder of the zlib source
to a new folder .../src/zlib

Add the files cZLIB.h, cZLIB.cpp and all the files in .../src/zlib
to the IDE project.

Build.

 */
class cZLIB
{
public:

    cZLIB();


    ~cZLIB();


    /** Result buffer after compression or inflation
        @return pointer to buffer containing processed data

        This buffer is owned by cZLIB
        - it will be freed when cZLIB goes out of scope
    */
    char* Buffer()
    {
        return (char*) myBuffer;
    }

    /** Result buffer length */
    int Length()
    {
        return (int) myResultLength;
    }

    /** Set maximum inflated data length

        @param[in] max new maximum in bytes

        The default constructor handles up to 100K approximately.
        Call this if you want to change the maximum handled
    */
    void InflatedLengthMax( int max );


    /** Compress

        @param[in] inflatedBuffer contains data to be comnpressed
        @param[in] inflatedLength length of data to be compressed in bytes

        @return true if successful

        The calling code owns the input buffer and is responsible for freeing it.
    */
    bool Compress(
        const char* inflatedBuffer,
        int inflatedLength );


    /** Inflate

        @param[in] compressedBuffer contains data to be inflated
        @param[in] compressedLength length of input compressed data

        @return true if successful

        The calling code owns the input buffer and is responsible for freeing it.
    */

    bool Inflate(
        const char* compressedBuffer,
        int compressedLength );


private:
    long unsigned int myInflatedLengthMax;
    long unsigned int myResultLength;
    unsigned char* myBuffer;
};

    }
}
