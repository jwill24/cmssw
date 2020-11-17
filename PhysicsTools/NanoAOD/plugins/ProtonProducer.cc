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

#include "CommonTools/Egamma/interface/EffectiveAreas.h"

#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "DataFormats/CTPPSDetId/interface/CTPPSDetId.h"
#include "DataFormats/CTPPSReco/interface/CTPPSLocalTrackLite.h"
#include "DataFormats/ProtonReco/interface/ForwardProton.h"
#include "DataFormats/ProtonReco/interface/ForwardProtonFwd.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"

class ProtonProducer : public edm::global::EDProducer<> {
public:
  ProtonProducer(edm::ParameterSet const &ps)
      : tokenRecoProtonsMultiRP_(
	    mayConsume<reco::ForwardProtonCollection>(ps.getParameter<edm::InputTag>("tagRecoProtonsMulti"))),
	tokenRecoProtonsSingleRP_(
	    mayConsume<reco::ForwardProtonCollection>(ps.getParameter<edm::InputTag>("tagRecoProtonsSingle"))),
        storeSingleRPProtons_(
			      ps.getParameter<bool>("storeSingleRPProtons")) {
    produces<edm::ValueMap<int>>("arm");
    produces<nanoaod::FlatTable>("ppsTrackTable");
    if (storeSingleRPProtons_) produces<edm::ValueMap<int>>("protonRPId");
  }
  ~ProtonProducer() override {}

  // ------------ method called to produce the data  ------------
  void produce(edm::StreamID id, edm::Event &iEvent, const edm::EventSetup &iSetup) const override {
    // Get Forward Proton handle
    edm::Handle<reco::ForwardProtonCollection> hRecoProtonsMultiRP;
    iEvent.getByToken(tokenRecoProtonsMultiRP_, hRecoProtonsMultiRP);

    edm::Handle<reco::ForwardProtonCollection> hRecoProtonsSingleRP;
    iEvent.getByToken(tokenRecoProtonsSingleRP_, hRecoProtonsSingleRP);

    // book output variables for protons
    std::vector<int> multiRP_arm;

    // book output variables for tracks
    std::vector<float> trackX, trackY, trackTime, trackTimeUnc;
    std::vector<int> multiRPProtonIdx, decRPId, rpType;
    std::vector<int> singleRPProtonIdx, singleRP_protonRPId;


    for (unsigned int p_idx = 0; p_idx < hRecoProtonsMultiRP->size(); ++p_idx) {
      const auto &proton = hRecoProtonsMultiRP->at(p_idx);
      multiRP_arm.push_back((proton.pz() < 0.) ? 1 : 0);

      for (const auto &ref : proton.contributingLocalTracks()) {
	multiRPProtonIdx.push_back(p_idx);
	if (storeSingleRPProtons_) singleRPProtonIdx.push_back(-1);
	CTPPSDetId rpId(ref->rpId());
	unsigned int rpDecId = rpId.arm() * 100 + rpId.station() * 10 + rpId.rp();
	decRPId.push_back(rpDecId);
	rpType.push_back(rpId.subdetId());
	trackX.push_back(ref->x());
	trackY.push_back(ref->y());
	trackTime.push_back(ref->time());
	trackTimeUnc.push_back(ref->timeUnc());
      }
    }

    if (storeSingleRPProtons_) {
      for (unsigned int p_idx = 0; p_idx < hRecoProtonsSingleRP->size(); ++p_idx) {
	const auto &proton = hRecoProtonsSingleRP->at(p_idx);
	CTPPSDetId rpId((*proton.contributingLocalTracks().begin())->rpId());
	unsigned int rpDecId = rpId.arm() * 100 + rpId.station() * 10 + rpId.rp();
	singleRP_protonRPId.push_back(rpDecId);
	
	for (const auto &ref : proton.contributingLocalTracks()) {
	  unsigned int nMulti = hRecoProtonsMultiRP->size();
	  singleRPProtonIdx.push_back(p_idx + nMulti);
	  multiRPProtonIdx.push_back(-1);
	  CTPPSDetId rpId(ref->rpId());
	  unsigned int rpDecId = rpId.arm() * 100 + rpId.station() * 10 + rpId.rp();
	  decRPId.push_back(rpDecId);
	  rpType.push_back(rpId.subdetId());
	  trackX.push_back(ref->x());
	  trackY.push_back(ref->y());
	  trackTime.push_back(ref->time());
	  trackTimeUnc.push_back(ref->timeUnc());
	}
      }
    }

    // update proton tables
    std::unique_ptr<edm::ValueMap<int>> multiRP_armV(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler fillermultiArm(*multiRP_armV);
    fillermultiArm.insert(hRecoProtonsMultiRP, multiRP_arm.begin(), multiRP_arm.end());
    fillermultiArm.fill();

    std::unique_ptr<edm::ValueMap<int>> protonRPIdV(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler fillerID(*protonRPIdV);
    if (storeSingleRPProtons_) {
      fillerID.insert(hRecoProtonsSingleRP, singleRP_protonRPId.begin(), singleRP_protonRPId.end());
      fillerID.fill();
    }

    // build track table
    auto ppsTab = std::make_unique<nanoaod::FlatTable>(trackX.size(), "PPSLocalTrack", false);
    ppsTab->addColumn<int>("multiRPProtonIdx", multiRPProtonIdx, "local track - proton correspondence");
    if (storeSingleRPProtons_) ppsTab->addColumn<int>("singleRPProtonIdx", singleRPProtonIdx, "local track - proton correspondence");
    ppsTab->addColumn<float>("x", trackX, "local track x", 16);
    ppsTab->addColumn<float>("y", trackY, "local track y", 13);
    ppsTab->addColumn<float>("time", trackTime, "local track time", 16);
    ppsTab->addColumn<float>("timeUnc", trackTimeUnc, "local track time uncertainty", 13);
    ppsTab->addColumn<int>("decRPId", decRPId, "local track detector dec id");
    ppsTab->addColumn<int>("rpType", rpType, "strip=3, pixel=4, diamond=5, timing=6");
    ppsTab->setDoc("ppsLocalTrack variables");

    // save output
    iEvent.put(std::move(multiRP_armV), "arm");
    iEvent.put(std::move(ppsTab), "ppsTrackTable");
    if (storeSingleRPProtons_) iEvent.put(std::move(protonRPIdV), "protonRPId");
  }

  // ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("tagRecoProtonsMulti")->setComment("multiRP proton collection");
    desc.add<edm::InputTag>("tagRecoProtonsSingle")->setComment("singleRP proton collection");
    desc.add<bool>("storeSingleRPProtons")->setComment("flag to store singleRP protons and associated tracks");
    descriptions.add("ProtonProducer", desc);
  }

protected:
  const edm::EDGetTokenT<reco::ForwardProtonCollection> tokenRecoProtonsMultiRP_;
  const edm::EDGetTokenT<reco::ForwardProtonCollection> tokenRecoProtonsSingleRP_;
  const bool storeSingleRPProtons_;
};

DEFINE_FWK_MODULE(ProtonProducer);
