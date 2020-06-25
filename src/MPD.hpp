#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#define VALUES_DELIMITER ' '

#ifndef MDPSEGMENT_HPP
#define MDPSEGMENT_HPP

/**
 * Defines a single line of the MPD.
 */
class Segment
{

private:
    int _index;
    int _encoding;
    int _bitSize;
    float _quality;

public:
    /**
     * @param index of the segment.
     * @param encoding Level of compression [0,1,2,3,4].
     * @param bitSize of the entry.
     * @param quality percieved [0-5].
     */
    Segment(int index, int encoding, int bitSize, float quality) : _index(index), _encoding(encoding), _bitSize(bitSize), _quality(quality) {}

    int getIndex()
    {
        return _index;
    }

    int getBitSize()
    {
        return _bitSize;
    }

    float getQuality()
    {
        return _quality;
    }

    int getEncoding()
    {
        return _encoding;
    }
};

/**
 * Defines a group of different segments with different encoding.
 */
class MPDGroup
{
private:
    static const int ENCODING_LEVELS = 5;

    Segment **_entries;

    static float *extractLineValues(std::string line)
    {
        char delimiter = VALUES_DELIMITER;
        std::string value;
        float *lineValues = new float[3];
        std::stringstream lineStream(line);                              // Creates a stream from the read file line
        for (int j = 0; std::getline(lineStream, value, delimiter); j++) // Splits the line by the delimiter character
        {
            lineValues[j] = std::stof(value);
        }
        return lineValues;
    }

public:
    MPDGroup(Segment **entries) : _entries(entries)
    {
    }

    Segment **getEntries()
    {
        return _entries;
    }

    Segment * getEntry(int index){
        return _entries[index];
    }

    /**
     * Loads the array of entries for each segment.
     */
    static std::vector<MPDGroup *> loadSegmentsFromFile(std::string filename)
    {
        std::string line;
        float *lineValues;
        std::vector<MPDGroup *> segments;
        Segment **segmentEntries;
        std::ifstream segmentsFile(filename);
        while (std::getline(segmentsFile, line))
        {
            segmentEntries = new Segment *[ENCODING_LEVELS];
            for (int c = 0; c < ENCODING_LEVELS; c++)
            {
                lineValues = extractLineValues(line);
                int index = (int)lineValues[0];
                int bitSize = (int)lineValues[1];
                float quality = lineValues[2];
                segmentEntries[c] = new Segment(index, c, bitSize, quality);
                if (c < ENCODING_LEVELS - 1)
                    std::getline(segmentsFile, line);
            }
            segments.push_back(new MPDGroup(segmentEntries));
        }
        return segments;
    }
};

#endif