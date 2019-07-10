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
    tokenRecoProtonsSingleRP_(consumes<reco::ForwardProtonCollection>(ps.getParameter<edm::InputTag>("tagRecoProtonsSingleRP"))),
    precision_( ps.getParameter<int>("precision") )
  {
    produces<edm::ValueMap<int>>("protonsDetId");
  }
  ~ProtonProducer() override {}
  
  
  
  
  // ------------ method called to produce the data  ------------
  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {
    
    edm::Handle<reco::ForwardProtonCollection> hRecoProtonsSingleRP;
    iEvent.getByToken(tokenRecoProtonsSingleRP_, hRecoProtonsSingleRP);

    std::vector<int> protonsDetId;

    protonsDetId.reserve( hRecoProtonsSingleRP->size() );

    for (const auto &proton : *hRecoProtonsSingleRP) {
      CTPPSDetId rpId((*proton.contributingLocalTracks().begin())->getRPId());
      protonsDetId.push_back( rpId.arm() * 100 + rpId.station() * 10 + rpId.rp() );
    }

    std::unique_ptr<edm::ValueMap<int>> protonDetIdV(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler fillerID(*protonDetIdV);
    fillerID.insert(hRecoProtonsSingleRP, protonsDetId.begin(), protonsDetId.end());
    fillerID.fill();
    
    iEvent.put(std::move(protonDetIdV), "protonsDetId");

  }  
  
  // ------------ method fills 'descriptions' with the allowed parameters for the module  ------------

  static void fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
  }
  

protected:
  const edm::EDGetTokenT<reco::ForwardProtonCollection> tokenRecoProtonsSingleRP_;
  const unsigned int precision_;
  
  
};


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(ProtonProducer);
