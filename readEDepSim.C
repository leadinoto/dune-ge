// TEST MACRO FOR USE WITH OLDER ROOT6.  DOESN"T WORK WHEN CLING KNOWS ABOUT
// THE VARIOUS CLASSES.
#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>

#include <iostream>
#include <string>
#include "TVector3.h"
#include "readEDepSim.h"
#include "src/utils.h"


TTree* gEDepSimTree = NULL;
TG4Event* gEDepSimEvent = NULL;
bool traj=false;      //put true to read trajectories
bool segm=true;      //put true to read segment	
TH2F* PosXY;
TH2F* PosYZ;
TH1F* PosX;

void readEDepSim() {
	std::cout<<"STARTING READ execution "<<std::endl;

	PosXY = new TH2F("PosXY","PosXY",1000,-3000,3000,1000,-6000,1000); //-5000.0,0.0,1000,21500.0,26500.0);
	PosYZ = new TH2F("PosYZ","PosYZ",1000,20000,28000,1000,-6000,1000); //-5000.0,0.0,1000,21500.0,26500.0);
		
	TFile *ffile=new TFile("/eos/user/l/ldinoto/SAND_SPILL_0/sand-spill-events.0.edep.root", "READ"); 
	gEDepSimTree = (TTree*) ffile->Get("EDepSimEvents");
		
	if (!gEDepSimTree) {
		std::cout << "Missing the event tree" << std::endl;
		return;
	}



	gEDepSimTree->SetBranchAddress("Event",&gEDepSimEvent);

	int N=gEDepSimTree->GetEntries();
	std::cout<<"number of entries "<<N<<std::endl;
	for (int i=0; i<N; i++){

		gEDepSimTree->GetEntry(i);
		EDepSimDumpEvent();
	}

	TFile *outf=new TFile ("Plots.root", "RECREATE");	
	PosYZ->Write();
	PosXY->Write();
}
TTree* EDepSimTree() {
	return gEDepSimTree;
}

TG4Event* EDepSimEvent() {
	return gEDepSimEvent;
}


void EDepSimDumpEvent() {
	TG4Event* event = EDepSimEvent();
	if (!event) {
		std::cout << "Event not available" << std::endl;
	}

	const char* prova="ppp";
	std::cout<<prova<<std::endl;
	std::cout << " " << event->EventId;
	std::cout << " >event " << event->EventId;
	std::cout << " primaries " << event->Primaries.size();
	std::cout << " trajectories " << event->Trajectories.size();
	std::cout << " segment-detectors " << event->SegmentDetectors.size();
	std::cout << std::endl;
	std::cout << "Analyzing PRIMARIES "<<std::endl;

	for (std::vector<TG4PrimaryVertex>::iterator
			v = event->Primaries.begin();
			v != event->Primaries.end(); ++v) {
		std::cout << "   Position " << v->Position.X()
			<< " " << v->Position.Y()
			<< " " << v->Position.Z()
			<< " Time " << v->Position.T()
		        << " ReacType "<<v->Reaction;
		std::cout <<" Nbr of particles " << v->Particles.size()<<std::endl;
		std::cout<<"Position in local coord. "<<LocalCoordinates(v->Position).Y()<<std::endl;

		/*
		for (std::vector<TG4PrimaryParticle>::iterator
				p = v->Particles.begin();
				p != v->Particles.end(); ++p) {
			std::cout << " TrackId " << p->TrackId;
			std::cout << " PDGcode " << p->PDGCode;
			std::cout << " Name " << p->Name<<std::endl;
		}
		std::cout << endl;
	*/
	}

	if(traj==true){
		std::cout << "Analyzing TRAJECTORY "<<std::endl;
		for (std::vector<TG4Trajectory>::iterator
				t = event->Trajectories.begin();
				t != event->Trajectories.end(); ++t) {
			std::cout << " TrackId " << t->TrackId;
			std::cout << " ParentId " << t->ParentId;
			std::cout << " Name " << t->Name;
			int count = t->Points.size();
			std::cout << " Up to " << count << " points";
			std::cout << std::endl;
			
			std::cout << " TRAJ POINTS:    ";
			   for (std::vector<TG4TrajectoryPoint>::iterator
			   p = t->Points.begin();
			   p != t->Points.end();
			   ++p) {
			   std::cout << " Time: " << p->Position.T();
			   std::cout << " Process: " << p->Process;
			//PosX->Fill(p->Position.X());
			std::cout << " Subprocess: " << p->Subprocess;
			std::cout << std::endl;
			if (--count < 1) break;
			}
		}
	}
	if(segm==true){	
		std::cout << "Analyzing SEGMENT DETECTORS "<<std::endl;
		for (auto d = event->SegmentDetectors.begin();
				d != event->SegmentDetectors.end(); ++d) {
			std::cout << "   det " << d->first;
			int count = d->second.size();
			std::cout << " up to " << count << " segments";
			std::cout << std::endl;

			if(d->first=="LArHit" || d->first=="Straw"){  //Straw //EMCalSci //MuID??
				int oldcontrib=0;
				for (std::vector<TG4HitSegment>::iterator
						h = d->second.begin();
						h != d->second.end();
						++h) {
					//			std::cout << "      ";
								std::cout << " Coordinates: " << h->Start.X()<<" "<<h->Start.Y()<<" "<<h->Start.Z()<<std::endl;
					PosXY->Fill(h->Start.X(), h->Start.Y());
					PosYZ->Fill(h->Start.Y(), h->Start.Z());
					int contrib=h->Contrib[0];
					//if(contrib!= oldcontrib){
					//	std::cout << " PrimaryId: " << h->PrimaryId;
					//			std::cout << " EnergyDeposit: " << h->EnergyDeposit;
					//std::cout << " S: " << h->SecondaryDeposit;
					//	std::cout << " Contrib: " << h->Contrib.size()<<" ["<<h->Contrib[0]<<"] ";
					//			std::cout << " Track Length: " << h->TrackLength;
					//			std::cout << std::endl;
					//	}
					oldcontrib=contrib;
					if ((--count) < 1) break;
				}
			}
		}
	}
}

TG4Event* EDepSimEvent(int i) {
	if (!gEDepSimTree) {
		std::cout << "Missing the event tree" << std::endl;
		return NULL;
	}
	gEDepSimTree->GetEntry(i);
	std::cout << "entry " << i;
	std::cout << " event " << gEDepSimEvent->EventId;
	std::cout << std::endl;
	return gEDepSimEvent;
}    
