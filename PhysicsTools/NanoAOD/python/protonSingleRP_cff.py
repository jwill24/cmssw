import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *

singleRPProton = cms.EDProducer("ProtonProducer",
                                tagRecoProtons = cms.InputTag("ctppsProtons", "singleRP"),
                                precision = cms.int32(10),
                            )

singleRPProtonTable = cms.EDProducer("SimpleProtonTrackFlatTableProducer",
    src = cms.InputTag("ctppsProtons","singleRP"),
    cut = cms.string(""),
    name = cms.string("Proton_singleRP"),
    doc  = cms.string("bon"),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the muons
    variables = cms.PSet(

        xi = Var("xi",float,doc="xi or dp/p",precision=10),  
        xiError = Var("xiError",float,doc="error on xi or dp/p",precision=10),
                                         
        #p = Var("p",float,doc="p",precision=10),
        pt = Var("pt",float,doc="pt",precision=10),
        
        thetaY = Var("thetaY",float,doc="th y",precision=10),
        
        chi2 = Var("chi2()",float,doc="chi 2",precision=10),
        ndof = Var("ndof()",int, doc="n dof", precision=10),
        normalizedChi2 = Var("normalizedChi2()",float,doc="normalized Chi 2",precision=10),
        
        thetaYError = Var("thetaYError",float,doc="theta Y Error",precision=10),
        
        t = Var("t",float,doc="t",precision=10),
        
        validFit = Var("validFit",bool,doc="valid Fit"),

                                         
    ),

    externalVariables = cms.PSet(
        decRpId = ExtVar("singleRPProton:protonsDetId",int,doc="Detector ID",precision=10),
        sector45 = ExtVar("singleRPProton:sector45",bool,doc="LHC sector 45"),
        sector56 = ExtVar("singleRPProton:sector56",bool,doc="LHC sector 56"),
    ),
)
    
singleRPProtonTables = cms.Sequence( singleRPProton + singleRPProtonTable )
    
