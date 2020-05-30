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

    std::vector<int> protonRPId, protonRPType;
    std::vector<bool> singleRP_sector45, singleRP_sector56, multiRP_sector45, multiRP_sector56;
    std::vector<float> trackX, trackXUnc, trackY, trackYUnc, trackTime, trackTimeUnc;
    std::vector<int> singleRP_trackIdx, multiRP_trackIdx, trackDetId, numPlanes, pixelRecoInfo;
    int detId;
    bool multiRP_proton = false;

    for (const auto &handle : {hRecoProtonsSingleRP, hRecoProtonsMultiRP}) {

      int num_proton = handle->size();
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
	  detId = (rpId.arm() * 100 + rpId.station() * 10 + rpId.rp() );
	  protonRPId.push_back( detId );
	  protonRPType.push_back( rpId.subdetId() );
	  singleRP_sector45.push_back( (proton.pz() > 0.) ? true : false );
	  singleRP_sector56.push_back( (proton.pz() < 0.) ? true : false );
	} 
	else if (method == 1) { //multiRP protons
	  if (!multiRP_proton) multiRP_trackIdx = std::vector<int>(trackX.size(),-1);
	  multiRP_proton = true;
	  multiRP_sector45.push_back( (proton.pz() > 0.) ? true : false );
	  multiRP_sector56.push_back( (proton.pz() < 0.) ? true : false );
	}
      
	for (const auto& tr : proton.contributingLocalTracks()) { // PPSLocalTracks
	  if (method == 0) {
	    singleRP_trackIdx.push_back( proton_pos );
	    trackX.push_back( tr->getX() );
	    trackXUnc.push_back( tr->getXUnc() );
	    trackY.push_back( tr->getY() );
	    trackYUnc.push_back( tr->getYUnc() );
	    trackTime.push_back( tr->getTime() );
	    trackTimeUnc.push_back( tr->getTimeUnc() );
	    trackDetId.push_back( detId );
	    numPlanes.push_back( tr->getNumberOfPointsUsedForFit() );
	    pixelRecoInfo.push_back( static_cast<int>(tr->getPixelTrackRecoInfo()) );
	  } else if (method == 1) {
	    auto it = std::find(trackX.begin(), trackX.end(), tr->getX() );
	    int index = std::distance(trackX.begin(), it);
	    multiRP_trackIdx[index] = proton_pos;
	  }
	} 
	proton_pos++;
      } 
    }

    while ( multiRP_trackIdx.size() < singleRP_trackIdx.size() ) multiRP_trackIdx.push_back(-1);

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
  } 

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
