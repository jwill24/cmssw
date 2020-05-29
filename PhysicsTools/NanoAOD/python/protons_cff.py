import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *

protonTable = cms.EDProducer("ProtonProducer",
                             precision = cms.int32(14),
                             tagRecoProtonsSingle = cms.InputTag("ctppsProtons", "singleRP"),
                             tagRecoProtonsMulti  = cms.InputTag("ctppsProtons", "multiRP")
)

singleRPTable = cms.EDProducer("SimpleProtonTrackFlatTableProducer",
    src = cms.InputTag("ctppsProtons","singleRP"),
    cut = cms.string(""),
    name = cms.string("Proton_singleRP"),
    doc  = cms.string("bon"),
    singleton = cms.bool(False),
    extension = cms.bool(False),
    variables = cms.PSet(
        xi = Var("xi",float,doc="xi or dp/p",precision=10),
        xiError = Var("xiError",float,doc="error on xi or dp/p",precision=10),
        pt = Var("pt",float,doc="pt",precision=10),
        thetaY = Var("thetaY",float,doc="th y",precision=10),
        thetaYError = Var("thetaYError",float,doc="theta Y Error",precision=10),
        validFit = Var("validFit",bool,doc="valid Fit"),
    ),
    externalVariables = cms.PSet(
        decDetId = ExtVar("protonTable:singleRP_protonRPId",int,doc="Detector ID",precision=10),
        protonRPType = ExtVar("protonTable:singleRP_protonRPType",int,doc="Sub detector ID",precision=10),
        sector45 = ExtVar("protonTable:singleRP_sector45",bool,doc="LHC sector 45"),
        sector56 = ExtVar("protonTable:singleRP_sector56",bool,doc="LHC sector 56"),
    ),
)


multiRPTable = cms.EDProducer("SimpleProtonTrackFlatTableProducer",
    src = cms.InputTag("ctppsProtons","multiRP"),
    cut = cms.string(""),
    name = cms.string("Proton_multiRP"),
    doc  = cms.string("bon"),
    singleton = cms.bool(False),
    extension = cms.bool(False),
    variables = cms.PSet(
        xi = Var("xi",float,doc="xi or dp/p",precision=10),
        xiError = Var("xiError",float,doc="error on xi or dp/p",precision=10),
        vy = Var("vy()",float,doc="vy",precision=10),
        vyError = Var("vyError",float,doc="vy Error",precision=10),
        pt = Var("pt",float,doc="pt",precision=10),
        thetaX = Var("thetaX",float,doc="th x",precision=10),
        thetaXError = Var("thetaXError",float,doc="theta X Error",precision=10),
        thetaY = Var("thetaY",float,doc="th y",precision=10),
        chi2 = Var("chi2",float,doc="chi 2",precision=10),
        ndof = Var("ndof()",int, doc="n dof", precision=10),
        thetaYError = Var("thetaYError",float,doc="theta Y Error",precision=10),
        t = Var("t",float,doc="t",precision=10),
        validFit = Var("validFit",bool,doc="valid Fit"),
        time = Var("time()",float,doc="time",precision=10),
        timeError = Var("timeError",float,doc="time Error",precision=10),
    ),
    externalVariables = cms.PSet(
        sector45 = ExtVar("protonTable:multiRP_sector45",bool,doc="LHC sector 45"),
        sector56 = ExtVar("protonTable:multiRP_sector56",bool,doc="LHC sector 56"),
    ),
)


protonTables = cms.Sequence(    
    protonTable
    +singleRPTable
    +multiRPTable
)
