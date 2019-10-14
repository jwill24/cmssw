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
#include "FWCore/Utilities/interface/transform.h"

#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"

#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "DataFormats/CTPPSDetId/interface/CTPPSDetId.h"
#include "DataFormats/CTPPSReco/interface/CTPPSLocalTrackLite.h"
#include "DataFormats/ProtonReco/interface/ForwardProton.h"
#include "DataFormats/ProtonReco/interface/ForwardProtonFwd.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"

class ProtonProducer : public edm::global::EDProducer<> {
public:
  ProtonProducer( edm::ParameterSet const & ps) :
    precision_( ps.getParameter<int>("precision") ),
    protonLabel_(ps.getParameter<std::vector<edm::InputTag>>("tagRecoProtons")),
    protonTag_(edm::vector_transform(protonLabel_,
				     [this](const edm::InputTag& tag) { return mayConsume<reco::ForwardProtonCollection>(tag); }))
  {
    produces<edm::ValueMap<int>>("protonRPId");
    produces<edm::ValueMap<int>>("protonRPType");
    produces<edm::ValueMap<bool>>("sector45Single");
    produces<edm::ValueMap<bool>>("sector56Single");
    produces<edm::ValueMap<bool>>("sector45Multi");
    produces<edm::ValueMap<bool>>("sector56Multi");
    produces<nanoaod::FlatTable>("ppsTrackTable");
  }
  ~ProtonProducer() override {}
  
  // ------------ method called to produce the data  ------------
  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {

    std::vector<int> protonRPId, protonRPType;
    std::vector<bool> sector45Single, sector56Single, sector45Multi, sector56Multi;
    std::vector<bool> trackMethod;
    std::vector<float> trackX, trackXUnc, trackY, trackYUnc, trackTime, trackTimeUnc;
    std::vector<int> trackIdx, numPlanes, pixelRecoInfo;
    int reco_type = 0;

    // Get Forward Proton handle
    edm::Handle<reco::ForwardProtonCollection> hRecoProtons;
    for (const auto& protonTag : protonTag_) {
      if (!hRecoProtons.isValid()) {
	break;
      }
      iEvent.getByToken(protonTag,hRecoProtons);

      int proton_pos = 0;

      for (const auto &proton : *hRecoProtons) {
	if ( proton.method() == reco::ForwardProton::ReconstructionMethod::singleRP ) {
	  CTPPSDetId rpId((*proton.contributingLocalTracks().begin())->getRPId());
	  protonRPId.push_back( rpId.arm() * 100 + rpId.station() * 10 + rpId.rp() );
	  protonRPType.push_back( rpId.subdetId() );
	}
	
	if (proton.pz() < 0. ) {
	  if ( proton.method() == reco::ForwardProton::ReconstructionMethod::singleRP ) {
	    sector56Single.push_back( true );
	    sector45Single.push_back( false );
	  } else {
	    sector56Multi.push_back( true );
	    sector45Multi.push_back( false );
	  }
	}
	else if (proton.pz() > 0. ) {
	  if( proton.method() == reco::ForwardProton::ReconstructionMethod::singleRP ) {
	    sector45Single.push_back( true );
	    sector56Single.push_back( false );
	  } else {
	    sector45Multi.push_back( true );
            sector56Multi.push_back( false );
	  }
	}
	else {
	  if ( proton.method() == reco::ForwardProton::ReconstructionMethod::singleRP ) {
            sector45Single.push_back( false );
            sector56Single.push_back( false );
          } else {
            sector45Multi.push_back( false );
            sector56Multi.push_back( false );
          }
	}

	for (const auto& tr : proton.contributingLocalTracks()) {
	  ( proton.method() == reco::ForwardProton::ReconstructionMethod::singleRP ) ? trackMethod.push_back( true ) : trackMethod.push_back( false );
	  trackX.push_back( tr->getX() );
	  trackXUnc.push_back( tr->getXUnc() );
	  trackY.push_back( tr->getY() );
	  trackYUnc.push_back( tr->getYUnc() );
	  trackTime.push_back( tr->getTime() );
	  trackTimeUnc.push_back( tr->getTimeUnc() );
	  trackIdx.push_back( proton_pos );
	  numPlanes.push_back( tr->getNumberOfPointsUsedForFit() );
	  pixelRecoInfo.push_back( static_cast<int>(tr->getPixelTrackRecoInfo()) );
	}
	proton_pos++;
      }

      if (reco_type == 0) {
	std::unique_ptr<edm::ValueMap<int>> protonRPIdV(new edm::ValueMap<int>());
	edm::ValueMap<int>::Filler fillerID(*protonRPIdV);
	fillerID.insert(hRecoProtons, protonRPId.begin(), protonRPId.end());
	fillerID.fill();
	
	std::unique_ptr<edm::ValueMap<int>> protonRPTypeV(new edm::ValueMap<int>());
	edm::ValueMap<int>::Filler fillerSubID(*protonRPTypeV);
	fillerSubID.insert(hRecoProtons, protonRPType.begin(), protonRPType.end());
	fillerSubID.fill();

	std::unique_ptr<edm::ValueMap<bool>> sector45SingleV(new edm::ValueMap<bool>());
	edm::ValueMap<bool>::Filler filler45Single(*sector45SingleV);
	filler45Single.insert(hRecoProtons, sector45Single.begin(), sector45Single.end());
	filler45Single.fill();
	
	std::unique_ptr<edm::ValueMap<bool>> sector56SingleV(new edm::ValueMap<bool>());
	edm::ValueMap<bool>::Filler filler56Single(*sector56SingleV);
	filler56Single.insert(hRecoProtons, sector56Single.begin(), sector56Single.end());
	filler56Single.fill();

	iEvent.put(std::move(protonRPIdV), "protonRPId");
	iEvent.put(std::move(protonRPTypeV), "protonRPType");
	iEvent.put(std::move(sector45SingleV), "sector45Single");
	iEvent.put(std::move(sector56SingleV), "sector56Single");
      } else if (reco_type == 1) {
	std::unique_ptr<edm::ValueMap<bool>> sector45MultiV(new edm::ValueMap<bool>());
	edm::ValueMap<bool>::Filler filler45Multi(*sector45MultiV);
	filler45Multi.insert(hRecoProtons, sector45Multi.begin(), sector45Multi.end());
	filler45Multi.fill();
	
	std::unique_ptr<edm::ValueMap<bool>> sector56MultiV(new edm::ValueMap<bool>());
	edm::ValueMap<bool>::Filler filler56Multi(*sector56MultiV);
	filler56Multi.insert(hRecoProtons, sector56Multi.begin(), sector56Multi.end());
	filler56Multi.fill();
	
	iEvent.put(std::move(sector45MultiV), "sector45Multi");
	iEvent.put(std::move(sector56MultiV), "sector56Multi");
      }
      reco_type++;
    } // Loop over proton types

      auto ppsTab = std::make_unique<nanoaod::FlatTable>(trackX.size(),"PPSLocalTrack",false);
      ppsTab->addColumn<float>("x",trackX,"local track x",nanoaod::FlatTable::FloatColumn,precision_);
      ppsTab->addColumn<float>("xUnc",trackXUnc,"local track x uncertainty",nanoaod::FlatTable::FloatColumn,precision_);
      ppsTab->addColumn<float>("y",trackY,"local track y",nanoaod::FlatTable::FloatColumn,precision_);
      ppsTab->addColumn<float>("yUnc",trackYUnc,"local track y uncertainty",nanoaod::FlatTable::FloatColumn,precision_);
      ppsTab->addColumn<float>("time",trackTime,"local track time",nanoaod::FlatTable::FloatColumn,precision_);
      ppsTab->addColumn<float>("timeUnc",trackTimeUnc,"local track time uncertainty",nanoaod::FlatTable::FloatColumn,precision_);
      ppsTab->addColumn<uint8_t>("isMultiRP",trackMethod,"singleRP or multiRP track",nanoaod::FlatTable::BoolColumn);
      ppsTab->addColumn<int>("protonIdx",trackIdx,"local track - proton correspondence",nanoaod::FlatTable::IntColumn);
      ppsTab->addColumn<int>("numPlanes",numPlanes,"number of points used for fit",nanoaod::FlatTable::IntColumn);
      ppsTab->addColumn<int>("pixelRecoInfo",pixelRecoInfo,"flag if a ROC was shifted by a bunchx",nanoaod::FlatTable::IntColumn);
      ppsTab->setDoc("ppsLocalTrack variables");

      iEvent.put(std::move(ppsTab), "ppsTrackTable");
  }
  
  // ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
  static void fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
  }
  
protected:
  const unsigned int precision_;
  const std::vector<edm::InputTag> protonLabel_;
  const std::vector<edm::EDGetTokenT<reco::ForwardProtonCollection> > protonTag_;
};

DEFINE_FWK_MODULE(ProtonProducer);
