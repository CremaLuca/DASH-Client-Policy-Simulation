#include "policy.hpp"
#include "request.hpp"
#include "server.hpp"
#include "response.hpp"
#include "MPD.hpp"
#include <utility>
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>

#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client
{

private:
    Server *_server;
    const float SEGMENT_LEN = 0.5;

    /**
     * Calculates the number of consecutive segments in the buffer starting from 0.
     * @param buffer array of downloaded segments ready for playout.
     * @param maxLength length of the buffer.
     */
    static int getConsecutiveBufferSegmentCount(Segment **buffer, int maxLength)
    {
        int i;
        for (i = 0; buffer[i] != nullptr && i < maxLength; i++);
        return i;
    }

    /**
     * Calculates quality of the media streaming.
     * @param buffer segments buffer.
     * @param delays array of delays, same length of buffer.
     * @param maxLength length of the buffer.
     */
    static float calcQuality(Segment **buffer, float *delays, int maxLength)
    {
        float totQuality = buffer[0]->getQuality() - exp(delays[0]);
        float totSegQuality = buffer[0]->getQuality();
        float totQualityDiff = 0;
        float totDelayTime = delays[0];
        float totDelayCost = exp(delays[0]);
        float qualityDiff = 0;
        for (int i = 1; buffer[i] != nullptr && i < maxLength; i++)
        {
            qualityDiff = abs(buffer[i]->getQuality() - buffer[i-1]->getQuality());
            totQuality += buffer[i]->getQuality() - (0.5 * qualityDiff) - exp(delays[i]); // exp(0.5 * delays[i] / 0.5)
            totSegQuality += buffer[i]->getQuality();
            totQualityDiff += qualityDiff;
            totDelayTime += delays[i];
            totDelayCost += exp(delays[i]);
        }
        printf("Segm quality sum: %.2f\nSegm quality diff sum: 0.5 * %.2f\nDelay time sum: %.2fs\nDelay costs sum: %.2f\n",totSegQuality, totQualityDiff,totDelayTime, totDelayCost);
        return totQuality;
    }

    /**
     * Creates a 'Policy.txt' file containing one request log per line.
     * @param requests Vector of the requests made by the client.
     */
    static void writeRequestsFile(std::vector<Request *> requests){
        std::ofstream policyFile;
        policyFile.open("Policy.txt");
        policyFile << "t p i e\n";
        for(int i=0; i < requests.size();i++){
            policyFile << std::to_string(requests[i]->getTime()) + " ";
            policyFile << (requests[i]->getPlaying()) ? std::string("1") : std::string("0");
            policyFile << " ";
            policyFile << std::to_string(requests[i]->getIndex()) + " ";
            policyFile << std::to_string(requests[i]->getEncoding()) + "\n";
        }
        policyFile.close();
    }

public:
    Client(Server *server) : _server(server)
    {
    }

    /**
     * Simulates the playout of a video using the given policy.
     * 
     * @param groups MPD group of segments.
     * @param policy Execution policy, decides whether to make a request and the quality.
     * @returns TODO percieved quality.
     */
    float execute(std::vector<MPDGroup *> MPDgroups, Policy *policy)
    {
        int mediaSegLen = MPDgroups.size();
        float mediaTime = mediaSegLen * SEGMENT_LEN; // Length of the media.
        float time = 0; // Simulation time, only depends on segments transfer time.
        float playTime = 0; // Playout time of the media, depends on segments transfer time and freezes.
        bool isPlaying = false;
        float readyPlayTime = 0; // Time of the media ready to be played.
        std::vector<Request *> requests;
        std::vector<Response *> responses;
        Segment **buffer = new Segment *[mediaSegLen];
        float *delays = new float[mediaSegLen];
        for (int i=0;i<mediaSegLen;i++){ // Array initialisation
            buffer[i] = nullptr;
            delays[i] = 0;
        }
        while (playTime < mediaTime)
        {
            // Ask the policy what resource to require to the server.
            std::pair<int,int> req_values = policy->createRequest(playTime, responses, buffer, MPDgroups);
            readyPlayTime = getConsecutiveBufferSegmentCount(buffer,mediaSegLen) * SEGMENT_LEN - playTime;
            if(req_values.first != -1){
                // Build a request from the pair given by the policy.
                Request *req = new Request(req_values.first, time, isPlaying, req_values.second);
                // Get a response from the server containing the bitrate.
                Response *res = _server->getMedia(req);

                // Update the time
                time += res->getTransferTime();
                if (res->getTransferTime() < readyPlayTime)
                {
                    // If the tranfer time is less than the available playtime it means that at the time of
                    // the next request the video will be playing.
                    isPlaying = true;
                    playTime += res->getTransferTime();
                }
                else
                {
                    // Video has freezed.
                    isPlaying = false;
                    playTime += readyPlayTime;
                    printf("Video freezed[%d]: ready: %.2f, trans: %.2f, delay: %.2f, codec: %d\n",req->getIndex(), readyPlayTime, res->getTransferTime(), res->getTransferTime() - readyPlayTime, req->getEncoding());
                    delays[req->getIndex()] = res->getTransferTime() - readyPlayTime;
                }
                // Save data from this request for next requests
                requests.push_back(req);
                responses.push_back(res);
                if(req->getIndex() * SEGMENT_LEN >= playTime){
                    buffer[req->getIndex()] = res->getSegment();
                }else{
                    // If it's been requested an old segment and it didn't make it in time to be played, it's discarded.
                    printf("Discarded segment %d, it's already been played, cannot override it (playTime: %.2f -> currentSegm: %d)\n",res->getSegment()->getIndex(), playTime, (int)(playTime*2));
                }
            }else{
                // Required segment is -1 means fast forward to the end of the playout.
                time += readyPlayTime;
                playTime += readyPlayTime;
            }
        }
        // End of simulation
        float quality = calcQuality(buffer, delays, mediaSegLen);
        writeRequestsFile(requests);
        // Cleanup
        delete [] buffer;
        for (int i = 0; i < responses.size(); i++)
        {
            delete responses[i];
        }
        for (int i = 0; i < requests.size(); i++)
        {
            delete requests[i];
        }
        return quality;
    }
};

#endif