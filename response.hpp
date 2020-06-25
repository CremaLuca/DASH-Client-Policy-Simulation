#include "request.hpp"
#include "MPD.hpp"

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

/**
 * Container for statistics of media transfers.
 */
class Response
{
private:
    Request* _request;
    int _bitrate;
    float _transferTime;
    Segment * _segment;

public:
    Response(Request* request, int bitrate, float transferTime, Segment* segment) : _request(request), _bitrate(bitrate), _transferTime(transferTime), _segment(segment) {}

    ~Response(){
        delete _request, _segment;
    }

    int getBitrate(){
        return _bitrate;
    }

    float getTransferTime(){
        return _transferTime;
    }

    Request* getRequest(){
        return _request;
    }

    Segment * getSegment(){
        return _segment;
    }
};

#endif