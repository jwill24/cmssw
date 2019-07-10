# LHC info

import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *


lhcInfo = cms.EDProducer("lhcInfoProducer",
    precision = cms.int32(10),
)



#lhcInfoTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
#    src = cms.InputTag("lhcInfo"),
#    cut = cms.string(""),
#    name = cms.string("LHCInfo"),
#    doc  = cms.string("bon"),
#    singleton = cms.bool(False), # the number of entries is variable
#    extension = cms.bool(False), # this is the main table for the muons
#    variables = cms.PSet(
#        xangle = Var("xangle",float,doc="LHC crossing anlge",precision=10)
#    ),
#    externalVariables = cms.PSet(
        #xangle = ExtVar("lhcInfo:xangle",float,doc="LHC crossing angle",precision=10)
#    ),
#)

lhcInfoTables = cms.Sequence(lhcInfo)
