import FWCore.ParameterSet.Config as cms

process = getattr(__import__("PhysicsTools.NanoAOD.myMiniAOD_cfg",fromlist=["process"]),"process")
subprocess = getattr(__import__("PhysicsTools.NanoAOD.myNanoAOD_cfg",fromlist=["process"]),"process")

output = sorted(process.outputModules_())

process.addSubProcess(
    cms.SubProcess(
        process = subprocess,
        SelectEvents = cms.untracked.PSet(),
        outputCommands = getattr(process,output[0]).outputCommands,
    )
)

delattr(process,output[0])
