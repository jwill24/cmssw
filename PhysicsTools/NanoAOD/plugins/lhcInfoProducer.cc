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
    //lhcInfoLabel_(ps.getParameter<std::string>("lhcInfoLabel"))
  {
    //produces<float>("xangle");
    produces<nanoaod::FlatTable>("xangle");
  }
  ~lhcInfoProducer() override {}



  // ------------ method called to produce the data  ------------
  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {

    // Get LHCInfo handle
    edm::ESHandle<LHCInfo> lhcInfo;
    iSetup.get<LHCInfoRcd>().get(lhcInfo);

    //const LHCInfo* info = lhcInfo.product();
    //float xangle = info->crossingAngle();


    std::unique_ptr<nanoaod::FlatTable> out = fillTable(iEvent, lhcInfo);
    out->setDoc("LHC crossing angle");

    iEvent.put(std::move(out),"xangle");

  }


  std::unique_ptr<nanoaod::FlatTable> fillTable(const edm::Event &iEvent, const edm::ESHandle<LHCInfo> & prod) const {
    
    const LHCInfo* info = prod.product();
    float xangle = info->crossingAngle();

    /*std::vector<const T *> selobjs;
    std::vector<edm::Ptr<T>> selptrs; // for external variables                                                                                                                                             
    if (singleton_) {
      assert(prod->size() == 1);
      selobjs.push_back(& (*prod)[0] );
      if (!extvars_.empty()) selptrs.emplace_back(prod->ptrAt(0));
    } else {
      for (unsigned int i = 0, n = prod->size(); i < n; ++i) {
        const auto & obj = (*prod)[i];
	if (cut_(obj)) {
          selobjs.push_back(&obj);
          if (!extvars_.empty()) selptrs.emplace_back(prod->ptrAt(i));
	}
        if(selobjs.size()>=maxLen_) break;
      }
    }
    auto out = std::make_unique<nanoaod::FlatTable>(selobjs.size(), this->name_, singleton_, this->extension_);
    for (const auto & var : this->vars_) var.fill(selobjs, *out);
    for (const auto & var : this->extvars_) var.fill(iEvent, selptrs, *out);*/

    auto out = std::make_unique<nanoaod::FlatTable>(1,"LHCInfo",false);
    out.addColumnValue<float>("xangle", xangle, "LHC crossing angle", nanoaod::FlatTable::FloatColumn);

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
  //std::string lhcInfoLabel_;
  
};


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(lhcInfoProducer);
