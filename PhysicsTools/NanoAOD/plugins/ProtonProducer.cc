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
    tokenTracksLite_(mayConsume<std::vector<CTPPSLocalTrackLite>>(ps.getParameter<edm::InputTag>("tagTrackLite"))),
    precision_( ps.getParameter<int>("precision") )
  {
    produces<edm::ValueMap<int>>("protonRPId");
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

    // Get PPS Local Track handle
    edm::Handle<std::vector<CTPPSLocalTrackLite> > ppsTracksLite;
    iEvent.getByToken( tokenTracksLite_, ppsTracksLite );

    std::vector<int> protonRPId, protonRPType;
    std::vector<bool> singleRP_sector45, singleRP_sector56, multiRP_sector45, multiRP_sector56;
    std::vector<float> trackX, trackXUnc, trackY, trackYUnc, trackTime, trackTimeUnc, localSlopeX, localSlopeY, normalizedChi2;
    std::vector<int> singleRPProtonIdx, multiRPProtonIdx, decRPId, numFitPoints, pixelRecoInfo, rpType;
    int detId, subdetId;
    bool multiRP_proton = false;

    for (const auto &handle : {hRecoProtonsSingleRP, hRecoProtonsMultiRP}) {

      int num_proton = handle->size();
      int proton_pos = 0; // indexing convention for cross-linking
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
	  subdetId = rpId.subdetId();
	  protonRPId.push_back( detId );
	  protonRPType.push_back( rpId.subdetId() );
	  singleRP_sector45.push_back( (proton.pz() > 0.) ? true : false );
	  singleRP_sector56.push_back( (proton.pz() < 0.) ? true : false );
	} 
	else if (method == 1) { //multiRP protons
	  if (!multiRP_proton) multiRPProtonIdx = std::vector<int>(trackX.size(),-1);
	  multiRP_proton = true;
	  multiRP_sector45.push_back( (proton.pz() > 0.) ? true : false );
	  multiRP_sector56.push_back( (proton.pz() < 0.) ? true : false );
	}
      
	for (const auto& tr : proton.contributingLocalTracks()) { // PPSLocalTracks
	  if (method == 0) {
	    singleRPProtonIdx.push_back( proton_pos );
	    trackX.push_back( tr->getX() );
	    trackXUnc.push_back( tr->getXUnc() );
	    trackY.push_back( tr->getY() );
	    trackYUnc.push_back( tr->getYUnc() );
	    trackTime.push_back( tr->getTime() );
	    trackTimeUnc.push_back( tr->getTimeUnc() );
	    decRPId.push_back( detId );
	    numFitPoints.push_back( tr->getNumberOfPointsUsedForFit() );
	    pixelRecoInfo.push_back( static_cast<int>(tr->getPixelTrackRecoInfo()) );
	    normalizedChi2.push_back( tr->getChiSquaredOverNDF() );
	    rpType.push_back( subdetId );
	    localSlopeX.push_back( tr->getTx() );
	    localSlopeY.push_back( tr->getTy() );
	  } else if (method == 1) {
	    auto it = std::find(trackX.begin(), trackX.end(), tr->getX() );
	    int index = std::distance(trackX.begin(), it);
	    multiRPProtonIdx[index] = proton_pos;
	  }
	} 
	proton_pos++;
      } 
    }

    while ( multiRPProtonIdx.size() < singleRPProtonIdx.size() ) multiRPProtonIdx.push_back(-1); // fill empy multiRP indices with -1

    for ( const auto& trk : *ppsTracksLite ) { // Store PPS timing tracks
      CTPPSDetId rpId(trk.getRPId());
      int detId = (rpId.arm() * 100 + rpId.station() * 10 + rpId.rp() );
      if (detId == 16 or detId == 116) {
	singleRPProtonIdx.push_back(-1);
	multiRPProtonIdx.push_back(-1);
	trackX.push_back( trk.getX() );
	trackXUnc.push_back( trk.getXUnc() );
	trackY.push_back( trk.getY() );
	trackYUnc.push_back( trk.getYUnc() );
	trackTime.push_back( trk.getTime() );
	trackTimeUnc.push_back( trk.getTimeUnc() );
	decRPId.push_back( rpId.arm() * 100 + rpId.station() * 10 + rpId.rp() );
	numFitPoints.push_back( trk.getNumberOfPointsUsedForFit() );
	pixelRecoInfo.push_back( static_cast<int>(trk.getPixelTrackRecoInfo()) );
	normalizedChi2.push_back( trk.getChiSquaredOverNDF() );
	rpType.push_back( rpId.subdetId() );
	localSlopeX.push_back( trk.getTx() );
	localSlopeY.push_back( trk.getTy() );
      }
    }


    auto ppsTab = std::make_unique<nanoaod::FlatTable>(trackX.size(),"PPSLocalTrack",false);
    ppsTab->addColumn<int>("singleRPProtonIdx",singleRPProtonIdx,"local track - proton correspondence",nanoaod::FlatTable::IntColumn);
    ppsTab->addColumn<int>("multiRPProtonIdx",multiRPProtonIdx,"local track - proton correspondence",nanoaod::FlatTable::IntColumn);
    ppsTab->addColumn<float>("x",trackX,"local track x",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("xUnc",trackXUnc,"local track x uncertainty",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("y",trackY,"local track y",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("yUnc",trackYUnc,"local track y uncertainty",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("time",trackTime,"local track time",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("timeUnc",trackTimeUnc,"local track time uncertainty",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<int>("decRPId",decRPId,"local track detector dec id",nanoaod::FlatTable::IntColumn);
    ppsTab->addColumn<int>("numFitPoints",numFitPoints,"number of points used for fit",nanoaod::FlatTable::IntColumn);
    ppsTab->addColumn<int>("pixelRecoInfo",pixelRecoInfo,"flag if a ROC was shifted by a bunchx",nanoaod::FlatTable::IntColumn);
    ppsTab->addColumn<float>("normalizedChi2",normalizedChi2,"chi2 over NDF",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<int>("rpType",rpType,"strip=3, pixel=4, diamond=5, timing=6",nanoaod::FlatTable::IntColumn);
    ppsTab->addColumn<float>("localSlopeX",localSlopeX,"track horizontal angle",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->addColumn<float>("localSlopeY",localSlopeY,"track vertical angle",nanoaod::FlatTable::FloatColumn,precision_);
    ppsTab->setDoc("ppsLocalTrack variables");
    
    std::unique_ptr<edm::ValueMap<int>> protonRPIdV(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler fillerID(*protonRPIdV);
    fillerID.insert(hRecoProtonsSingleRP, protonRPId.begin(), protonRPId.end());
    fillerID.fill();

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
  const edm::EDGetTokenT<std::vector<CTPPSLocalTrackLite> > tokenTracksLite_;
  const unsigned int precision_;
};


DEFINE_FWK_MODULE(ProtonProducer);
