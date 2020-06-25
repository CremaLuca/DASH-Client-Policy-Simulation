#include "request.hpp"
#include "MPD.hpp"
#include "response.hpp"

#ifndef SERVER_HPP
#define SERVER_HPP

class Server
{
private:
    std::vector<MPDGroup *> _groups;
    std::vector<int> _bitrates;
    int reqCount = 0;

public:
    Server(std::vector<MPDGroup *> groups, std::vector<int>bitrates) : _groups(groups), _bitrates(bitrates)
    {
    }

    /**
     * Calculates the time needed to download the media
     * @param req Requested media
     * @returns TransferStats containing bitrate and transfer time
     */
    Response *getMedia(Request *req)
    {
        int index = req->getIndex();
        int encoding = req->getEncoding();
        float quality = _groups[req->getIndex()]->getEntries()[req->getEncoding()]->getQuality();
        float bitSize = (float)_groups[req->getIndex()]->getEntries()[req->getEncoding()]->getBitSize();
        float bitrate = (float)_bitrates[reqCount++];
        return new Response(req, bitrate, bitSize / bitrate, new Segment(index, encoding, bitSize, quality));
    }
};

#endif