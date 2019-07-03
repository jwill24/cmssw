import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *
from Configuration.StandardSequences.Eras import eras

ppsLocalTrackTable = cms.EDProducer("SimpleLocalTrackFlatTableProducer",
    src = cms.InputTag("ctppsLocalTrackLiteProducer"),
    cut = cms.string(""),
    name = cms.string("ppsLocalTrack"),
    doc  = cms.string("bon"),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the muons
    variables = cms.PSet(
        
        x = Var("getX()",float,doc="x",precision=10),
        y = Var("getY()",float,doc="y",precision=10),
        
        rpId = Var("getRPId()",int,doc="rpId")

    ),
)


ppsLocalTrackTables = cms.Sequence(ppsLocalTrackTable)
