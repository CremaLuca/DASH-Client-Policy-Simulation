#include <stdio.h>

#ifndef REQUEST_HPP
#define REQUEST_HPP

class Request
{

private:
    int _index;
    float _time;
    bool _playing;
    int _encoding;

public:
    /**
     * @param index Segment indexRequest(int index, float time, bool playing, int encoding) : _index(index), _time(time), _playing(playing), _encoding(encoding)
     * @param time Request time
     * @param playing If there is something else playing at t time
     * @param encoding Encoding level index [0,1,2,3,4]
     */
    Request(int index, float time, bool playing, int encoding) : _index(index), _time(time), _playing(playing), _encoding(encoding)
    {}

    /**
     * @return A char array containing a line for the output file
     */
    void getOutput(char *buffer)
    {
        sprintf(buffer, "%f %d %d %d", _time, _playing, _index, _encoding);
    }

    int getIndex()
    {
        return _index;
    }

    float getTime()
    {
        return _time;
    }

    bool getPlaying()
    {
        return _playing;
    }

    /**
     * @returns encoding level, between 0 and 4
     */
    int getEncoding()
    {
        return _encoding;
    }

    void setPlaying(bool p)
    {
        _playing = p;
    }
};

#endif
