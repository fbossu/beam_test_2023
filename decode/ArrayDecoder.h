#include <array>

#include "TFile.h"
#include "TTree.h"

#include "Decoder"

class ArrayDecoder : public DataDecoder {
public:
    // Constructor
    ArrayDecoder(const std::string& filename) : Decoder(filename) {}

private:
    // Override method to store decoded data using arrays
    void StoreDecodedData(int dreamID, int channelID, int ampl) override {
        // Store decoded data in arrays
        sample_[eventCount_] = sampleID_;
        channel_[eventCount_] = dreamID * 64 + channelID;
        amplitude_[eventCount_] = ampl;
    }

    // Maximum number of events
    static constexpr int kMaxEvents = 1000;

    // Arrays to store decoded data
    std::array<uint16_t, kMaxEvents> sample_;
    std::array<uint16_t, kMaxEvents> channel_;
    std::array<uint16_t, kMaxEvents> amplitude_;

    // Current event count
    int eventCount_ = 0;
};
