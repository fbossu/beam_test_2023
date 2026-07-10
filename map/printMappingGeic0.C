void printMapping(){

  // parameters
  float ZActiveX = 340.; float ZActiveY = 590.;  // size of active area
  int nbStrX = 192; int nbStrY = 576; // nb de voies
  float pitchX = 1.766; float pitchY = 1.021;  // pitch en X et Y
  float sizeStrX = 0.7; float sizeStrY = 0.9;  // taille de l'aperture des pistes, carré size X size
  float gapEndStrip = 1; // gap between end of strips and side of active area


  // parameters of strip area
  double largX = (nbStrX-1)*pitchX;
  double largY = (nbStrY-1)*pitchY;
  double longX = ZActiveY - 2*gapEndStrip - sizeStrX;
  double longY = ZActiveX - 2*gapEndStrip - sizeStrY;


  // center

  float centerX = 180.;
  float centerY = 365.;

  cout << "connector,connectorChannel,stripNb,axis,pitch(mm),interpitch(mm),neighbours(:separated),xGerber,yGerber" << endl;

  int connector=0;
  for ( int i = 0; i < nbStrX ; i++ ){

    if( i < 64 ){
      connector = 1;
    }
    else if ( i < 128 ){
      connector = 5;
    }
    else {
      connector = 10;
    }

    cout <<
      connector << "," <<
      i%64      << "," <<
      i         << "," <<
      "x"       << "," <<
      pitchX           << "," <<
      pitchX-sizeStrX  << "," <<
      i-1 << ":" << i+1 << "," <<
      centerX -longY/2 + i*pitchX << "," << 
      centerY - longX/2 
      << endl;

  }


  connector=0;
  for ( int i = 0; i < nbStrY ; i++ ){

    if( i < 64 ){
      connector = 0;
    }
    else if ( i < 256 ){
      connector = i/64+1;
    }
    else if ( i < 64*8) {
      connector = i/64+2;
    }
    else {
      connector = i/64+3;
    }

    cout <<
      connector << "," <<
      i%64      << "," <<
      i         << "," <<
      "y"       << "," <<
      pitchY           << "," <<
      pitchY-sizeStrY  << "," <<
      i-1 << ":" << i+1 << "," <<
      centerX -longY/2  << "," << 
      centerY - longX/2 + i*pitchY
      << endl;

  }
  //Point firstXup(center + Vector(-largX/2., longX/2.));
  //Point firstXdown(center + Vector(-largX/2., -longX/2.));



}

