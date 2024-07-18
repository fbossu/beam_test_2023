#include <iostream>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"


// Calculate and subtract pedestals from data. Input data and pedestal files both in vector format.

void subtract_pedestals(const char* input_data_file_name, const char* input_ped_file_name, const char* output_file_name, float n_sigma=3) {
    // Open pedestal file and get tree
    TFile fin_ped(input_ped_file_name);
    TTree* input_ped_tree = dynamic_cast<TTree*>(fin_ped.Get("nt"));
    if (!input_ped_tree) {
        std::cerr << "Error: Input pedestal tree not found." << std::endl;
        return;
    }

    std::vector<uint16_t>* ped_sample = nullptr;
    std::vector<uint16_t>* ped_channel = nullptr;
    std::vector<uint16_t>* ped_amplitude = nullptr;

    input_ped_tree->SetBranchAddress("sample", &ped_sample);
    input_ped_tree->SetBranchAddress("channel", &ped_channel);
    input_ped_tree->SetBranchAddress("amplitude", &ped_amplitude);

    // Map to hold pedestal parameters (mean and sigma) for each channel
    std::map<uint16_t, std::pair<double, double>> pedestal_params;

    // Loop over pedestal tree entries to calculate pedestal mean and sigma for each channel
    int n_ped_entries = input_ped_tree->GetEntries();
    std::map<uint16_t, TH1F*> hist_map;

    for (int i = 0; i < n_ped_entries; ++i) {
        input_ped_tree->GetEntry(i);
        for (size_t j = 0; j < ped_channel->size(); ++j) {
            uint16_t ch = (*ped_channel)[j];
            if (hist_map.find(ch) == hist_map.end()) {
                hist_map[ch] = new TH1F(Form("hist_ch%d", ch), Form("Channel %d Pedestal", ch), 4096, 0, 4096);
            }
            hist_map[ch]->Fill((*ped_amplitude)[j]);
        }
    }

    // Fit each histogram to a Gaussian and store the mean and sigma
    for (auto& kv : hist_map) {
        TF1* fit_func = new TF1(Form("fit_func_ch%d", kv.first), "gaus");
        fit_func->SetParameter(1, 300); // Initial guess for mean
        fit_func->SetParameter(2, 5); // Initial guess for width
        kv.second->Fit(fit_func, "Q0");
        double mean = fit_func->GetParameter(1);
        double sigma = fit_func->GetParameter(2);
        pedestal_params[kv.first] = std::make_pair(mean, sigma);
        delete kv.second; // Clean up histogram
        delete fit_func; // Clean up fit function
    }

    // Open data file and get tree
    TFile fin(input_data_file_name);
    TTree* input_tree = dynamic_cast<TTree*>(fin.Get("nt"));
    if (!input_tree) {
        std::cerr << "Error: Input tree not found." << std::endl;
        return;
    }

    uint64_t timestamp = 0;
    uint64_t delta_timestamp = 0;
    uint16_t fine_timestamp = 0;
    uint64_t event_id = 0;
    std::vector<uint16_t>* sample = nullptr;
    std::vector<uint16_t>* channel = nullptr;
    std::vector<uint16_t>* amplitude = nullptr;

    input_tree->SetBranchAddress("eventId", &event_id);
    input_tree->SetBranchAddress("timestamp", &timestamp);
    input_tree->SetBranchAddress("delta_timestamp", &delta_timestamp);
    input_tree->SetBranchAddress("ftst", &fine_timestamp);
    input_tree->SetBranchAddress("sample", &sample);
    input_tree->SetBranchAddress("channel", &channel);
    input_tree->SetBranchAddress("amplitude", &amplitude);

    // Create output file and tree
    TFile fout(output_file_name, "recreate");
    TTree* nt = new TTree("nt", "nt");

    nt->Branch("eventId", &event_id);
    nt->Branch("timestamp", &timestamp);
    nt->Branch("delta_timestamp", &delta_timestamp);
    nt->Branch("ftst", &fine_timestamp);
    nt->Branch("sample", &sample);
    nt->Branch("channel", &channel);
    nt->Branch("amplitude", &amplitude);

    // Loop over data tree entries and filter based on pedestal mean + 3 sigma
    int n_entries = input_tree->GetEntries();
    for (int i = 0; i < n_entries; ++i) {
        input_tree->GetEntry(i);

        std::vector<uint16_t> filtered_sample;
        std::vector<uint16_t> filtered_channel;
        std::vector<uint16_t> filtered_amplitude;

        for (size_t j = 0; j < sample->size(); ++j) {
            uint16_t ch = (*channel)[j];
            double mean = pedestal_params[ch].first;
            double sigma = pedestal_params[ch].second;
            if ((*amplitude)[j] > mean + 3 * sigma) {
                filtered_sample.push_back((*sample)[j]);
                filtered_channel.push_back((*channel)[j]);
                filtered_amplitude.push_back((*amplitude)[j] - mean);
            }
        }

        if (!filtered_sample.empty()) {
            // Assign stack-allocated vectors to the pointer vectors
            sample->swap(filtered_sample);
            channel->swap(filtered_channel);
            amplitude->swap(filtered_amplitude);
            nt->Fill();
        }
    }

    fout.Write();
    fout.Close();
}

int main(int argc, char* argv[]) {
    const char* input_data_file_name = "data.root";
    const char* input_ped_file_name = "pedestal.root";
    const char* output_file_name = "pedestal_subtracted_data.root";
	float n_sigma = 3;

    if (argc >= 2) {
        input_data_file_name = argv[1];
    }
    if (argc >= 3) {
        input_ped_file_name = argv[2];
    }
    if (argc >= 4) {
        output_file_name = argv[3];
    }
	if (argc >= 5) {
		n_sigma = std::atof(argv[4]);
	}

    subtract_pedestals(input_data_file_name, input_ped_file_name, output_file_name, n_sigma);

    return 0;
}