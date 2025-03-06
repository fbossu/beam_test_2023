#include <iostream>
#include <iomanip>
#include <fstream>
#include <bitset>
#include <vector>
#include <map>
#include <array>
using namespace std;

#include "TFile.h"
#include "TTree.h"

#include <arpa/inet.h>
#include "dreamdataline.h"

bool read16( ifstream &is, uint16_t &data ){
    is.read( (char*) &data, sizeof(data) );
    
    data = ntohs( data );

    return is.eof();
}

void print_data( uint16_t data ){

    bitset<16> x(data);

    cout << setw(20)<< x << setw(5) 
      << is_final_trailer(data) << setw(5) 
      << is_Feu_header(data) << setw(5) 
      << is_data(data) << setw(5) 
      << is_data_header(data) << setw(5) 
      << is_data_trailer( data ) ;
      cout << endl;
}

int main( int argc, const char **argv) {

  if (argc < 2) {
	cout << "Usage: " << argv[0] << " <filename>" << endl;
    return 1;
  }
  cout << argv[1] << endl;
  ifstream is( argv[1], std::ifstream::binary );
  if (!is) {
	cout << "Cannot open " << argv[1] << endl << "Exiting." << endl;
	return 1;
  }

  if (is.peek() == ifstream::traits_type::eof()) {
      cout << "File is empty " << argv[1] << endl << "Exiting." << endl;
      return 1;
  }

  string outputFileName = "ftest.root";
  if (argc == 3) {
	  outputFileName = argv[2];
	  if (outputFileName.find(".root") == string::npos) {
		  outputFileName += ".root";
	  }
  }

  // this is where we store the 2 bytes of each data line
  uint16_t data = 0;

  // data fields
  short    iFeuH = 0;
  uint64_t timestamp = 0;      
  uint64_t old_timestamp = 0;      
  uint64_t delta_timestamp = 0;      
  uint16_t fine_timestamp = 0;
  uint64_t eventID   = 0;      
  uint32_t FeuID = 0;
  uint16_t sampleID = 0;
  uint16_t channelID = 0;
  uint16_t dreamID = 0;
  uint16_t ampl = 0;

  // useful vairables
  bool isEvent = false;  // check the end of the event
  bool isFT    = false;  // on if FT (Final Trailer) is reached and set off by the header
  bool isZS    = true;   // true if zero suppressed data. false if not
  int i = 0;             // just a counter
  bool debug = false;     // printing stuff
  char prev = cout.fill(); // for debug formatting

  // store data
  vector<uint16_t> sample;
  vector<uint16_t> channel;
  vector<uint16_t> amplitude;
 
  TFile fout(outputFileName.data(), "recreate");
  TTree nt("nt","nt");
  nt.SetDirectory(&fout);
  nt.Branch("eventId",&eventID );
  nt.Branch("timestamp",&timestamp);
  nt.Branch("delta_timestamp",&delta_timestamp);
  nt.Branch("ftst",&fine_timestamp);
  nt.Branch("sample",&sample);
  nt.Branch("channel",&channel);
  nt.Branch("amplitude",&amplitude);

  // loop over the file
  while( true ){

    //if( i > 600 ) break;
    //if( i > 574 ) debug = true;
    //if (debug && (i > 0)) break;

    // FEU header
    // ---------
    if ( is_Feu_header( data ) ){
      // we enter here the first time the data is a FEU header
      // then we loop over all the FEU header data

      isEvent = true; // we are in an event
      isFT = false;
      isZS = get_zs_mode(data);  // true if zero supressed data, false otherwise

      timestamp = 0;      
      delta_timestamp = 0;      
      eventID   = 0;      
      FeuID     = 0;
      sampleID  = 0;
      iFeuH = 0;

      if (debug) cout << "Start FEU header..." << endl;
      // loop over the FEU header data
      while ( is_Feu_header( data ) ){
        if (debug) {
            print_data(data);
        }
        if( iFeuH == 0 ){
          FeuID = get_Feu_ID( data );
          sampleID = data & 0x800;
        }
        else if( iFeuH == 1 ){
          eventID =  get_Event_ID( data );
        }
        else if( iFeuH == 2 ){
          timestamp =  get_timestamp( data );
        }
        else if( iFeuH == 3 ) {
          sampleID += get_sample_ID( data ); 
          fine_timestamp = get_fine_timestamp( data );
        }
        else if( iFeuH == 4 ){
          eventID  += (uint64_t)get_Event_ID( data )<<12;
        }
        else if( iFeuH == 5 ){
          timestamp +=  (uint64_t)get_timestamp( data )<<12;
        }
        else if( iFeuH == 6 ){
          timestamp +=  (uint64_t)get_timestamp( data )<<24;
        }
        else if( iFeuH == 7 ){
          timestamp +=  (uint64_t)get_timestamp( data )<<36;
        }

        iFeuH++;
        if (read16(is, data)) break;
      }

      if ( debug ){
        cout
          << " * FEU H"
          << setw(6) << FeuID 
          << setw(10) << eventID
          << setw(6) << sampleID
          << setw(20) << timestamp
          << setw(5) << fine_timestamp
          << " === " << iFeuH<<endl;
      }

    }
    else if( isZS && is_data( data ) && isEvent && ! isFT ) {
      // read zero-suppressed data
      // =======================================
      // first line dreamId and channel Id
      // second line channel data
      // isEvent is to avoid reading a empty line after the EoE
     
      channelID = get_channel_ID( data );
      dreamID   = get_dream_ID_ZS( data );

      if( debug ){
        print_data(data);
        prev = cout.fill('0');
        cout << isEvent << "  "  << setw(4)  << hex << data << "   "; 
      }
      // read next line
      read16(is,data);

      ampl = get_data( data );

      if( debug ){
        cout << setw(4)  << hex << data << endl;
        cout << dec ;
        cout.fill( prev);
        print_data(data);
      }


      channel.push_back( dreamID*64 + channelID );
      sample.push_back( sampleID );
      amplitude.push_back( ampl );
      if (read16(is, data)) break;
      
    }
    else if (!isZS && is_data_header(data) && isEvent && !isFT) {
        // read non-zero suppressed data
        // =======================================
        // first 4 words raw header, first 3 seem to be trigger id, skip
        // word 4 contains dream id
        // 5th to 68th words are channel data for channels 0-63
        // 69th to 74th words raw trailer, skip
        // isEvent is to avoid reading a empty line after the EoE

        int data_header_num = 0;
        bool got_dream_id = false;
        bool eof = false;
        // Raw header word 1 is Trigger Id MSB, 2 is Trigger Id ISB, 3 is Trigger Id LSB, 4 contains Dream Id
        while (is_data_header(data)) {
            data_header_num++;
            if (debug) { cout << "Data header #" << data_header_num << " "; print_data(data); }
            if (data_header_num == 4) {
                dreamID = get_dream_ID(data);  // Contains Dream Id
                got_dream_id = true;
            }
            eof = read16(is, data);
            if (eof)  break;
        }
        if (eof) {
            cout << "End of file reached while reading data header " << data_header_num << ". Exiting early!" << endl;
            break;
        }

        if (!got_dream_id) {
            cout << "Bad read, didn't get dream id from data header." << endl;
            dreamID = 0;
        }

        channelID = 0;
        while (is_data(data)) {
            ampl = get_data(data);

            if (debug) {
                cout << setw(4) << hex << data;
                cout << dec;
                cout.fill(prev);
                cout << "  channel #" << channelID << endl;
                print_data(data);
            }

            channel.push_back(dreamID * 64 + channelID);
            sample.push_back(sampleID);
            amplitude.push_back(ampl);
            channelID++;
            eof = read16(is, data);
            if (eof)  break;
        }
        if (eof) {
            cout << "End of file reached while reading data channel " << channelID << ". Exiting early!" << endl;
            break;
        }

        if (channelID != 64) {
            cout << "Bad read, last channel ID " << channelID << " != 64" << endl;
            if (debug) {
                print_data(data);
                cout << "Press Enter to continue..." << endl;
                cin.get(); // Wait for user to press Enter
                while (!read16(is, data)) {
                    print_data(data);
                }
            }
        }

        int data_trailer_num = 0;
        while (is_data_trailer(data)) {
            data_trailer_num++;
            if (debug) {
                cout << "Data trailer #" << data_trailer_num << " ";
                print_data(data);
            }
            eof = read16(is, data);
        }
        if (eof) break;  // End of file
        
        // Shouldn't be true if successful read. If bad read, read till data header to reset for next event.
        while (!is_final_trailer(data) && !is_data_header(data)) { 
            cout << "Bad read, expected data header but got the following:" << endl;
            print_data(data);
            eof = read16(is, data);
        }
        if (eof) break;  // End of file
    }

    else{ 
        if (debug) {
            cout << "other ... " ;
            print_data(data);
        }
        if (read16(is, data)) break;
    }

    if (is_final_trailer(data)) {
        // read the final trailer
        // =====================

        isFT = true;
        // check if this is the end of the event (EoE)
        if (get_EoE(data) == 1) {

            delta_timestamp = timestamp - old_timestamp;
            old_timestamp = timestamp;
            auto a = nt.Fill();  // fill the tree

            // reset all
            isEvent = false;

            channel.clear();
            sample.clear();
            amplitude.clear();

            if (i == 0) {
                cout << " reading FEU " << FeuID << endl;
            }

            i++;  // count events;
        }

        if (debug) {
            prev = cout.fill('0');
            cout << "FT " << setw(4) << hex << data << "   ";
        }

        read16(is, data);  // read one additional line for the VEP

        if (debug) {
            cout << setw(4) << hex << data << "  " << isEvent << endl;
            cout << dec;
            cout.fill(prev);
        }
        if (read16(is, data)) break;
    }
  }

  cout << " Events analysed : " << i << endl;

  is.close();
  fout.Write();
  //fout.SetName( Form("FeuID%d.root", FeuID) );
  fout.Close();
  return 0;
}

