#include <iostream>
#include <iomanip>
#include <fstream>
#include <bitset>
#include <vector>
#include <map>
#include <array>

#include "TFile.h"
#include "TTree.h"

class Decoder {
public:
    Decoder(const std::string& filename) : filename_(filename) {}

    void Decode() {
        std::ifstream is(filename_, std::ifstream::binary);
        if (!is.is_open()) {
            std::cerr << "Failed to open file!" << std::endl;
            return;
        }

        // Loop over the file
        while (true) {
            // Read 16-bit data
            if (read16(is, data_)) break;

            // Check different conditions and decode accordingly
            if (is_Feu_header(data_)) {
                // Decode FEU header
                DecodeFeuHeader(is);
            }
            else if (isZS_ && is_data(data_) && isEvent_ && !isFT_) {
                // Decode zero-suppressed data
                DecodeZeroSuppressedData(is);
            }
            else if (!isZS_ && is_data_header(data_) && isEvent_ && !isFT_) {
                // Decode non-zero suppressed data
                DecodeNonZeroSuppressedData(is);
            }
            else {
                // Other cases
                if (debug_) {
                    std::cout << "other ... ";
                    print_data(data_);
                }
                read16(is, data_);
            }

            // Check for final trailer
            if (is_final_trailer(data_)) {
                DecodeFinalTrailer(is);
            }
        }

        // Clean up
        is.close();
        fout_.Write();
        fout_.Close();

        std::cout << "Events analyzed: " << eventCount_ << std::endl;
    }

private:
    std::string filename_;
    uint16_t data_ = 0;

    // Data fields
    short iFeuH_ = 0;
    uint64_t timestamp_ = 0;
    uint64_t old_timestamp_ = 0;
    uint64_t delta_timestamp_ = 0;
    uint16_t fine_timestamp_ = 0;
    uint64_t eventID_ = 0;
    uint32_t FeuID_ = 0;
    uint16_t sampleID_ = 0;
    uint16_t channelID_ = 0;
    uint16_t dreamID_ = 0;
    uint16_t ampl_ = 0;

    // Useful variables
    bool isEvent_ = false;
    bool isFT_ = false;
    bool isZS_ = true;
    int eventCount_ = 0;
    bool debug_ = false;

    // Store data
    std::vector<uint16_t> sample_;
    std::vector<uint16_t> channel_;
    std::vector<uint16_t> amplitude_;

    // ROOT file
    TFile fout_( "ftest.root", "recreate" );
    TTree nt_( "nt", "nt" );

    // Helper functions
    bool read16(std::ifstream& is, uint16_t& data) {
        is.read(reinterpret_cast<char*>(&data), sizeof(data));
        data = ntohs(data);
        return is.eof();
    }

    void DecodeFeuHeader(std::ifstream& is) {
        // Reset variables for FEU header
        iFeuH_ = 0;
        timestamp_ = 0;
        delta_timestamp_ = 0;
        eventID_ = 0;
        FeuID_ = 0;
        sampleID_ = 0;

        // Loop over the FEU header data
        while (is_Feu_header(data_)) {
            if (debug_) {
                print_data(data_);
            }
            switch (iFeuH_) {
            case 0:
                FeuID_ = get_Feu_ID(data_);
                sampleID_ = data_ & 0x800;
                break;
            case 1:
                eventID_ = get_Event_ID(data_);
                break;
            case 2:
                timestamp_ = get_timestamp(data_);
                break;
            case 3:
                sampleID_ += get_sample_ID(data_);
                fine_timestamp_ = get_fine_timestamp(data_);
                break;
            case 4:
                eventID_ += get_Event_ID(data_) << 12;
                break;
            case 5:
                timestamp_ += get_timestamp(data_) << 12;
                break;
            case 6:
                timestamp_ += get_timestamp(data_) << 24;
                break;
            case 7:
                timestamp_ += static_cast<uint64_t>(get_timestamp(data_)) << 36;
                break;
            }

            ++iFeuH_;
            if (read16(is, data_)) break;
        }

        if (debug_) {
            std::cout << " * FEU H" << std::setw(6) << FeuID_ << std::setw(10) << eventID_
                << std::setw(6) << sampleID_ << std::setw(20) << timestamp_
                << std::setw(5) << fine_timestamp_ << " === " << iFeuH_ << std::endl;
        }
    }

    void DecodeZeroSuppressedData(std::ifstream& is) {
        // Read dreamId and channel Id
        channelID_ = get_channel_ID(data_);
        dreamID_ = get_dream_ID_ZS(data_);

        if (debug_) {
            print_data(data_);
            char prev = std::cout.fill('0');
            std::cout << isEvent_ << "  " << std::setw(4) << std::hex << data_ << "   ";
        }

        // Read next line
        read16(is, data_);

        // Read amplitude
        ampl_ = get_data(data_);

        if (debug_) {
            std::cout << std::setw(4) << std::hex << data_ << std::endl;
            std::cout << std::dec;
            std::cout.fill(prev);
            print_data(data_);
        }

        // Store decoded data
        StoreDecodedData(dreamID_, channelID_, ampl_);

        // Read next line
        if (read16(is, data_)) return;
    }

    void DecodeNonZeroSuppressedData(std::ifstream& is) {
        int data_header_num = 0;
        channelID_ = 0;
        dreamID_ = -1;

        // Loop to read raw header
        while (is_data_header(data_)) {
            ++data_header_num;
            if (debug_) std::cout << "Data header #" << data_header_num << " ";
            print_data(data_);

            if (data_header_num == 4) {
                // Contains Dream Id
                dreamID_ = get_dream_ID(data_);
            }
            read16(is, data_);
        }

        if (dreamID_ == -1) {
            std::cout << "Bad read, didn't get dream id from data header." << std::endl;
        }

        // Loop to read channel data
        while (is_data(data_)) {
            ampl_ = get_data(data_);

            if (debug_) {
                std::cout << std::setw(4) << std::hex << data_ << "  channel #" << channelID << std::endl;
                std::cout << std::dec;
                print_data(data_);
            }

            // Store decoded data
            StoreDecodedData(dreamID_, channelID_, ampl_);

            // Increment channel ID
            ++channelID_;

            // Read next line
            if (read16(is, data_)) return;
        }

        if (channelID != 64) {
            std::cout << "Bad read, last channel ID != 64" << std::endl;
        }

        // Loop to skip raw trailer
        bool eof = false;
        int data_trailer_num = 0;
        while (is_data_trailer(data_)) {
            ++data_trailer_num;
            if (debug_) {
                std::cout << "Data trailer #" << data_trailer_num << " ";
                print_data(data_);
            }
            eof = read16(is, data_);
        }

        // Check for end of file
        if (eof) return;

        // Check for unexpected data after trailer
        while (!is_final_trailer(data_) && !is_data_header(data_)) {
            std::cout << "Bad read, expected data header but got the following:" << std::endl;
            print_data(data_);
            eof = read16(is, data_);
        }

        // Check for end of file
        if (eof) return;
    }

    void DecodeFinalTrailer(std::ifstream& is) {
        isFT_ = true;
        // Check if this is the end of the event (EoE)
        if (get_EoE(data_) == 1) {
            delta_timestamp_ = timestamp_ - old_timestamp_;
            old_timestamp_ = timestamp_;
            nt_.Fill(); // Fill the tree

            // Reset variables for the next event
            isEvent_ = false;
            channel_.clear();
            sample_.clear();
            amplitude_.clear();

            // Print FEU ID for the first event
            if (eventCount_ == 0) {
                std::cout << "Reading FEU " << FeuID_ << std::endl;
            }

            ++eventCount_;
        }

        // Read one additional line for the VEP
        read16(is, data_);
    }

    void StoreDecodedData(int dreamID, int channelID, int ampl) {
        // Store decoded data
        channel_.push_back(dreamID * 64 + channelID);
        sample_.push_back(sampleID_);
        amplitude_.push_back(ampl);
    }

    void InitializeTree() {
        nt_.SetDirectory(&fout);
        nt_.Branch("eventId", &eventID);
        nt_.Branch("timestamp", &timestamp);
        nt_.Branch("delta_timestamp", &delta_timestamp);
        nt_.Branch("ftst", &fine_timestamp);
        nt_.Branch("sample", &sample);
        nt_.Branch("channel", &channel);
        nt_.Branch("amplitude", &amplitude);
    }
};

