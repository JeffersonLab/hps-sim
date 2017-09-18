#ifndef HPSSIM_LCIOPERSISTENCYMANAGER_H_
#define HPSSIM_LCIOPERSISTENCYMANAGER_H_

#include "lcdd/core/LCDDProcessor.hh"
#include "lcdd/hits/CalorimeterHit.hh"
#include "lcdd/hits/TrackerHit.hh"

#include "G4PersistencyManager.hh"
#include "G4PersistencyCenter.hh"
#include "G4Run.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"

#include "IO/LCWriter.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/SimTrackerHitImpl.h"
#include "EVENT/LCIO.h"
#include "IOIMPL/LCFactory.h"

#include "LcioPersistencyMessenger.h"
#include "MCParticleBuilder.h"

using IMPL::LCCollectionVec;
using EVENT::LCIO;

namespace hpssim {

class LcioPersistencyManager : public G4PersistencyManager {

    public:

        LcioPersistencyManager() :
                G4PersistencyManager(G4PersistencyCenter::GetPersistencyCenter(), "LcioPersistencyManager") {
            G4PersistencyCenter::GetPersistencyCenter()->RegisterPersistencyManager(this);
            G4PersistencyCenter::GetPersistencyCenter()->SetPersistencyManager(this, "LcioPersistencyManager");
            writer_ = nullptr;
            builder_ = new MCParticleBuilder();
            messenger_ = new LcioPersistencyMessenger(this);
        }

        virtual ~LcioPersistencyManager() {
            if (writer_) {
                delete writer_;
            }
            delete messenger_;
        }

        static LcioPersistencyManager* getInstance() {
            return (LcioPersistencyManager*) G4PersistencyCenter::GetPersistencyCenter()->CurrentPersistencyManager();
        }

        G4bool Store(const G4Event* anEvent) {
            if (!anEvent->IsAborted()) {

                if (m_verbose > 1) {
                    std::cout << "LcioPersistencyManager: Storing event " << anEvent->GetEventID() << std::endl;
                }

                // create new LCIO event
                IMPL::LCEventImpl* lcioEvent = new IMPL::LCEventImpl();
                lcioEvent->setEventNumber(anEvent->GetEventID());
                lcioEvent->setRunNumber(G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID());
                lcioEvent->setDetectorName(LCDDProcessor::instance()->getDetectorName());

                // write MCParticles to LCIO event
                lcioEvent->addCollection(builder_->buildMCParticleColl(anEvent), EVENT::LCIO::MCPARTICLE);

                // write hits collections to LCIO event
                writeHitsCollections(anEvent, lcioEvent);

                // write event and flush writer
                writer_->writeEvent(static_cast<EVENT::LCEvent*>(lcioEvent));
                writer_->flush();

                // delete the event object to avoid memory leak
                delete lcioEvent;

                return true;

            } else {
                if (m_verbose > 1) {
                    std::cout << "LcioPersistencyManager: Skipping aborted event " << anEvent->GetEventID() << std::endl;
                }
                return false;
            }
        }

        G4bool Store(const G4Run* aRun) {
            std::cout << "LcioPersistencyManager: store run " << aRun->GetRunID() << std::endl;

            writer_->close();

            return true;
        }

        G4bool Store(const G4VPhysicalVolume*) {
            return false;
        }

        // executed manually at start of run
        void Initialize() {

            std::cout << "LcioPersistencyManager: Initializing" << std::endl;

            writer_ = IOIMPL::LCFactory::getInstance()->createLCWriter();
            std::cout << "LcioPersistencyManager: Opening '" << outputFile_ << "' for writing" << std::endl;
            writer_->open(outputFile_);

            auto runHeader = new IMPL::LCRunHeaderImpl();
            runHeader->setDetectorName(LCDDProcessor::instance()->getDetectorName());
            runHeader->setRunNumber(G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID());
            runHeader->setDescription("HPS MC events");

            writer_->writeRunHeader(static_cast<EVENT::LCRunHeader*>(runHeader));
        }

        void setOutputFile(std::string outputFile) {
            outputFile_ = outputFile;
        }

    private:

        void writeHitsCollections(const G4Event* g4Event, IMPL::LCEventImpl* lcioEvent) {

            G4HCofThisEvent* hce = g4Event->GetHCofThisEvent();
            int nColl = hce->GetNumberOfCollections();

            for (int iColl = 0; iColl < nColl; iColl++) {

                G4VHitsCollection* hc = hce->GetHC(iColl);
                std::string collName = hc->GetName();
                LCCollectionVec* collVec = nullptr;

                if (dynamic_cast<TrackerHitsCollection*>(hc)) {
                    collVec = writeTrackerHitsCollection(hc, collName);
                } else if (dynamic_cast<CalorimeterHitsCollection*>(hc)) {
                    collVec = writeCalorimeterHitsCollection(hc, collName);
                }

                if (collVec) {
                    lcioEvent->addCollection(collVec, collName);
                    if (m_verbose > 1) {
                        std::cout << "LcioPersistencyManager: Stored " << collVec->size() << " hits in " << collName << std::endl;
                    }
                }
            }

        }

        IMPL::LCCollectionVec* writeTrackerHitsCollection(G4VHitsCollection* hc, const std::string& collName) {
            auto trackerHits = dynamic_cast<TrackerHitsCollection*>(hc);
            auto collVec = new LCCollectionVec(LCIO::SIMTRACKERHIT);
            IMPL::LCFlagImpl collFlag;
            collFlag.setBit(EVENT::LCIO::THBIT_MOMENTUM);
            collVec->setFlag(collFlag.getFlag());

            int nhits = trackerHits->GetSize();
            if (m_verbose > 2) {
                std::cout << "LcioPersistencyManager: Converting " << nhits << " tracker hits to LCIO" << std::endl;
            }
            for (int i = 0; i < nhits; i++) {

                auto trackerHit = static_cast<TrackerHit*>(trackerHits->GetHit(i));
                auto simTrackerHit = new IMPL::SimTrackerHitImpl();

                // position in mm
                const G4ThreeVector posVec = trackerHit->getPosition();
                double pos[3] = { posVec.x(), posVec.y(), posVec.z() };
                simTrackerHit->setPosition(pos);

                // momentum in GeV
                const G4ThreeVector& momentum = trackerHit->getMomentum();
                simTrackerHit->setMomentum(momentum.x() / GeV, momentum.y() / GeV, momentum.z() / GeV);

                // pathLength = distance between exit and entry points in mm
                simTrackerHit->setPathLength(trackerHit->getLength());

                // dEdx in GeV (LCIO units)
                float edep = trackerHit->getEdep();
                simTrackerHit->setEDep(edep / GeV);

                // time in NS
                float tEdep = trackerHit->getTdep();
                simTrackerHit->setTime(tEdep);

                // Cell ID.
                simTrackerHit->setCellID0(trackerHit->getId());
                collVec->push_back(simTrackerHit);

                // get the MCParticle for the hit
                if (m_verbose > 3) {
                    std::cout << "LcioPersistencyManager: Looking for track ID " << trackerHit->getTrackID()
                            << std::endl;
                }
                IMPL::MCParticleImpl* mcp = builder_->findMCParticle(trackerHit->getTrackID());
                if (!mcp) {
                    std::cerr << "LcioPersistencyManager: No MCParticle found for trackID " << trackerHit->getTrackID()
                            << " from sim tracker hit" << std::endl;
                    G4Exception("LcioPersistencyManager::writeHitsCollections", "", FatalException,
                            "MCParticle for track ID is missing.");
                } else {
                    simTrackerHit->setMCParticle(mcp);
                }
            }
            return collVec;
        }

        IMPL::LCCollectionVec* writeCalorimeterHitsCollection(G4VHitsCollection* hc, const std::string& collName) {

            auto calHits = dynamic_cast<CalorimeterHitsCollection*>(hc);
            auto collVec = new LCCollectionVec(LCIO::SIMCALORIMETERHIT);
            IMPL::LCFlagImpl collFlag;
            collFlag.setBit(EVENT::LCIO::CHBIT_LONG);
            collFlag.setBit(EVENT::LCIO::CHBIT_PDG);
            collVec->setFlag(collFlag.getFlag());

            int nhits = calHits->GetSize();
            if (m_verbose > 2) {
                std::cout << "LcioPersistencyManager: Converting " << nhits << " cal hits to LCIO" << std::endl;
            }
            for (int i = 0; i < nhits; i++) {

                auto calHit = static_cast<CalorimeterHit*>(calHits->GetHit(i));
                auto simCalHit = new IMPL::SimCalorimeterHitImpl();

                // set cellid from cal hit's id64
                const Id64bit& id64 = calHit->getId64bit();
                simCalHit->setCellID0(id64.getId0());
                simCalHit->setCellID1(id64.getId1());

                // position
                const G4ThreeVector hitPos = calHit->getPosition();
                float pos[3] = {(float)hitPos.x(), (float)hitPos.y(), (float)hitPos.z()};
                simCalHit->setPosition(pos);

                // energy
                simCalHit->setEnergy(calHit->getEdep());

                // add to output collection
                collVec->push_back(simCalHit);

                auto contribs = calHit->getHitContributions();
                for (auto contrib : contribs) {
                    auto edep = contrib.getEdep();
                    auto hitTime = contrib.getGlobalTime();
                    auto pdg = contrib.getPDGID();
                    auto contribPos = contrib.getPosition();
                    auto trackID = contrib.getTrackID();

                    if (trackID <= 0) {
                        std::cerr << "LcioPersistencyManager: Bad track ID " << trackID << " for cal hit contrib" << std::endl;
                        G4Exception("LcioPersistencyManager::writeHitsCollections", "",
                                FatalException, "Bad track ID in cal hit contribution.");
                    }

                    auto mcp = builder_->findMCParticle(trackID);

                    if (!mcp) {
                        std::cerr << "LcioPersistencyManager: No MCParticle found for track ID " << trackID << std::endl;
                        G4Exception("LcioPersistencyManager::writeHitsCollections", "",
                                FatalException, "No MCParticle found for track ID.");
                    }

                    simCalHit->addMCParticleContribution(static_cast<EVENT::MCParticle*>(mcp), (float)edep, (float)hitTime, (int)pdg, (float*)contribPos);

                    if (m_verbose > 3) {
                        std::cout << "LcioPersistencyManager: Assigned cal hit contrib with "
                                << "trackID = " << trackID << "; "
                                << "edep = " << edep << "; "
                                << "time = " << hitTime << "; "
                                << "pdg = " << pdg << "; "
                                << "pos = ( " << contribPos[0] << ", " << contribPos[1] << ", " << contribPos[2] << " ) "
                                << std::endl;
                    }
                }
            }
            return collVec;
        }

    private:

        std::string outputFile_{"hps_sim_events.slcio"};
        IO::LCWriter* writer_;
        MCParticleBuilder* builder_;
        LcioPersistencyMessenger* messenger_;

};


}

#endif