
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


  int dreamID = 7;
  TFile *inf = TFile::Open( "rec_ftest_POS13_FEU4.root" );

  TTreeReader reader("events", inf);

  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");
  TTreeReaderValue< std::vector<cluster> > cls( reader, "clusters");


  TH1F *hcentroid = new TH1F("hcentroid", "centroid", 4*64,0,64);
  hcentroid->SetXTitle("centroid");
  TH2F *hsize = new TH2F("hsize", "size", 4*64,0,64, 20,0,20);
  hsize->SetXTitle("centroid");
  hsize->SetYTitle("cluster size");
  TH2F *h2c = new TH2F("h2c", "centroid", 4*64,0,64,4*64,0,64);
  h2c->SetXTitle("centroid");
  h2c->SetYTitle("centroid");


  while( reader.Next() ){

    if( hits->size() == 0 ) continue;

    for( auto c : *cls ){
      if( floor( c.centroid/64 ) != dreamID ) continue;

      hcentroid->Fill( c.centroid - dreamID*64);
      hsize->Fill( c.centroid - dreamID*64, c.size);

    }

    if( cls->size() > 1){
      for( auto icl = cls->begin(); icl < cls->end(); icl++){
        if( floor( icl->centroid/64 ) != dreamID ) continue;
        for( auto jcl = icl + 1; jcl < cls->end(); jcl++ ){
          if( floor( jcl->centroid/64 ) != dreamID ) continue;
          h2c->Fill( icl->centroid - dreamID*64, jcl->centroid - dreamID*64 );
        }
      }

    }


  }

  auto c = new TCanvas();
  c->Divide(2,2);
  c->cd(1);
  hcentroid->Draw();
  c->cd(2)->SetLogz();
  h2c->Draw("colz");
  c->cd(3)->SetLogz();
  hsize->Draw("colz");
  c->cd(4)->SetLogz();
  hsize->ProjectionY()->Draw();


}


