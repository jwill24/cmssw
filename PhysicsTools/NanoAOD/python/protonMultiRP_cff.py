import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *
from Configuration.StandardSequences.Eras import eras

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
        
        #vx = Var("vx()",float,doc="vx",precision=10), #bad
        vy = Var("vy()",float,doc="vy",precision=10),
        #vz = Var("vz()",float,doc="vz",precision=10), #bad

        p = Var("p",float,doc="p",precision=10),
        pt = Var("pt",float,doc="pt",precision=10),
        #px = Var("px",float,doc="px",precision=10),
        #py = Var("py",float,doc="py",precision=10),
        #pz = Var("pz",float,doc="pz",precision=10),
        
        thx = Var("thetaX",float,doc="th x",precision=10),
        thy = Var("thetaY",float,doc="th y",precision=10),
        
        chi2 = Var("chi2",float,doc="chi 2",precision=10),
        ndof = Var("ndof()",int, doc="n dof", precision=10), #bad
        normalizedChi2 = Var("normalizedChi2",float,doc="normalized Chi 2",precision=10),

        thetaXError = Var("thetaXError",float,doc="theta X Error",precision=10),
        thetaYError = Var("thetaYError",float,doc="theta Y Error",precision=10),
        #vxError = Var("vxError",float,doc="vx Error",precision=10), #bad
        vyError = Var("vyError",float,doc="vy Error",precision=10),

        t = Var("t",float,doc="t",precision=10),

        validFit = Var("validFit",bool,doc="valid Fit"),  

        time = Var("time()",float,doc="time",precision=10),
        timeError = Var("timeError",float,doc="time Error",precision=10),        
    ),
)


nmuTable = cms.EDProducer("GlobalVariablesTableProducer",
                          variables = cms.PSet(
                              SlimmedMuonsN = ExtVar( cms.InputTag("slimmedMuons"), "candidatesize", doc = "number of of all the slimmed muons" ),
                          )
                      )


multiRPProtonTables = cms.Sequence(
    multiRPProtonTable +
    nmuTable 
)
