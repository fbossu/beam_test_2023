
struct hit {
  uint16_t channel;
  uint16_t maxamp;
  uint16_t samplemax;
  float    inflex;
  uint16_t clusterId;

};

struct cluster {
  float    centroid;
  uint16_t size;
  uint16_t id;
};

void timeVsStrip() {


  TFile *inf = TFile::Open( "rec_ftest_POS13_FEU4.root" );

  TTreeReader reader("events", inf);

  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");
  TTreeReaderValue< std::vector<cluster> > cls( reader, "clusters");

  int dreamID = 7;

  TH1F *hcentroid = new TH1F("hcentroid", "centroid", 4*64,0,64);

  while( reader.Next() ){

    if( hits->size() == 0 ) continue;

    for( auto c : *cls ){
      if( floor( c.centroid/64 ) != dreamID ) continue;

      hcentroid->Fill( c.centroid - dreamID*64);
    }

  }

  hcentroid->Draw();


}


