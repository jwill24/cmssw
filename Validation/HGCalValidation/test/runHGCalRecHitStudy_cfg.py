import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

#from Configuration.Eras.Era_Phase2C4_cff import Phase2C4
#process = cms.Process('PROD',Phase2C4)
#process.load('Configuration.Geometry.GeometryExtended2023D28_cff')
#process.load('Configuration.Geometry.GeometryExtended2023D28Reco_cff')

from Configuration.Eras.Era_Phase2C4_timing_layer_bar_cff import Phase2C4_timing_layer_bar
process = cms.Process('PROD',Phase2C4_timing_layer_bar)
process.load('Configuration.Geometry.GeometryExtended2023D41_cff')
process.load('Configuration.Geometry.GeometryExtended2023D41Reco_cff')

process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')

process.MessageLogger.cerr.FwkReport.reportEvery = 2
#if 'MessageLogger' in process.__dict__:
#    process.MessageLogger.categories.append('HGCalValidation')

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
        'file:step3.root',
#       'root://cms-xrd-global.cern.ch//store/relval/CMSSW_9_1_1_patch1/RelValSingleElectronPt35Extended/GEN-SIM-RECO/91X_upgrade2023_realistic_v1_D17-v1/10000/10D95AC2-B14A-E711-BC4A-0CC47A7C3638.root',
        )
                            )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load('Validation.HGCalValidation.hgcalRecHitStudy_cff')
process.hgcalRecHitStudyBH.verbosity = 0

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('hgcRecHitD41tt.root'),
                                   closeFileFast = cms.untracked.bool(True)
                                   )

SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",ignoreTotal = cms.untracked.int32(1) )

process.p = cms.Path(process.hgcalRecHitStudyEE+process.hgcalRecHitStudyFH+process.hgcalRecHitStudyBH)

