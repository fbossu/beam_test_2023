#include "clusterSize.h"


// void stripMap(TChain* chain, std::string detname, StripTable det){

// }

// void stripMapLims(TChain* chain, std::string detname, StripTable det, std::vector<int> xlim, std::vector<int> ylim){

// }

// // x,y map of the clusters in the detector by position
// void posMap(TChain* chain, std::string detname, StripTable det);
// void posMapLims(TChain* chain, std::string detname, StripTable det, std::vector<int> xlim, std::vector<int> ylim);

void clusterSizeRegion(TChain* chain, std::string detname, StripTable det) {

  std::string graphMap = detname+"_Map.png";
  std::string graphStrip = detname+"_strips.png";
  std::string graphClSize = detname+"_ClSize.png";

  TTreeReader reader(chain);

  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");
  TTreeReaderValue< std::vector<cluster> > cls( reader, "clusters");

  int nbZones = det.nbZone();

  TH1F *hcentroidX[nbZones];
  TH1F *hcentroidY[nbZones];
  TH1F *hclSizeX[nbZones];
  TH1F *hclSizeY[nbZones];

  for(int i=0; i<nbZones; i++){
    std::string labelCentroid = "hcentroid"+std::to_string(i);
    hcentroidX[i] = new TH1F((labelCentroid+"X").c_str(), ("X "+det.zoneLabel(i)).c_str(), 128,0,128);
    hcentroidY[i] = new TH1F((labelCentroid+"Y").c_str(), ("Y "+det.zoneLabel(i)).c_str(), 128,0,128);
    hcentroidX[i]->SetXTitle("strip centroid"); hcentroidY[i]->SetXTitle("strip centroid");

    std::string labelclSize = "hclSize"+std::to_string(i);
    hclSizeX[i] = new TH1F((labelclSize+"X").c_str(), ("X "+det.zoneLabel(i)).c_str(), 10,-0.5,10.5);
    hclSizeY[i] = new TH1F((labelclSize+"Y").c_str(), ("Y "+det.zoneLabel(i)).c_str(), 10,-0.5,10.5);
    hclSizeX[i]->SetXTitle("cluster size"); hclSizeY[i]->SetXTitle("cluster size");
  }

  // TH2F *h2c = new TH2F("h2c", "cluster map", 128,0,128,128,0,128);
  TH2F *h2c = new TH2F("h2c", "cluster map", 360,-129,29,360,-29,129);
  h2c->SetXTitle("centroid on y direction strips");
  h2c->SetYTitle("centroid on x direction strips");

  std::vector<cluster> clX, clY;

  while( reader.Next() ){

    if( hits->size() == 0 ) continue;
    clX.clear();
    clY.clear();

    for( auto c : *cls ){
      if( c.axis == 'x' ){
        clX.push_back(c);
      }else if( c.axis == 'y' ){
        clY.push_back(c);
      }
    }

    for( auto x = clX.begin(); x < clX.end(); x++){
      for(auto y = clY.begin(); y < clY.end(); y++){
        int zone = det.zone(int(x->stripCentroid), int(y->stripCentroid));
        if(zone>=0){
          h2c->Fill(det.posY(y->stripCentroid)[0], det.posX(x->stripCentroid)[1]);
          hcentroidX[zone]->Fill(x->stripCentroid);
          hcentroidY[zone]->Fill(y->stripCentroid);

          hclSizeX[zone]->Fill(x->size);
          hclSizeY[zone]->Fill(y->size);
        }
      }
    }
  }

  // gStyle->SetOptStat(0);

  TCanvas *cclSize = new TCanvas("cclSize", "cclSize", 1600,1300);
  cclSize->Divide(int(sqrt(nbZones)), int(sqrt(nbZones)));
  for(int i=0; i<nbZones; i++){
    cclSize->cd(i+1);
    gPad->SetLogy();
    hclSizeX[i]->Draw();

    hclSizeY[i]->SetLineColor(kRed);
    hclSizeY[i]->Draw("same");
  }
  cclSize->cd(0);
  TLegend *leg = new TLegend(0.87,0.75,0.99,0.8);
  leg->AddEntry(hclSizeX[0],"cluster size in X (bottom)","l");
  leg->AddEntry(hclSizeY[0],"cluster size in Y (top)","l");
  leg->Draw();
  cclSize->Print(graphClSize.c_str(), "png");


  TCanvas *cstrips = new TCanvas("cstrips", "cstrips", 1600,1300);
  cstrips->Divide(int(sqrt(nbZones)), int(sqrt(nbZones)));
  for(int i=0; i<nbZones; i++){
    cstrips->cd(i+1);
    hcentroidX[i]->Draw();

    hcentroidY[i]->SetLineColor(kRed);
    hcentroidY[i]->Draw("same");
  }
  cstrips->cd(0);
  TLegend *legS = new TLegend(0.87,0.75,0.99,0.8);
  legS->AddEntry(hcentroidX[0],"strip centroid in X (bottom)","l");
  legS->AddEntry(hcentroidY[0],"strip centroid in Y (top)","l");
  legS->Draw();
  cstrips->Print(graphStrip.c_str(), "png");

  // gStyle->SetOptStat(0);
  TCanvas *c3 = new TCanvas("c3", "c3", 1000,1000);
  h2c->SetStats(11111);
  h2c->Draw("colz");
  gPad->SetLogz();
  c3->Print(graphMap.c_str(), "png");

}


void clusterSizeLims(TChain* chain, std::string detname, StripTable det, std::vector<int> xlim, std::vector<int> ylim) {

  int zone = det.zone((xlim[0]+xlim[1])/2, (ylim[0]+ylim[1])/2);

  float pitch = det.pitchX(xlim[0]);
  std::string strpitchX = std::to_string(pitch).substr(0,3);
  std::string strpitchY = std::to_string( det.pitchX(ylim[0])).substr(0,3);

  std::string graphMap = detname+"_ref"+std::to_string(zone)+"_Map.png";
  std::string graphStrip = detname+"_ref"+std::to_string(zone)+"_strips.png";
  std::string graphClSize = detname+"_ref"+std::to_string(zone)+"_ClSize.png";
  std::string graphAmpX = detname+"_ref"+std::to_string(zone)+"_AmpX.png";
  std::string graphAmpY = detname+"_ref"+std::to_string(zone)+"_AmpY.png";
  std::string graphampCenter = detname+"_ref"+std::to_string(zone)+"_CenterAmp.png";
  std::string graphMax = detname+"_ref"+std::to_string(zone)+"_Max.png";

  TTreeReader reader(chain);

  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");
  TTreeReaderValue< std::vector<cluster> > cls( reader, "clusters");

  TH1F *hcentroidX = new TH1F("hcentroidX", ("X "+det.zoneLabel(zone)).c_str(), 128,0,128);
  TH1F *hcentroidY = new TH1F("hcentroidY", ("Y "+det.zoneLabel(zone)).c_str(), 128,0,128);
  TH1F *hclSizeX = new TH1F("hclSizeX", ("X "+det.zoneLabel(zone)).c_str(), 10,-0.5,10.5);
  TH1F *hclSizeY = new TH1F("hclSizeY", ("Y "+det.zoneLabel(zone)).c_str(), 10,-0.5,10.5);
  hcentroidX->SetXTitle("strip centroid"); hcentroidY->SetXTitle("strip centroid");
  hclSizeX->SetXTitle("cluster size"); hclSizeY->SetXTitle("cluster size");

  TH1F *hampCenterX = new TH1F("hampCenterX", ("MaxAmp on the center strip "+det.zoneLabel(zone)).c_str(), 400,0,1200);
  TH1F *hampCenterY = new TH1F("hampCenterY", ("MaxAmp on the center strip "+det.zoneLabel(zone)).c_str(), 400,0,1200);
  hampCenterX->SetXTitle("amplitude (ADC counts)"); hampCenterY->SetXTitle("amplitude (ADC counts)");

  TH1F *hampSampleX = new TH1F("hampSampleX", ("sampleMax "+det.zoneLabel(zone)).c_str(), 13,-0.5,12.5);
  TH1F *hampSampleY = new TH1F("hampSampleY", ("sampleMax "+det.zoneLabel(zone)).c_str(), 13,-0.5,12.5);
  hampSampleX->SetXTitle("sample number"); hampSampleY->SetXTitle("sample number");

  TH2F *h2c = new TH2F("h2c", "cluster map", 128,0,128,128,0,128);
  h2c->SetXTitle("centroid on y direction strips");
  h2c->SetYTitle("centroid on x direction strips");

  TH2F *h2ampX[6];
  TH2F *h2ampY[6];
  for(int i=0; i<6; i++){
    std::string label = "h2amp"+std::to_string(i);
    std::string title = "clusters size"+std::to_string(i+1);
    h2ampX[i] = new TH2F((label+"X").c_str(), ("X"+title).c_str(), 13, -6.5, 6.5, 50, 0., 1.1);
    h2ampY[i] = new TH2F((label+"Y").c_str(), ("Y"+title).c_str(), 13, -6.5, 6.5, 50, 0., 1.1);
    h2ampX[i]->SetXTitle("strip number"); h2ampY[i]->SetXTitle("strip number");
    h2ampX[i]->SetYTitle("amplitude normalised"); h2ampY[i]->SetYTitle("amplitude normalised");
  }

  cluster clX, clY;
  std::vector<hit> hX, hY;

  while( reader.Next() ){

    if( hits->size() == 0 ) continue;
    hX.clear(); hY.clear();
    int maxSizeX = 0;
    int maxSizeY = 0;
    clX.id = 0; clX.size = 0;
    clY.id = 0; clY.size = 0;

    for( auto c : *cls ){
      if( c.axis == 'x' and c.stripCentroid>xlim[0] and c.stripCentroid<xlim[1] ){
        if(c.size>maxSizeX) {
          clX = c;
          maxSizeX = c.size;
        }
      }
      else if( c.axis == 'y' and c.stripCentroid>ylim[0] and c.stripCentroid<ylim[1] ){
        if(c.size>maxSizeY) {
          clY = c;
          maxSizeY = c.size;
        }
      }
    }

    for(auto h : *hits){
      if(h.clusterId == clX.id) hX.push_back(h);
      if(h.clusterId == clY.id) hY.push_back(h);
    }

    if( hX.size() > 0 ){
      hcentroidX->Fill(clX.stripCentroid);
      hclSizeX->Fill(clX.size);
      auto maxHit = *std::max_element(hX.begin(), hX.end(),
                             [](const hit& a,const hit& b) { return a.maxamp < b.maxamp; });
      // std::cout<<hX.size()<<" "<<clX.size<<std::endl;
      if(clX.size == 1){
        hampCenterX->Fill(maxHit.maxamp);
        hampSampleX->Fill(maxHit.samplemax);
      }
      else if(clX.size<7){
        for( auto hitx = hX.begin(); hitx < hX.end(); hitx++){
          h2ampX[clX.size-1]->Fill(hitx->strip-maxHit.strip, (float)hitx->maxamp/(float)maxHit.maxamp);
          // std::cout<<hitx->maxamp<<" "<<maxHit.maxamp<<std::endl;
        }
      }
    }


    if( hY.size() > 0 ){
      hcentroidY->Fill(clY.stripCentroid);
      hclSizeY->Fill(clY.size);
      auto maxHit = *std::max_element(hY.begin(), hY.end(),
                             [](const hit& a,const hit& b) { return a.maxamp < b.maxamp; });
      if(clY.size == 1){
        hampCenterY->Fill(maxHit.maxamp);
        hampSampleY->Fill(maxHit.samplemax);
      }
      else if(clY.size<7){
        for( auto hity = hY.begin(); hity < hY.end(); hity++){
          h2ampY[clY.size-1]->Fill(hity->strip-maxHit.strip, (float)hity->maxamp/(float)maxHit.maxamp);
        }
      }
    }

    if(hY.size() > 0 and hX.size()>0) h2c->Fill(clY.stripCentroid, clX.stripCentroid);
  }

  // gStyle->SetOptStat(1111);

  TCanvas *cclSize = new TCanvas("cclSize", "cclSize", 1000,700);
  gPad->SetLogy();
  hclSizeX->Draw();

  hclSizeY->SetLineColor(kRed);
  hclSizeY->Draw("same");
  
  TLegend *leg = new TLegend(0.89,0.69,0.99,0.75);
  leg->AddEntry(hclSizeX,"X","l");
  leg->AddEntry(hclSizeY,"Y","l");
  leg->Draw();
  cclSize->Print(graphClSize.c_str(), "png");


  TCanvas *cstrips = new TCanvas("cstrips", "cstrips", 1000,700);
  hcentroidX->Draw();
  hcentroidY->SetLineColor(kRed);
  hcentroidY->Draw("same");
  TLegend *legS = new TLegend(0.89,0.69,0.99,0.75);
  legS->AddEntry(hcentroidX,"X","l");
  legS->AddEntry(hcentroidY,"Y","l");
  legS->Draw();
  cstrips->Print(graphStrip.c_str(), "png");


  TCanvas *campCenter = new TCanvas("camp", "camp", 1600,1000);
  campCenter->Divide(2,1);
  campCenter->cd(1);
  hampCenterY->SetLineColor(kRed);
  hampCenterY->Draw();
  hampCenterX->Draw("same");

  campCenter->cd(2);
  hampSampleY->SetLineColor(kRed);
  hampSampleY->Draw();
  hampSampleX->Draw("same");

  campCenter->cd(0);
  TLegend *legA = new TLegend(0.89,0.69,0.99,0.75);
  legA->AddEntry(hampCenterX,"X","l");
  legA->AddEntry(hampCenterY,"Y","l");
  // legA->SetTextSize(0.02);
  legA->Draw();
  campCenter->Print(graphampCenter.c_str(), "png");

  // gStyle->SetOptStat(0);
  TCanvas *c3 = new TCanvas("c3", "c3", 1000,1000);
  h2c->SetStats(0);
  h2c->Draw("colz");
  gPad->SetLogz();
  c3->Print(graphMap.c_str(), "png");

  TCanvas *campX = new TCanvas("campX", "campX", 1600,1000);
  campX->Divide(3, 2);
  for(int i=0; i<6; i++){
    campX->cd(i+1);
    if(i==0){
      hampCenterX->Draw();
    }else{
     h2ampX[i]->SetStats(0);
     h2ampX[i]->Draw("colz");
     gPad->SetLogz();
    }
  }
  campX->Print(graphAmpX.c_str(), "png");

  TCanvas *campY = new TCanvas("campY", "campY", 1600,1000);
  campY->Divide(3, 2);
  for(int i=0; i<6; i++){
    campY->cd(i+1);
    if(i==0){
      hampCenterY->Draw();
    }else{
     h2ampY[i]->SetStats(0);
     h2ampY[i]->Draw("colz");
     gPad->SetLogz();
    }
  }
  campY->Print(graphAmpY.c_str(), "png");
}