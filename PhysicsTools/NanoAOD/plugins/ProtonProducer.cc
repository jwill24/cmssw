// -*- C++ -*-
//
// Package:    PhysicsTools/NanoAOD
// Class:      ProtonProducer
//
/**\class ProtonProducer ProtonProducer.cc PhysicsTools/NanoAOD/plugins/ProtonProducer.cc
 Description: [one line class summary]
 Implementation:
     [Notes on implementation]
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


class ProtonProducer : public edm::global::EDProducer<> {
public:
  ProtonProducer( edm::ParameterSet const & ps) :
    tokenRecoProtons_(consumes<reco::ForwardProtonCollection>(ps.getParameter<edm::InputTag>("tagRecoProtons"))),
    precision_( ps.getParameter<int>("precision") )
  {
    produces<edm::ValueMap<int>>("protonsDetId");
    produces<edm::ValueMap<bool>>("sector45");
    produces<edm::ValueMap<bool>>("sector56");
  }
  ~ProtonProducer() override {}
  
  
  
  
  // ------------ method called to produce the data  ------------
  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {
    
    edm::Handle<reco::ForwardProtonCollection> hRecoProtonsSingleRP;
    iEvent.getByToken(tokenRecoProtons_, hRecoProtonsSingleRP);

    std::vector<int> protonsDetId;
    std::vector<bool> sector45;
    std::vector<bool> sector56;
    int num_proton = hRecoProtonsSingleRP->size();

    protonsDetId.reserve( num_proton );
    sector45.reserve( num_proton );
    sector56.reserve( num_proton );

    for (const auto &proton : *hRecoProtonsSingleRP) {
      CTPPSDetId rpId((*proton.contributingLocalTracks().begin())->getRPId());
      protonsDetId.push_back( rpId.arm() * 100 + rpId.station() * 10 + rpId.rp() );

      if (proton.pz() < 0. ) {
	sector56.push_back( true );
	sector45.push_back( false );
      }
      else if (proton.pz() > 0. ) {
	sector45.push_back( true );
	sector56.push_back( false );
      }
    }

    std::unique_ptr<edm::ValueMap<int>> protonDetIdV(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler fillerID(*protonDetIdV);
    fillerID.insert(hRecoProtonsSingleRP, protonsDetId.begin(), protonsDetId.end());
    fillerID.fill();

    std::unique_ptr<edm::ValueMap<bool>> sector45V(new edm::ValueMap<bool>());
    edm::ValueMap<bool>::Filler filler45(*sector45V);
    filler45.insert(hRecoProtonsSingleRP, sector45.begin(), sector45.end());
    filler45.fill();

    std::unique_ptr<edm::ValueMap<bool>> sector56V(new edm::ValueMap<bool>());
    edm::ValueMap<bool>::Filler filler56(*sector56V);
    filler56.insert(hRecoProtonsSingleRP, sector56.begin(), sector56.end());
    filler56.fill();
    
    iEvent.put(std::move(protonDetIdV), "protonsDetId");
    iEvent.put(std::move(sector45V), "sector45");
    iEvent.put(std::move(sector56V), "sector56");

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
  
  
};


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(ProtonProducer);
