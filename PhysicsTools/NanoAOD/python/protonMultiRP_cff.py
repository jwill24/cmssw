import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *
from Configuration.StandardSequences.Eras import eras


multiRPProton = cms.EDProducer("ProtonProducer",
                                tagRecoProtons = cms.InputTag("ctppsProtons", "multiRP"),
                                precision = cms.int32(10),
                            )

multiRPProtonTable = cms.EDProducer("SimpleProtonTrackFlatTableProducer",
    src = cms.InputTag("ctppsProtons","multiRP"),
    cut = cms.string(""),
    name = cms.string("Proton_multiRP"),
    doc  = cms.string("bon"),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the muons
    variables = cms.PSet(
        
        xi = Var("xi",float,doc="xi or dp/p",precision=10),  
        xiError = Var("xiError",float,doc="error on xi or dp/p",precision=10),
        
        vy = Var("vy()",float,doc="vy",precision=10),

        #p = Var("p",float,doc="p",precision=10),
        pt = Var("pt",float,doc="pt",precision=10),
        
        thetaX = Var("thetaX",float,doc="th x",precision=10),
        thetaY = Var("thetaY",float,doc="th y",precision=10),
        
        chi2 = Var("chi2",float,doc="chi 2",precision=10),
        ndof = Var("ndof()",int, doc="n dof", precision=10),
        normalizedChi2 = Var("normalizedChi2",float,doc="normalized Chi 2",precision=10),

        thetaXError = Var("thetaXError",float,doc="theta X Error",precision=10),
        thetaYError = Var("thetaYError",float,doc="theta Y Error",precision=10),
        vyError = Var("vyError",float,doc="vy Error",precision=10),

        t = Var("t",float,doc="t",precision=10),

        validFit = Var("validFit",bool,doc="valid Fit"),  

        time = Var("time()",float,doc="time",precision=10),
        timeError = Var("timeError",float,doc="time Error",precision=10),        
    ),
    externalVariables = cms.PSet(
        sector45 = ExtVar("multiRPProton:sector45",bool,doc="LHC sector 45"),
        sector56 = ExtVar("multiRPProton:sector56",bool,doc="LHC sector 56"),
    ),
)


nmuTable = cms.EDProducer("GlobalVariablesTableProducer",
                          variables = cms.PSet(
                              SlimmedMuonsN = ExtVar( cms.InputTag("slimmedMuons"), "candidatesize", doc = "number of of all the slimmed muons" ),
                          )
                      )


multiRPProtonTables = cms.Sequence(
    multiRPProton +
    multiRPProtonTable +
    nmuTable 
)
