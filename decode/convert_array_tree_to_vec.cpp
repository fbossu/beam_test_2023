#include <iostream>
#include <vector>
#include "TFile.h"
#include "TTree.h"

// Convert a tree with arrays to a tree with vectors
// Untested!!!

void convert_tree(const char* input_file_name = "ftest_array.root", const char* output_file_name = "ftest_vec.root", int max_channels = 511, int max_samples = 31) {
    TFile fin(input_file_name);
    TTree* input_tree = dynamic_cast<TTree*>(fin.Get("nt"));
    if (!input_tree) {
        std::cerr << "Error: Input tree not found." << std::endl;
        return;
    }

    uint64_t timestamp = 0;
    uint64_t delta_timestamp = 0;
    uint16_t fine_timestamp = 0;
    uint64_t event_id = 0;
    uint16_t amp[max_channels + 1][max_samples + 1];

    input_tree->SetBranchAddress("eventId", &event_id);
    input_tree->SetBranchAddress("timestamp", &timestamp);
    input_tree->SetBranchAddress("delta_timestamp", &delta_timestamp);
    input_tree->SetBranchAddress("ftst", &fine_timestamp);
    input_tree->SetBranchAddress("amplitude", amp);

    TFile fout(output_file_name, "recreate");
    TTree nt("nt", "nt");

    std::vector<uint16_t> sample;
    std::vector<uint16_t> channel;
    std::vector<uint16_t> amplitude;

    nt.Branch("eventId", &event_id);
    nt.Branch("timestamp", &timestamp);
    nt.Branch("delta_timestamp", &delta_timestamp);
    nt.Branch("ftst", &fine_timestamp);
    nt.Branch("sample", &sample);
    nt.Branch("channel", &channel);
    nt.Branch("amplitude", &amplitude);

    int n_entries = input_tree->GetEntries();
    for (int i = 0; i < n_entries; ++i) {
        input_tree->GetEntry(i);

        sample.clear();
        channel.clear();
        amplitude.clear();

        for (int ch = 0; ch <= max_channels; ++ch) {
            for (int s = 0; s <= max_samples; ++s) {
                if (amp[ch][s] != 0) { // Only add non-zero amplitudes
                    sample.push_back(s);
                    channel.push_back(ch);
                    amplitude.push_back(amp[ch][s]);
                }
            }
        }
        nt.Fill();
    }

    fout.Write();
    fout.Close();
}

int main(int argc, char* argv[]) {
    const char* input_file_name = "ftest_array.root";
    const char* output_file_name = "ftest_vec.root";
    int max_channels = 511;
    int max_samples = 31;

    if (argc >= 2) {
        input_file_name = argv[1];
    }
    if (argc >= 3) {
        output_file_name = argv[2];
    }
    if (argc >= 4) {
        max_samples = std::atoi(argv[3]);
    }
    if (argc >= 5) {
        max_channels = std::atoi(argv[4]);
    }

    convert_tree(input_file_name, output_file_name, max_channels, max_samples);

    return 0;
}