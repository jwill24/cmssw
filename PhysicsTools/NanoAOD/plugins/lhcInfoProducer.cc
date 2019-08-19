// -*- C++ -*-
//
// Package:    PhysicsTools/NanoAOD
// Class:      lhcInfoProducer
//
/**\class lhcInfoProducer lhcInfoProducer.cc PhysicsTools/NanoAOD/plugins/lhcInfoProducer.cc
 Description: [one line class summary]
 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Justin Williams
//         Created: 05 Jul 2019 14:06:12 GMT
//
//

// System include files
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

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "FWCore/Framework/interface/SourceFactory.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"

#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "FWCore/Utilities/interface/transform.h"

// LHC Info
#include "CondFormats/RunInfo/interface/LHCInfo.h"
#include "CondFormats/DataRecord/interface/LHCInfoRcd.h"
#include "CondTools/RunInfo/interface/LHCInfoPopConSourceHandler.h"

class lhcInfoProducer : public edm::global::EDProducer<> {
public:
  lhcInfoProducer( edm::ParameterSet const & ps) :
    precision_( ps.getParameter<int>("precision") )
  {
    produces<nanoaod::FlatTable>("lhcInfoTab");
  }
  ~lhcInfoProducer() override {}


  // ------------ method called to produce the data  ------------
  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {

    // Get LHCInfo handle
    edm::ESHandle<LHCInfo> lhcInfo;
    iSetup.get<LHCInfoRcd>().get(lhcInfo);

    std::unique_ptr<nanoaod::FlatTable> out = fillTable(iEvent, lhcInfo);
    out->setDoc("LHC crossing angle");

    iEvent.put(std::move(out),"lhcInfoTab");

  }

  std::unique_ptr<nanoaod::FlatTable> fillTable(const edm::Event &iEvent, const edm::ESHandle<LHCInfo> & prod) const {
    
    const LHCInfo* info = prod.product();
    float xangle = info->crossingAngle();
    float betaStar = info->betaStar();

    auto out = std::make_unique<nanoaod::FlatTable>(1,"LHCInfo",true);
    out->addColumnValue<float>("xangle", xangle, "LHC crossing angle", nanoaod::FlatTable::FloatColumn);
    out->addColumnValue<float>("betaStar",betaStar,"LHC beta star", nanoaod::FlatTable::FloatColumn);
    return out;
  }

  // ------------ method fills 'descriptions' with the allowed parameters for the module  ------------

  static void fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
  }

protected:
  const unsigned int precision_;
};


DEFINE_FWK_MODULE(lhcInfoProducer);
