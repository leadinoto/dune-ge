#ifndef test
#define test

TLorentzVector GlobalCoordinates(TLorentzVector pos){
	TLorentzVector finalpos;
	finalpos.SetXYZT(pos.X()*10,
			pos.Y()*10 -2384.73,
			pos.Z()*10 +23910,
			pos.T());
	return finalpos;
}

TLorentzVector LocalCoordinates(TLorentzVector pos){
	TLorentzVector finalpos;
	finalpos.SetXYZT(pos.X(),
			pos.Y() +2384.73,
			pos.Z() -23910,
			pos.T());
	return finalpos;
}



#endif
