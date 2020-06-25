#include <iostream>
#include <fstream>
#include <vector>
#include "MPD.hpp"
#include "request.hpp"
#include "server.hpp"
#include "client.hpp"

/**
 *  Loads bitrates values from the file, one per line.
 *  @param filename of the bitrates file.
 *  @returns the absolute value of the read line.
 */
std::vector<int> loadFileBitrates(std::string filename){
    std::vector<int> bitrates;
    std::string line;
    std::ifstream bitratesFile(filename);
    while (std::getline(bitratesFile, line))
    {
        bitrates.push_back(std::abs(std::stoi(line)));
    }
    return bitrates;
}

int main()
{
    std::vector<MPDGroup*> MPDgroups = MPDGroup::loadSegmentsFromFile("MPD.txt");
    std::vector<int> bitrates = loadFileBitrates("channel.txt");
    printf("bitrates length: %d\n", bitrates.size());
    printf("MPD segments length: %d\n", MPDgroups.size());
    Server * s = new Server(MPDgroups, bitrates);
    Client * c = new Client(s);
    Policy * p = new CustomPolicy();
    float quality = c->execute(MPDgroups,p);

    // Cleanup
    for(int i=0;i<MPDgroups.size();i++)
        delete MPDgroups[i];
    delete s, c, p;
    
    // Print the results
    printf("Percieved quality: %.2f \n", quality);
}

