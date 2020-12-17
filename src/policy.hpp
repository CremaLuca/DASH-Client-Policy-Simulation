/**
 * DASH client policy class and a few implementations.
 * @autor Luca Crema
 * @date 11/06/2020
 */

#include <vector>
#include "request.hpp"
#include "response.hpp"
#include "MPD.hpp"
#include <math.h>
#include <algorithm>
#include <utility>

#ifndef POLICY_HPP
#define POLICY_HPP

class Policy
{

public:
    /**
     * Choses video quality based on previous request bitrates and times and how much playout buffer it's left.
     * @param playTime of the media.
     * @param responses previous responses from server to requests (used for bitrate).
     * @param buffer of loaded segments.
     * @param segments the Media Presentation Descriptor file entries.
     * @returns a pair of <position, encoding level> to request.
     */
    virtual std::pair<int, int> createRequest(float playTime, std::vector<Response *> responses, Segment **buffer, std::vector<MPDGroup *> segments)
    {
        return std::pair<int, int>(-1, -1);
    }
};

/**
 * Requires all the segments at the same quality.
 */
class LowQualityPolicy : public Policy
{
public:
    std::pair<int, int> createRequest(float playTime, std::vector<Response *> responses, Segment **buffer, std::vector<MPDGroup *> segments)
    {
        int i;
        for (i = 0; buffer[i] != nullptr && i < segments.size(); i++)
            ;
        if (i >= segments.size())
        { // Buffer full
            return std::pair<int, int>(-1, -1); // Tell the simulation to stop
        }
        return std::pair<int, int>(i, 4);
    }
};

class CustomPolicy : public Policy
{
private:
    static const int TARGET_BUFFER_SIZE = 12;
    const float BETA = 0.5;
    const float GAMMA = 0.5;
    const float SEGMENT_LEN = 0.5;

    /**
     * Calculate average bitrates of the last elementsCound responses.
     * @param responses Vector of responses containing bitrates.
     * @param elementsCound
     */
    float calcAvgBitrate(std::vector<Response *> responses, int elementsCount)
    {
        float avg_bitrate = 0;
        //Calc avg bitrate
        for (int j = 1; j <= elementsCount; j++)
        {
            avg_bitrate += responses[responses.size() - j]->getBitrate();
        }
        avg_bitrate = avg_bitrate / elementsCount;
        return avg_bitrate;
    }

public:
    std::pair<int, int> createRequest(float playTime, std::vector<Response *> responses, Segment **buffer, std::vector<MPDGroup *> segments)
    {
        int lastIndex; // Last segment index present in buffer
        for (lastIndex = 0; buffer[lastIndex] != nullptr && lastIndex < segments.size(); lastIndex++)
            ;
        if (lastIndex == 0)
            return std::pair<int, int>(lastIndex, 4);

        // Calculate the average bitrate of the last 4 communications.
        float avg_bitrate = calcAvgBitrate(responses, std::min(lastIndex, 4))/1.5;
        if (lastIndex >= segments.size())
        { // Buffer full, it can start improving already-downloaded segments quality if it has enough time
            int position = (int)(playTime * (1/SEGMENT_LEN));
            // Iterate to the end to find if it can improve any segment quality
            while (position < segments.size())
            {
                float oldQuality = buffer[position]->getQuality(); // Current buffer's segment quality
                int oldEncoding = buffer[position]->getEncoding(); // Current buffer's segment encoding
                float prevQuality = buffer[position - 1]->getQuality(); // Previous buffer's segment quality
                float maxScore = 0.1; // It's set to a treshold to avoid too little improvements.
                int requiredEncoding = oldEncoding; // Stores the encoding level to require at the end of the research for a better segment.
                float availableTime = position * SEGMENT_LEN - playTime; // Time before the segment has to be played

                // For every encoding check if it can be downloaded in time AND if it improves quality more than a threshold
                for (int codec = 0; codec < oldEncoding; codec++)
                {
                    float bitSize = (float)segments[position]->getEntries()[codec]->getBitSize();
                    float downloadTime = bitSize / avg_bitrate;
                    float encodingQuality = segments[position]->getEntries()[codec]->getQuality();
                    float qualityIncrease = encodingQuality - oldQuality;
                    float scoreIncrease = qualityIncrease - BETA * std::abs(encodingQuality - prevQuality);
                    if (scoreIncrease > maxScore && downloadTime < availableTime)
                    {
                        maxScore = scoreIncrease;
                        requiredEncoding = codec;
                    }
                }
                // If it could find an encoding that suits better.
                if (requiredEncoding < oldEncoding)
                {
                    return std::pair<int, int>(position, requiredEncoding);
                }
                // No encoding for this position could have done better/can be downloaded in time, skip to the next one.
                position++;
            }
            // If no segment can be improved, tell the simulation to end.
            return std::pair<int, int>(-1, -1);
        }
        // If the buffer is almost empty, load the worst quality.
        if (playTime >= (lastIndex - 1) * SEGMENT_LEN)
        {
            return std::pair<int, int>(lastIndex, 4);
        }
        int requiredEncoding = 4; // Stores the encoding level to require at the end of the research for a better segment.
        float prevQuality = buffer[lastIndex - 1]->getQuality(); // Quality of the segment before the one it's analysed
        float maxScore = -99999;
        float availableTime = (lastIndex * SEGMENT_LEN) - playTime; // Time before the playTime reaches the end of the buffer.

        for (int codec = 2; codec <= 4; codec++)
        {
            float bitSize = (float)segments[lastIndex]->getEntries()[codec]->getBitSize();
            float downloadTime = (bitSize / avg_bitrate);
            float currQuality = segments[lastIndex]->getEntries()[codec]->getQuality();
            float qualityDiff = BETA*abs(prevQuality - currQuality);
            float delay = downloadTime - availableTime / TARGET_BUFFER_SIZE;
            float score = currQuality - qualityDiff - exp(GAMMA*(delay/0.5));

            if (score > maxScore)
            {
                maxScore = score;
                requiredEncoding = codec;
            }
        }
        return std::pair<int, int>(lastIndex, requiredEncoding);
    }
};

#endif