// -*- C++ -*-
//
// Package:    PhysicsTools/NanoAOD
// Class:      ProtonProducer
//
/**\class ProtonProducer ProtonProducer.cc PhysicsTools/NanoAOD/plugins/ProtonProducer.cc
 Description: Realavent proton variables for analysis usage
 Implementation:
*/
//
// Original Author:  Justin Williams
//         Created: 04 Jul 2019 15:27:53 GMT
//
//

// system include files
#include <memory>
#include <map>
#include <string>
#include <vector>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"

#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Provenance/interface/Provenance.h"

#include "DataFormats/CTPPSDetId/interface/CTPPSDetId.h"
#include "DataFormats/CTPPSReco/interface/CTPPSLocalTrackLite.h"
#include "DataFormats/ProtonReco/interface/ForwardProton.h"
#include "DataFormats/ProtonReco/interface/ForwardProtonFwd.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"

class ProtonProducer : public edm::global::EDProducer<> {
public:
  ProtonProducer( edm::ParameterSet const & ps) :
    tokenRecoProtonsSingleRP_(mayConsume<reco::ForwardProtonCollection>(ps.getParameter<edm::InputTag>("tagRecoProtonsSingle"))),
    tokenRecoProtonsMultiRP_(mayConsume<reco::ForwardProtonCollection>(ps.getParameter<edm::InputTag>("tagRecoProtonsMulti"))),
    precision_( ps.getParameter<int>("precision") )
  {
    produces<edm::ValueMap<int>>("protonRPId");
    produces<edm::ValueMap<int>>("protonRPType");
    produces<edm::ValueMap<bool>>("singleRPsector45");
    produces<edm::ValueMap<bool>>("multiRPsector45");
    produces<edm::ValueMap<bool>>("singleRPsector56");
    produces<edm::ValueMap<bool>>("multiRPsector56");
    produces<nanoaod::FlatTable>("ppsTrackTable");
  }
  ~ProtonProducer() override {}
  
  // ------------ method called to produce the data  ------------
  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {

    // Get Forward Proton handles
    edm::Handle<reco::ForwardProtonCollection> hRecoProtonsSingleRP;
    iEvent.getByToken(tokenRecoProtonsSingleRP_, hRecoProtonsSingleRP);

    edm::Handle<reco::ForwardProtonCollection> hRecoProtonsMultiRP;
    iEvent.getByToken(tokenRecoProtonsMultiRP_, hRecoProtonsMultiRP);

    // FIXME: might need to move these inside the loop over handles
    std::vector<int> protonRPId, protonRPType;
    std::vector<bool> singleRP_sector45, singleRP_sector56, multiRP_sector45, multiRP_sector56;
    std::vector<float> trackX, trackXUnc, trackY, trackYUnc, trackTime, trackTimeUnc;
    std::vector<int> singleRP_trackIdx, multiRP_trackIdx, trackDetId, numPlanes, pixelRecoInfo;

    for (const auto &handle : {hRecoProtonsSingleRP, hRecoProtonsMultiRP}) {

      //const edm::Provenance& provenance = *handle.provenance();
      //const std::string& productInstanceName = provenance.productInstanceName();
      //std::cout << "" << std::endl;
      //std::cout << "Looping over " << productInstanceName << " protons" << std::endl;

      int num_proton = handle->size();
      //std::cout << "Num protons: " << num_proton << std::endl;
      int proton_pos = 0;
      protonRPId.reserve( num_proton );
      singleRP_sector45.reserve( num_proton );
      singleRP_sector56.reserve( num_proton );
      multiRP_sector45.reserve( num_proton );
      multiRP_sector56.reserve( num_proton );
      
      for (const auto &proton : *handle) {
	int method = (proton.method() == reco::ForwardProton::ReconstructionMethod::singleRP) ? 0 : 1;

	if (method == 0) { // singleRP protons
	  CTPPSDetId rpId((*proton.contributingLocalTracks().begin())->getRPId());
	  protonRPId.push_back( rpId.arm() * 100 + rpId.station() * 10 + rpId.rp() );
	  protonRPType.push_back( rpId.subdetId() );
	  // FIXME: this could probably be cleaner
	  if (proton.pz() < 0. ) {
	    singleRP_sector56.push_back( true );
	    singleRP_sector45.push_back( false );
	  }
	  else if (proton.pz() > 0. ) {
	    singleRP_sector45.push_back( true );
	    singleRP_sector56.push_back( false );
	  }
	  else {
	    singleRP_sector45.push_back( false );
	    singleRP_sector56.push_back( false );
	  }
	} 
	else if (method == 1) { //multiRP protons
	  if (proton.pz() < 0. ) {
	    multiRP_sector56.push_back( true );
	    multiRP_sector45.push_back( false );
	  }
	  else if (proton.pz() > 0. ) {
	    multiRP_sector45.push_back( true );
	    multiRP_sector56.push_back( false );
	  }
	  else {
	    multiRP_sector45.push_back( false );
	    multiRP_sector56.push_back( false );
	  }
	}



	for (const auto& tr : proton.contributingLocalTracks()) {
	  CTPPSDetId rpId(tr->getRPId());
	  singleRP_trackIdx.push_back( (method == 0) ? proton_pos : -1 );
	  multiRP_trackIdx.push_back( (method == 1) ? proton_pos : -1 );
	  trackX.push_back( tr->getX() );
	  trackXUnc.push_back( tr->getXUnc() );
	  trackY.push_back( tr->getY() );
	  trackYUnc.push_back( tr->getYUnc() );
	  trackTime.push_back( tr->getTime() );
	  trackTimeUnc.push_back( tr->getTimeUnc() );
	  trackDetId.push_back(rpId.arm() * 100 + rpId.station() * 10 + rpId.rp());
	  numPlanes.push_back( tr->getNumberOfPointsUsedForFit() );
	  pixelRecoInfo.push_back( static_cast<int>(tr->getPixelTrackRecoInfo()) );
	} // End loop over tracks
	proton_pos++;
      } // End loop over protons
    } // End loop over handles





    // Check for duplicate tracks before filling
    if (trackX.size() > 1) {
      std::vector<float> tmpX = trackX;
      std::vector<float> duplicate_x;    
      std::sort(tmpX.begin(), tmpX.end());
      
      for (auto x = trackX.begin(); x != trackX.end(); ++x) std::cout << "trackX: " << *x << std::endl;
      
      for (unsigned int i = 0; i < tmpX.size()-1; i++) {
	if (tmpX[i] == tmpX[i+1]) {
	  duplicate_x.push_back(tmpX[i]); // Duplicate track
	}
      }

      
      // Get positions of duplicates
      for (auto dup_val = duplicate_x.begin(); dup_val != duplicate_x.end(); ++dup_val) {
	std::cout << "Duplicate x: " << *dup_val << std::endl;	
	
	auto it_single = std::find(trackX.begin(), trackX.end(), *dup_val);
	auto it_multi = std::find(it_single+1, trackX.end(), *dup_val); 
	int singleRP_index = std::distance(trackX.begin(), it_single);
	int multiRP_index  = std::distance(trackX.begin(), it_multi);
	std::cout << "singleRP_index: " << singleRP_index << " multiRP_index: " << multiRP_index  << std::endl;

	std::cout << "Changing the Idx of the proton" << std::endl;
	
	// Change the Idx of the proton to keep
	std::cout << "Current singleRP_trackIdx: " << std::endl;
	for (auto id = singleRP_trackIdx.begin(); id != singleRP_trackIdx.end(); ++id) std::cout <<  *id << std::endl;
	std::cout << "Current multiRP_trackIdx: " << std::endl;
	for (auto id = multiRP_trackIdx.begin(); id != multiRP_trackIdx.end(); ++id) std::cout <<  *id << std::endl;

	singleRP_trackIdx.erase(singleRP_trackIdx.begin()+multiRP_index); // FIXME: I don't think this works in general
	multiRP_trackIdx.erase(multiRP_trackIdx.begin()+singleRP_index); // FIXME: I don't think this works in general

	std::cout << "New singleRP_trackIdx: " << std::endl;;
	for (auto id = singleRP_trackIdx.begin(); id != singleRP_trackIdx.end(); ++id) std::cout <<  *id << std::endl;;
	std::cout << "New multiRP_trackIdx: " << std::endl;;
	for (auto id = multiRP_trackIdx.begin(); id != multiRP_trackIdx.end(); ++id) std::cout <<  *id << std::endl;;

	std::cout << "Removing duplicate track info" << std::endl;

	// Remove the chosen track info
	for (auto x = trackX.begin(); x != trackX.end(); ++x) std::cout << "Current trackX: " << *x << std::endl;
	trackX.erase(trackX.begin()+multiRP_index);
	trackXUnc.erase(trackXUnc.begin()+multiRP_index);
	trackY.erase(trackY.begin()+multiRP_index);
	trackYUnc.erase(trackYUnc.begin()+multiRP_index);
	trackTime.erase(trackTime.begin()+multiRP_index);
	trackTimeUnc.erase(trackTimeUnc.begin()+multiRP_index);
	trackDetId.erase(trackDetId.begin()+multiRP_index);
	numPlanes.erase(numPlanes.begin()+multiRP_index);
	pixelRecoInfo.erase(pixelRecoInfo.begin()+multiRP_index);
	for (auto x = trackX.begin(); x != trackX.end(); ++x) std::cout << "New trackX: " << *x << std::endl;
      } // End loop over duplicates
    }


    




    auto ppsTab = std::make_unique<nanoaod::FlatTable>(trackX.size(),"PPSLocalTrack",false);
    ppsTab->addColumn<int>("singleRP_trackIdx",singleRP_trackIdx,"local track - proton correspondence",nanoaod::FlatTable::IntColumn);
    ppsTab->addColumn<int>("multiRP_trackIdx",multiRP_trackIdx,"local track - proton correspondence",nanoaod::FlatTable::IntColumn);
    ppsTab->addColumn<float>("x",trackX,"local track x",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("xUnc",trackXUnc,"local track x uncertainty",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("y",trackY,"local track y",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("yUnc",trackYUnc,"local track y uncertainty",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("time",trackTime,"local track time",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("timeUnc",trackTimeUnc,"local track time uncertainty",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("trackDetId",trackDetId,"local track detector id",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<int>("numPlanes",numPlanes,"number of points used for fit",nanoaod::FlatTable::IntColumn);
    ppsTab->addColumn<int>("pixelRecoInfo",pixelRecoInfo,"flag if a ROC was shifted by a bunchx",nanoaod::FlatTable::IntColumn);
    ppsTab->setDoc("ppsLocalTrack variables");
    
    std::unique_ptr<edm::ValueMap<int>> protonRPIdV(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler fillerID(*protonRPIdV);
    fillerID.insert(hRecoProtonsSingleRP, protonRPId.begin(), protonRPId.end());
    fillerID.fill();

    std::unique_ptr<edm::ValueMap<int>> protonRPTypeV(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler fillerSubID(*protonRPTypeV);
    fillerSubID.insert(hRecoProtonsSingleRP, protonRPType.begin(), protonRPType.end());
    fillerSubID.fill();

    std::unique_ptr<edm::ValueMap<bool>> singleRP_sector45V(new edm::ValueMap<bool>());
    edm::ValueMap<bool>::Filler fillersingle45(*singleRP_sector45V);
    fillersingle45.insert(hRecoProtonsSingleRP, singleRP_sector45.begin(), singleRP_sector45.end());
    fillersingle45.fill();
    
    std::unique_ptr<edm::ValueMap<bool>> singleRP_sector56V(new edm::ValueMap<bool>());
    edm::ValueMap<bool>::Filler fillersingle56(*singleRP_sector56V);
    fillersingle56.insert(hRecoProtonsSingleRP, singleRP_sector56.begin(), singleRP_sector56.end());
    fillersingle56.fill();

    std::unique_ptr<edm::ValueMap<bool>> multiRP_sector45V(new edm::ValueMap<bool>());
    edm::ValueMap<bool>::Filler fillermulti45(*multiRP_sector45V);
    fillermulti45.insert(hRecoProtonsMultiRP, multiRP_sector45.begin(), multiRP_sector45.end());
    fillermulti45.fill();
    
    std::unique_ptr<edm::ValueMap<bool>> multiRP_sector56V(new edm::ValueMap<bool>());
    edm::ValueMap<bool>::Filler fillermulti56(*multiRP_sector56V);
    fillermulti56.insert(hRecoProtonsMultiRP, multiRP_sector56.begin(), multiRP_sector56.end());
    fillermulti56.fill();

    iEvent.put(std::move(protonRPIdV), "protonRPId");
    iEvent.put(std::move(protonRPTypeV), "protonRPType");
    iEvent.put(std::move(singleRP_sector45V), "singleRPsector45");
    iEvent.put(std::move(singleRP_sector56V), "singleRPsector56");
    iEvent.put(std::move(multiRP_sector45V), "multiRPsector45");
    iEvent.put(std::move(multiRP_sector56V), "multiRPsector56");
    iEvent.put(std::move(ppsTab), "ppsTrackTable");


  } // End loop over events  

  

  // ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
  static void fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
  }
  
protected:
  const edm::EDGetTokenT<reco::ForwardProtonCollection> tokenRecoProtonsSingleRP_;
  const edm::EDGetTokenT<reco::ForwardProtonCollection> tokenRecoProtonsMultiRP_;
  const unsigned int precision_;
};


DEFINE_FWK_MODULE(ProtonProducer);
