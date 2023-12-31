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

  cout << argv[1] << endl;
  ifstream is( argv[1], std::ifstream::binary );


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
  bool isFT    = false;  // on if FT is reached and set off by the header
  int i = 0;             // just a counter
  bool debug = false;     // printing stuff
  char prev = cout.fill(); // for debug formatting

  // store data
  vector<uint16_t> sample;
  vector<uint16_t> channel;
  vector<uint16_t> amplitude;
 
  TFile fout( "ftest.root", "recreate");
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

    // FEU header
    // ---------
    if ( is_Feu_header( data ) ){
      // we enter here the first time the data is a FEU header
      // then we loop over all the FEU header data

      isEvent = true; // we are in an event
      isFT = false;

      timestamp = 0;      
      delta_timestamp = 0;      
      eventID   = 0;      
      FeuID     = 0;
      sampleID  = 0;

      // loop over the FEU header data
      while ( is_Feu_header( data ) ){
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
          eventID  += get_Event_ID( data )<<12;
        }
        else if( iFeuH == 5 ){
          timestamp +=  get_timestamp( data )<<12;
        }
        else if( iFeuH == 6 ){
          timestamp +=  get_timestamp( data )<<24;
        }
        else if( iFeuH == 7 ){
          timestamp +=  (uint64_t)get_timestamp( data )<<36;
        }

        iFeuH++;
        // there are maximum 8 header lines
        if( iFeuH < 8 ) { if( read16( is, data ) ) break; }
        else if ( iFeuH > 8 ){ cerr << "ERROR: Too many FEU headers " << endl; break; }
        else { break; }
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

      // end of the header lines, we can now reset the counter of the headers
      iFeuH=0;
    }
    else if( is_data_zs( data ) && isEvent && ! isFT ) {
      // read data
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


      //stripValues = { dreamID*64 + channelID, sampleID, amplitude };
      //stripSamples.push_back( stripValues );
      channel.push_back( dreamID*64 + channelID );
      sample.push_back( sampleID );
      amplitude.push_back( ampl );
      
    }
    else if ( is_final_trailer( data ) ){
      // read the final trailer
      // =====================
 
      isFT = true;
      // check if this is the end of the event (EoE)
      if(  get_EoE(data) == 1  ) { 

        delta_timestamp = timestamp - old_timestamp;
        old_timestamp = timestamp;
        // fill the three
        auto a = nt.Fill();

        // reset all
        isEvent = false;

        channel.clear();
        sample.clear();
        amplitude.clear();

        if( i==0 ){
          cout << " reading FEU " << FeuID << endl;
        }

        i++; // count events;
      }


      iFeuH = 0; // reset the FEU header counter

      if( debug ){
        prev = cout.fill('0');
        cout << "FT "<< setw(4)  << hex << data << "   ";
      }

      // read one additional line for the VEP
      read16(is,data);

      if( debug ){
        cout << setw(4)  << hex << data<< "  " << isEvent << endl;
        cout << dec  ;
        cout.fill( prev);
      }
    


    }
    else{ 
      if( debug ) {
        cout << "other ... " ;
        print_data(data);
      }
    }

    // read next block of 16bits
    if( read16( is, data ) ) break;
    //print_data( data );

  }

  cout << " Events analysed : " << i << endl;

  is.close();
  fout.Write();
  //fout.SetName( Form("FeuID%d.root", FeuID) );
  fout.Close();
  return 0;
}

