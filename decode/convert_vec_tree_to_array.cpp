#include <iostream>
#include <vector>
#include "TFile.h"
#include "TTree.h"

void convertTree(const char* inputFileName = "ftest.root", const char* outputFileName = "ftest_vec.root", int maxSamples = -1, int maxChannels = -1) {
    TFile fin(inputFileName);
    TTree* inputTree = dynamic_cast<TTree*>(fin.Get("nt"));
    if (!inputTree) {
        std::cerr << "Error: Input tree not found." << std::endl;
        return;
    }
    
    uint64_t timestamp = 0;
    uint64_t delta_timestamp = 0;
    uint16_t fine_timestamp = 0;
    uint64_t eventID = 0;
    std::vector<uint16_t> *sample = nullptr;
    std::vector<uint16_t> *channel = nullptr;
    std::vector<uint16_t> *amplitude = nullptr;

    TFile inputFile(inputFileName);
    inputTree->SetBranchAddress("eventId", &eventID);
    inputTree->SetBranchAddress("timestamp", &timestamp);
    inputTree->SetBranchAddress("delta_timestamp", &delta_timestamp);
    inputTree->SetBranchAddress("ftst", &fine_timestamp);
    inputTree->SetBranchAddress("sample", &sample);
    inputTree->SetBranchAddress("channel", &channel);
    inputTree->SetBranchAddress("amplitude", &amplitude);

    if (maxSamples == -1 || maxChannels == -1) {
        int nEntries = inputTree->GetEntries();
        for (int i = 0; i < nEntries; ++i) {
            inputTree->GetEntry(i);
            for (size_t j = 0; j < sample->size(); ++j) {
                if (maxSamples == -1 || (*sample)[j] > maxSamples) {
                    maxSamples = (*sample)[j];
                }
                if (maxChannels == -1 || (*channel)[j] > maxChannels) {
                    maxChannels = (*channel)[j];
                }
            }
        }
    }

    TFile fout(outputFileName, "recreate");
    TTree nt("nt", "nt");

    uint16_t amp[maxChannels + 1][maxSamples + 1];

    nt.Branch("eventId", &eventID);
    nt.Branch("timestamp", &timestamp);
    nt.Branch("delta_timestamp", &delta_timestamp);
    nt.Branch("ftst", &fine_timestamp);
    nt.Branch("amp", amp, Form("amp[%d][%d]/s", maxChannels + 1, maxSamples + 1));

    int nEntries = inputTree->GetEntries();
    for (int i = 0; i < nEntries; ++i) {
        inputTree->GetEntry(i);
        for (int ch = 0; ch <= maxChannels; ++ch) {
            for (int s = 0; s <= maxSamples; ++s) {
                amp[ch][s] = 0; // Initialize all elements to zero
            }
        }
        for (size_t j = 0; j < sample->size(); ++j) {
            amp[(*channel)[j]][(*sample)[j]] = (*amplitude)[j];
        }
        nt.Fill();
    }

    fout.Write();
    fout.Close();
}


int main(int argc, char* argv[]) {
    const char* inputFileName = "ftest.root";
    const char* outputFileName = "ftest_array.root";
    int maxSamples = -1;
    int maxChannels = -1;

    if (argc >= 2) {
        inputFileName = argv[1];
    }
    if (argc >= 3) {
        outputFileName = argv[2];
    }
    if (argc >= 4) {
        maxSamples = std::atoi(argv[3]);
    }
    if (argc >= 5) {
        maxChannels = std::atoi(argv[4]);
    }

    convertTree(inputFileName, outputFileName, maxSamples, maxChannels);

    return 0;
}