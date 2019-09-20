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

#include "DataFormats/CTPPSDetId/interface/CTPPSDetId.h"
#include "DataFormats/CTPPSReco/interface/CTPPSLocalTrackLite.h"
#include "DataFormats/ProtonReco/interface/ForwardProton.h"
#include "DataFormats/ProtonReco/interface/ForwardProtonFwd.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"

class ProtonProducer : public edm::global::EDProducer<> {
public:
  ProtonProducer( edm::ParameterSet const & ps) :
    tokenRecoProtons_(consumes<reco::ForwardProtonCollection>(ps.getParameter<edm::InputTag>("tagRecoProtons"))),
    precision_( ps.getParameter<int>("precision") ),
    method_( ps.getParameter<std::string>("method") )
  {
    produces<edm::ValueMap<int>>("protonRPId");
    produces<edm::ValueMap<int>>("protonRPType");
    produces<edm::ValueMap<bool>>("sector45");
    produces<edm::ValueMap<bool>>("sector56");
    produces<nanoaod::FlatTable>("trackTable");
  }
  ~ProtonProducer() override {}
  
  // ------------ method called to produce the data  ------------
  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {

    // Get Forward Proton handle
    edm::Handle<reco::ForwardProtonCollection> hRecoProtons;
    iEvent.getByToken(tokenRecoProtons_, hRecoProtons);

    std::vector<int> protonRPId, protonRPType;
    std::vector<bool> sector45, sector56;
    int num_proton = hRecoProtons->size();
    int proton_pos = 0;
    std::vector<float> trackX, trackXUnc, trackY, trackYUnc, trackTime, trackTimeUnc;
    std::vector<int> trackIdx, numPlanes, pixelRecoInfo;

    protonRPId.reserve( num_proton );
    sector45.reserve( num_proton );
    sector56.reserve( num_proton );

    for (const auto &proton : *hRecoProtons) {
      CTPPSDetId rpId((*proton.contributingLocalTracks().begin())->getRPId());
      protonRPId.push_back( rpId.arm() * 100 + rpId.station() * 10 + rpId.rp() );
      protonRPType.push_back( rpId.subdetId() );

      if (proton.pz() < 0. ) {
	sector56.push_back( true );
	sector45.push_back( false );
      }
      else if (proton.pz() > 0. ) {
	sector45.push_back( true );
	sector56.push_back( false );
      }
      else {
	sector45.push_back( false );
        sector56.push_back( false );
      }

      for (const auto& tr : proton.contributingLocalTracks()) {
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

    auto ppsTab = std::make_unique<nanoaod::FlatTable>(trackX.size(),Form("PPSLocalTrack_%s",method_.c_str()),false);
    ppsTab->addColumn<float>("x",trackX,"local track x",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("xUnc",trackXUnc,"local track x uncertainty",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("y",trackY,"local track y",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("yUnc",trackYUnc,"local track y uncertainty",nanoaod::FlatTable::FloatColumn,precision_);
    if ( method_ == "multiRP" ) {
      ppsTab->addColumn<float>("time",trackTime,"local track time",nanoaod::FlatTable::FloatColumn,precision_);
      ppsTab->addColumn<float>("timeUnc",trackTimeUnc,"local track time uncertainty",nanoaod::FlatTable::FloatColumn,precision_);
    }
    ppsTab->addColumn<int>("protonIdx",trackIdx,"local track - proton correspondence",nanoaod::FlatTable::IntColumn);
    ppsTab->addColumn<int>("numPlanes",numPlanes,"number of points used for fit",nanoaod::FlatTable::IntColumn);
    ppsTab->addColumn<int>("pixelRecoInfo",pixelRecoInfo,"flag if a ROC was shifted by a bunchx",nanoaod::FlatTable::IntColumn);
    ppsTab->setDoc("ppsLocalTrack variables");
    
    std::unique_ptr<edm::ValueMap<int>> protonRPIdV(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler fillerID(*protonRPIdV);
    fillerID.insert(hRecoProtons, protonRPId.begin(), protonRPId.end());
    fillerID.fill();

    std::unique_ptr<edm::ValueMap<int>> protonRPTypeV(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler fillerSubID(*protonRPTypeV);
    fillerSubID.insert(hRecoProtons, protonRPType.begin(), protonRPType.end());
    fillerSubID.fill();

    std::unique_ptr<edm::ValueMap<bool>> sector45V(new edm::ValueMap<bool>());
    edm::ValueMap<bool>::Filler filler45(*sector45V);
    filler45.insert(hRecoProtons, sector45.begin(), sector45.end());
    filler45.fill();

    std::unique_ptr<edm::ValueMap<bool>> sector56V(new edm::ValueMap<bool>());
    edm::ValueMap<bool>::Filler filler56(*sector56V);
    filler56.insert(hRecoProtons, sector56.begin(), sector56.end());
    filler56.fill();

    iEvent.put(std::move(protonRPIdV), "protonRPId");
    iEvent.put(std::move(protonRPTypeV), "protonRPType");
    iEvent.put(std::move(sector45V), "sector45");
    iEvent.put(std::move(sector56V), "sector56");
    iEvent.put(std::move(ppsTab), "trackTable");
  }  

  // ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
  static void fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
  }
  
protected:
  const edm::EDGetTokenT<reco::ForwardProtonCollection> tokenRecoProtons_;
  const unsigned int precision_;
  const std::string method_;
};


DEFINE_FWK_MODULE(ProtonProducer);
