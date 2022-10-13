/**
 *  varStore.cpp
 *
*/

#include "varStore.h"

// WARNING: the arrays and strings are just written into storage space.
// It is up to the user to monitor their length:
// possible overflow into space used by other variables is not
// detected (nor detectable!)
// Overflow out of the storage area is prevented (array/string is truncated).

/// Write n-byte data array
template<typename Tadr, Tadr bytesize>
void varStore<Tadr,bytesize>::
setB(Tadr adr, byte *src, Tadr maxlen)
{
    Tadr cc;
    cc = 0;
    if(adr >= bytesize) return;
    do {
        _store[adr++] = *(src++);
        cc++;
    } while ((cc < maxlen) && (adr < bytesize));
}
/// Read byte array into supplied buffer.
/// Read up to <maxlen> chars (may be less if the end of storage area is reached).
//  Warning: it is up to the user to supply a large enough buffer
template<typename Tadr, Tadr bytesize>
void varStore<Tadr,bytesize>::
getB(Tadr adr, byte *dst, Tadr maxlen)
{
    Tadr cc = 0;
    do {
        *(dst++) = _store[adr++];
        cc++;
    } while ((cc < maxlen) && (adr < bytesize) && (adr != 0));
}
/// Write null-terminated string (including final NUL)
/// Up to <maxlen> chars are copied (NUL included)
template<typename Tadr, Tadr bytesize>
void varStore<Tadr,bytesize>::
setS(Tadr adr, char *src, Tadr maxlen)
{
    Tadr cc = 1;    // Count 1 char less to account for NUL
    if(adr >= bytesize) return;
    do {
        _store[adr++] = *(src++);
        cc++;
    } while ((*src != 0) && (cc < maxlen) && (adr < bytesize));
    if(adr == bytesize) adr--;
    _store[adr] = 0;   // Terminate string
}

/// Read null-terminated string (including final NULL) into supplied buffer.
/// Up to <maxlen> chars are copied (NUL included).
/// The string may be truncated if the end of storage area is reached
/// (in this case the NUL terminator is added anyway).
//  Warning: it is up to the user to supply a large enough buffer
template<typename Tadr, Tadr bytesize>
void varStore<Tadr,bytesize>::
getS(Tadr adr, char *dst, Tadr maxlen)
{
    Tadr cc = 1;    // Count 1 char less to account for NUL
    do {
        *dst++ = _store[adr++];
        cc++;
    } while ((_store[adr]!=0) && (cc < maxlen) && (adr < bytesize));
    *dst = 0;   // Terminate string
}

// END varStore.cpp
