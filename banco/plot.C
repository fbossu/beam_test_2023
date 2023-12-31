
void plot() {
  gErrorIgnoreLevel = kWarning;
  gStyle->SetOptFit(1000001);
  std::vector<std::string> names = {
    "ladder162",
    "ladder157",
    "ladder163",
    "ladder160"
  };

  TFile *fin = TFile::Open("fout.root");

  //const char *base = "histos/hres";
  const char *base = "histos/hUres";

  for( auto s : names ){
    TCanvas *c = new TCanvas(s.c_str(), s.c_str());
    c->Divide(2,1);
    c->cd(1);
    TH1F* hx = fin->Get<TH1F>(Form("%sx_%s",base,s.c_str()));
    hx->Fit("gaus","Q0S+");
    hx->DrawClone();
    hx->GetFunction("gaus")->SetNpx(500);
    hx->GetFunction("gaus")->DrawClone("same");

    c->cd(2);
    TH1F* hy = fin->Get<TH1F>(Form("%sy_%s",base,s.c_str()));
    hy->Fit("gaus","Q0S+");
    hy->DrawClone();
    hy->GetFunction("gaus")->SetNpx(500);
    hy->GetFunction("gaus")->DrawClone("same");
    //cout << s << " x: " << setw(10) << hx->GetMean();
    //cout <<     "  y: " << setw(10) << hy->GetMean();
    cout << setprecision(3);
    cout << s << " x: " 
              << setw(11) << hx->GetFunction("gaus")->GetParameter("Mean") << " +- "
              << setw(8) << hx->GetFunction("gaus")->GetParameter("Sigma");
    cout <<     " ;  y: " 
              << setw(11) << hy->GetFunction("gaus")->GetParameter("Mean") << " +- "
              << setw(8) << hy->GetFunction("gaus")->GetParameter("Sigma");
    cout << endl;
    c->SaveAs( (s + ".pdf").c_str() );
  }

  // draw slopes
  TCanvas *cm = new TCanvas("cm","slopes");
  TLegend *l = new TLegend(.1,0.6,.3,.9);
  auto nt = fin->Get<TTree>("events");
  TH1F *hmx = new TH1F("hmx", "slope", 300, -0.05, 0.05 );
  hmx->SetLineColor(kBlack);
  nt->Draw("tracks.mx>>hmx");
  l->AddEntry(hmx,"l","x");
  TH1F *hmy = new TH1F("hmy", "slope", 300, -0.05, 0.05 );
  hmy->SetLineColor(kRed);
  nt->Draw("tracks.my>>hmy","","same");
  l->AddEntry(hmy,"l","y");

  cout << "mean"  << setw(10) << hmx->GetMean() << setw(10) << hmy->GetMean() << endl;
  cout << "angle" << setw(10) << 
          atan( hmx->GetMean() ) << setw(10) << 
          atan( hmy->GetMean() )<< endl;

}

