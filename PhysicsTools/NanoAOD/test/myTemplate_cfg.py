##################################
# Original author: Justin Williams
# 17 June 2019
# ja.williams@cern.ch
##################################

import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run2_2018_cff import Run2_2018

process = cms.Process('PAT',Run2_2018)

print('Load files for process')
# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('PhysicsTools.PatAlgos.slimming.metFilterPaths_cff')
process.load('Configuration.StandardSequences.PAT_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

print('Load GT for process')
# Global Tag
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_dataRun2_v11', '')

print('Load input source')
# Input source
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
                                'file:FF8D082F-08BB-254D-85F1-242DAA30818D.root',
                                #'file:D6ED7520-C256-4540-BB9A-71ACFA322F0A.root',                               
                            ),
                            secondaryFileNames = cms.untracked.vstring()
                        )
process.options = cms.untracked.PSet(
)

# Output definition
process.MINIAODoutput = cms.OutputModule("PoolOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(4),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string(''),
        filterName = cms.untracked.string('')
    ),
    dropMetaData = cms.untracked.string('ALL'),
    eventAutoFlushCompressedSize = cms.untracked.int32(-900),
    fastCloning = cms.untracked.bool(False),
    fileName = cms.untracked.string('miniAOD-prod_PAT.root'),
    outputCommands = process.MINIAODEventContent.outputCommands,
    overrideBranchesSplitLevel = cms.untracked.VPSet(
        cms.untracked.PSet(
            branch = cms.untracked.string('patPackedCandidates_packedPFCandidates__*'),
            splitLevel = cms.untracked.int32(99)
        ),
        cms.untracked.PSet(
            branch = cms.untracked.string('recoGenParticles_prunedGenParticles__*'),
            splitLevel = cms.untracked.int32(99)
        ),
        cms.untracked.PSet(
            branch = cms.untracked.string('patTriggerObjectStandAlones_slimmedPatTrigger__*'),
            splitLevel = cms.untracked.int32(99)
        ),
        cms.untracked.PSet(
            branch = cms.untracked.string('patPackedGenParticles_packedGenParticles__*'),
            splitLevel = cms.untracked.int32(99)
       ),
       cms.untracked.PSet(
            branch = cms.untracked.string('patJets_slimmedJets__*'),
            splitLevel = cms.untracked.int32(99)
        ),
        cms.untracked.PSet(
            branch = cms.untracked.string('recoVertexs_offlineSlimmedPrimaryVertices__*'),
            splitLevel = cms.untracked.int32(99)
        ),
        cms.untracked.PSet(
            branch = cms.untracked.string('recoCaloClusters_reducedEgamma_reducedESClusters_*'),
            splitLevel = cms.untracked.int32(99)
        ),
        cms.untracked.PSet(
            branch = cms.untracked.string('EcalRecHitsSorted_reducedEgamma_reducedEBRecHits_*'),
            splitLevel = cms.untracked.int32(99)
        ),
        cms.untracked.PSet(
            branch = cms.untracked.string('EcalRecHitsSorted_reducedEgamma_reducedEERecHits_*'),
            splitLevel = cms.untracked.int32(99)
        ),
        cms.untracked.PSet(
            branch = cms.untracked.string('recoGenJets_slimmedGenJets__*'),
            splitLevel = cms.untracked.int32(99)
        ),
        cms.untracked.PSet(
            branch = cms.untracked.string('patJets_slimmedJetsPuppi__*'),
            splitLevel = cms.untracked.int32(99)
        ),
        cms.untracked.PSet(
            branch = cms.untracked.string('EcalRecHitsSorted_reducedEgamma_reducedESRecHits_*'),
            splitLevel = cms.untracked.int32(99)
        )
   ),
    overrideInputFileSplitLevels = cms.untracked.bool(True),
    splitLevel = cms.untracked.int32(0)
)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('miniAOD-prod nevts:1'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)


# Path and EndPath definitions
process.Flag_trackingFailureFilter = cms.Path(process.goodVertices+process.trackingFailureFilter)
process.Flag_goodVertices = cms.Path(process.primaryVertexFilter)
process.Flag_CSCTightHaloFilter = cms.Path(process.CSCTightHaloFilter)
process.Flag_trkPOGFilters = cms.Path(process.trkPOGFilters)
process.Flag_HcalStripHaloFilter = cms.Path(process.HcalStripHaloFilter)
process.Flag_trkPOG_logErrorTooManyClusters = cms.Path(~process.logErrorTooManyClusters)
process.Flag_EcalDeadCellTriggerPrimitiveFilter = cms.Path(process.EcalDeadCellTriggerPrimitiveFilter)
process.Flag_ecalLaserCorrFilter = cms.Path(process.ecalLaserCorrFilter)
process.Flag_globalSuperTightHalo2016Filter = cms.Path(process.globalSuperTightHalo2016Filter)
process.Flag_eeBadScFilter = cms.Path(process.eeBadScFilter)
process.Flag_METFilters = cms.Path(process.metFilters)
process.Flag_chargedHadronTrackResolutionFilter = cms.Path(process.chargedHadronTrackResolutionFilter)
process.Flag_globalTightHalo2016Filter = cms.Path(process.globalTightHalo2016Filter)
process.Flag_CSCTightHaloTrkMuUnvetoFilter = cms.Path(process.CSCTightHaloTrkMuUnvetoFilter)
process.Flag_HBHENoiseIsoFilter = cms.Path(process.HBHENoiseFilterResultProducer+process.HBHENoiseIsoFilter)
process.Flag_BadChargedCandidateSummer16Filter = cms.Path(process.BadChargedCandidateSummer16Filter)
process.Flag_hcalLaserEventFilter = cms.Path(process.hcalLaserEventFilter)
process.Flag_BadPFMuonFilter = cms.Path(process.BadPFMuonFilter)
process.Flag_ecalBadCalibFilter = cms.Path(process.ecalBadCalibFilter)
process.Flag_HBHENoiseFilter = cms.Path(process.HBHENoiseFilterResultProducer+process.HBHENoiseFilter)
process.Flag_trkPOG_toomanystripclus53X = cms.Path(~process.toomanystripclus53X)
process.Flag_EcalDeadCellBoundaryEnergyFilter = cms.Path(process.EcalDeadCellBoundaryEnergyFilter)
process.Flag_BadChargedCandidateFilter = cms.Path(process.BadChargedCandidateFilter)
process.Flag_trkPOG_manystripclus53X = cms.Path(~process.manystripclus53X)
process.Flag_BadPFMuonSummer16Filter = cms.Path(process.BadPFMuonSummer16Filter)
process.Flag_muonBadTrackFilter = cms.Path(process.muonBadTrackFilter)
process.Flag_CSCTightHalo2015Filter = cms.Path(process.CSCTightHalo2015Filter)
process.endjob_step = cms.EndPath(process.endOfProcess)

process.schedule = cms.Schedule(
    process.Flag_HBHENoiseFilter,
    process.Flag_HBHENoiseIsoFilter,
    process.Flag_CSCTightHaloFilter,
    process.Flag_CSCTightHaloTrkMuUnvetoFilter,
    process.Flag_CSCTightHalo2015Filter,
    process.Flag_globalTightHalo2016Filter,
    process.Flag_globalSuperTightHalo2016Filter,
    process.Flag_HcalStripHaloFilter,
    process.Flag_hcalLaserEventFilter,
    process.Flag_EcalDeadCellTriggerPrimitiveFilter,
    process.Flag_EcalDeadCellBoundaryEnergyFilter,
    process.Flag_ecalBadCalibFilter,
    process.Flag_goodVertices,
    process.Flag_eeBadScFilter,
    process.Flag_ecalLaserCorrFilter,
    process.Flag_trkPOGFilters,
    process.Flag_chargedHadronTrackResolutionFilter,
    process.Flag_muonBadTrackFilter,
    process.Flag_BadChargedCandidateFilter,
    process.Flag_BadPFMuonFilter,
    process.Flag_BadChargedCandidateSummer16Filter,
    process.Flag_BadPFMuonSummer16Filter,
    process.Flag_trkPOG_manystripclus53X,
    process.Flag_trkPOG_toomanystripclus53X,
    process.Flag_trkPOG_logErrorTooManyClusters,
    process.Flag_METFilters,
    process.endjob_step
)
process.schedule.associate(process.patTask)

from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

from FWCore.ParameterSet.Utilities import convertToUnscheduled
process=convertToUnscheduled(process)

from PhysicsTools.PatAlgos.slimming.miniAOD_tools import miniAOD_customizeAllData
process = miniAOD_customizeAllData(process)

from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)

#--------------------------------------------------------------------------------------------------------------------------------

print('Start nanoProcess')

from Configuration.Eras.Era_Run2_2018_cff import Run2_2018
from Configuration.Eras.Modifier_run2_nanoAOD_102Xv1_cff import run2_nanoAOD_102Xv1

nanoProcess = cms.Process('NANO',Run2_2018,run2_nanoAOD_102Xv1)
process.addSubProcess(cms.SubProcess(nanoProcess))

print('Load files for nanoProcess')
nanoProcess.load('Configuration.StandardSequences.Services_cff')
nanoProcess.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
nanoProcess.load('FWCore.MessageService.MessageLogger_cfi')
nanoProcess.load('Configuration.EventContent.EventContent_cff')
nanoProcess.load('Configuration.StandardSequences.GeometryRecoDB_cff')
nanoProcess.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
nanoProcess.load('Configuration.StandardSequences.EndOfProcess_cff')
nanoProcess.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
nanoProcess.load('PhysicsTools.NanoAOD.nano_cff')
#nanoProcess.load('RecoCTPPS.Configuration.recoCTPPS_cff')

# Maybe GT is needed here?
from Configuration.AlCa.GlobalTag import GlobalTag
nanoProcess.GlobalTag = GlobalTag(nanoProcess.GlobalTag, '106X_dataRun2_v11', '')

# Input source
#process.source = cms.Source("PoolSource",
#                            #fileNames = cms.untracked.vstring('file:../test/miniAOD1k.root'),
#                            fileNames = cms.untracked.vstring('file:miniAOD-prod_PAT.root'),
#                            secondaryFileNames = cms.untracked.vstring()
#                        )
nanoProcess.options = cms.untracked.PSet()

# Production Info
nanoProcess.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('myNanoProdData nevts:1'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

print('Output module')
# Output definition
nanoProcess.NANOAODoutput = cms.OutputModule("NanoAODOutputModule",
                                             compressionAlgorithm = cms.untracked.string('LZMA'),
                                             compressionLevel = cms.untracked.int32(9),
                                             dataset = cms.untracked.PSet(
                                                 dataTier = cms.untracked.string('NANOAOD'),
                                                 filterName = cms.untracked.string('')
                                             ),
                                             fileName = cms.untracked.string('myNanoProdData_NANO.root'),
                                             outputCommands = nanoProcess.NANOAODEventContent.outputCommands
                                         )

process.Tracer = cms.Service("Tracer")

nanoProcess.nanoAOD_step = cms.Path(nanoProcess.nanoSequence)
nanoProcess.endjob_step = cms.EndPath(nanoProcess.endOfProcess)
nanoProcess.NANOAODoutput_step = cms.EndPath(nanoProcess.NANOAODoutput)

nanoProcess.schedule = cms.Schedule(
    nanoProcess.nanoAOD_step,
    nanoProcess.endjob_step,
    nanoProcess.NANOAODoutput_step
)

from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(nanoProcess)

## customisation of the process.
from PhysicsTools.NanoAOD.nano_cff import nanoAOD_customizeData 
nanoProcess = nanoAOD_customizeData(nanoProcess)

from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
nanoProcess = customiseEarlyDelete(nanoProcess)
