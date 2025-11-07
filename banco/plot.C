
void plot() {
  gErrorIgnoreLevel = kWarning;
  gStyle->SetOptFit(1000001);
  std::vector<std::string> names = {
    "ladder160",
    "ladder163",
    "ladder157",
    "ladder162"
  };

  TFile *fin = TFile::Open("fout.root");

  //const char *base = "histos/hres";
  const char *base = "histos/hUres";

  TCanvas *cr = new TCanvas("cres","residues", 900,900);
  cr->Divide(2,2);
  int ic=0;
  for( auto s : names ){
    ic++;
    auto *c = cr->cd(ic);
    c->SetTitle(s.c_str());
    c->Divide(2,1);
    c->cd(1);
    TH1F* hx = fin->Get<TH1F>(Form("%sx_%s",base,s.c_str()));
    if( hx->GetEntries() < 1 ) continue;
    hx->Fit("gaus","Q0S+");
    hx->DrawClone();
    TF1 *f = hx->GetFunction("gaus");
    if( ! f ) continue;
    f->SetNpx(500);
    f->DrawClone("same");

    c->cd(2);
    TH1F* hy = fin->Get<TH1F>(Form("%sy_%s",base,s.c_str()));
    hy->Fit("gaus","Q0S+");
    hy->DrawClone();
    f = hy->GetFunction("gaus");
    if( ! f ) continue;
    f->SetNpx(500);
    f->DrawClone("same");
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
  l->Draw();

  cout << "mean"  << setw(10) << hmx->GetMean() << setw(10) << hmy->GetMean() << endl;
  cout << "angle" << setw(10) << 
          atan( hmx->GetMean() ) << setw(10) << 
          atan( hmy->GetMean() )<< endl;


 // draw occupancies
  TCanvas *co = new TCanvas("banco occupancies","banco occupancies", 1000,600);
  co->Divide(1,4);
  co->cd(1)->SetLogz();
  fin->Get<TH2F>("histos/h2xy_ladder162")->Draw("colz");
  co->cd(2)->SetLogz();
  fin->Get<TH2F>("histos/h2xy_ladder157")->Draw("colz");
  co->cd(3)->SetLogz();
  fin->Get<TH2F>("histos/h2xy_ladder163")->Draw("colz");
  co->cd(4)->SetLogz();
  fin->Get<TH2F>("histos/h2xy_ladder160")->Draw("colz");

 // draw correlations x
  TCanvas *cox = new TCanvas("banco corr x","banco corr X", 1000,800);
  cox->SetLogz();
  cox->Divide(2,3);
  cox->cd(1)->SetLogz();
  fin->Get<TH2F>("histos/hladder157ladder162x")->Draw("colz");
  cox->cd(2)->SetLogz();
  fin->Get<TH2F>("histos/hladder157ladder163x")->Draw("colz");
  cox->cd(3);
  fin->Get<TH2F>("histos/hladder157ladder160x")->Draw("colz");
  cox->cd(4)->SetLogz();
  fin->Get<TH2F>("histos/hladder162ladder163x")->Draw("colz");
  cox->cd(5)->SetLogz();
  fin->Get<TH2F>("histos/hladder160ladder162x")->Draw("colz");
  cox->cd(6)->SetLogz();
  fin->Get<TH2F>("histos/hladder160ladder163x")->Draw("colz");
 // draw correlations y
  TCanvas *coy = new TCanvas("banco corr y","banco corr Y", 1000,800);
  cox->SetLogz();
  coy->Divide(2,3);
  coy->cd(1)->SetLogz();
  fin->Get<TH2F>("histos/hladder157ladder162y")->Draw("colz");
  coy->cd(2)->SetLogz();
  fin->Get<TH2F>("histos/hladder157ladder163y")->Draw("colz");
  coy->cd(3)->SetLogz();
  fin->Get<TH2F>("histos/hladder157ladder160y")->Draw("colz");
  coy->cd(4)->SetLogz();
  fin->Get<TH2F>("histos/hladder162ladder163y")->Draw("colz");
  coy->cd(5)->SetLogz();
  fin->Get<TH2F>("histos/hladder160ladder162y")->Draw("colz");
  coy->cd(6)->SetLogz();
  fin->Get<TH2F>("histos/hladder160ladder163y")->Draw("colz");
}

