#ifndef HPSSIM_USERTRACKINGACTION_H_
#define HPSSIM_USERTRACKINGACTION_H_ 1

/*
 * Geant4
 */
#include "G4UserTrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4RunManager.hh"

/*
 * LCDD
 */
#include "lcdd/core/UserRegionInformation.hh"
#include "lcdd/detectors/CurrentTrackState.hh"

/*
 * HPS
 */
#include "PluginManager.h"
#include "TrackMap.h"
#include "UserPrimaryParticleInformation.h"
#include "UserTrackInformation.h"

namespace hpssim {

/**
 * @class UserTrackingAction
 * @brief Implementation of Geant4 user tracking action
 */
class UserTrackingAction : public G4UserTrackingAction {

    public:

        UserTrackingAction() {
        }

        virtual ~UserTrackingAction() {
        }

        void PreUserTrackingAction(const G4Track* aTrack) {
            //std::cout << "UserTrackingAction: pre tracking - " << aTrack->GetTrackID() << std::endl;
            //std::cout << "  PDGID: " << aTrack->GetParticleDefinition()->GetPDGEncoding() << std::endl;

            /*
            if (aTrack->GetDynamicParticle()->GetParticleDefinition()->GetParticleType() == "extra") {
                std::cout << "[ PreUserTrackingAction ] Forcing extra particle " << 
                    aTrack->GetParticleDefinition()->GetPDGEncoding() << " to 0 kE for decay" << std::endl;          
                (const_cast<G4Track*>(aTrack))->SetKineticEnergy(0.);
            }
            */
 
            int trackID = aTrack->GetTrackID();

            // This is set for LCDD sensitive detectors, which is strange but we don't want to change it right now!
            CurrentTrackState::setCurrentTrackID(trackID);

            if (trackMap_.contains(trackID)) {
                if (trackMap_.hasTrajectory(trackID)) {
                    // This makes sure the tracking manager does not delete the trajectory if it already exists.
                    fpTrackingManager->SetStoreTrajectory(true);
                }
            } else {
                // Process a new track.
                processTrack(aTrack);
            }

            PluginManager::getPluginManager()->preTracking(aTrack);
        }

        void PostUserTrackingAction(const G4Track* aTrack) {
            //std::cout << "UserTrackingAction: post tracking - " << aTrack->GetTrackID() << std::endl;

            // Save extra trajectories on tracks that were flagged for saving during event processing.
            auto info = dynamic_cast<UserTrackInformation*>(aTrack->GetUserInformation());

            // Save tracks with tracker hits.
            // This flag is used by LCDD tracker detectors.
            //if (info->hasTrackerHit()) {
            //    info->setSaveFlag(true);
            //}

            // Store trajectory if info has save flag turned on.
            if (info->getSaveFlag()) {
                if (!trackMap_.hasTrajectory(aTrack->GetTrackID())) {
                    storeTrajectory(aTrack);
                    //std::cout << "UserTrackingAction: Storing extra trajectory for track " << aTrack->GetTrackID() << std::endl;
                }
            }

            // Set end point momentum on the trajectory.
            if (fpTrackingManager->GetStoreTrajectory()) {

                auto traj = dynamic_cast<Trajectory*>(fpTrackingManager->GimmeTrajectory());

                if (traj) {

                    // Set end point momentum from last point if track is being killed.
                    if (aTrack->GetTrackStatus() == G4TrackStatus::fStopAndKill) {
                        traj->setEndPointMomentum(aTrack);
                    }

                    // Pass save flag from track info to the trajectory for persistency engine.
                    bool saveFlag = UserTrackInformation::getUserTrackInformation(aTrack)->getSaveFlag();
                    //std::cout << "UserTrackingAction: Passing save flag " << saveFlag
                    //        << " to trajectory " << aTrack->GetTrackID() << std::endl;
                    traj->setSaveFlag(saveFlag);
                }
            }

            PluginManager::getPluginManager()->postTracking(aTrack);
        }

        void storeTrajectory(const G4Track* aTrack) {

            //std::cout << "UserTrackingAction: Creating new trajectory for " << aTrack->GetTrackID() << std::endl;

            // Create a new trajectory for this track.
            fpTrackingManager->SetStoreTrajectory(true);
            Trajectory* traj = new Trajectory(aTrack);
            fpTrackingManager->SetTrajectory(traj);

            // Map track ID to trajectory.
            trackMap_.addTrajectory(traj);
        }

        void processTrack(const G4Track* aTrack) {

            // Setup the track info object.
            UserTrackInformation* info = nullptr;
            if (!aTrack->GetUserInformation()) {
                info = new UserTrackInformation;
                info->setInitialMomentum(aTrack->GetMomentum());
                const_cast<G4Track*>(aTrack)->SetUserInformation(info);
            }

            /*
             * Trajectory storage is enabled if the region is flagged for storing secondaries
             * (e.g. "tracking region") and particle energy is above threshold or the particle is a primary.
             */
            UserRegionInformation* regionInfo =
                    (UserRegionInformation*) aTrack->GetLogicalVolumeAtVertex()->GetRegion()->GetUserInformation();
            bool isPrimary = (aTrack->GetDynamicParticle()->GetPrimaryParticle() != nullptr);
            // TODO: Make sure GPS primary particles pass this check. ^^^
            bool aboveEnergyThreshold = false;
            bool storeSecondaries = false;
            if (regionInfo) {
                aboveEnergyThreshold = (aTrack->GetKineticEnergy() > regionInfo->getThreshold());
                storeSecondaries = regionInfo->getStoreSecondaries();
            }

            // DEBUG
            /*
            if (isPrimary) {
                std::cout << "UserTrackingAction: Track " << aTrack->GetTrackID() << " is a primary." << std::endl;
            }
            */

            if ((storeSecondaries && aboveEnergyThreshold) || isPrimary) {
                /*
                if (regionInfo && regionInfo->getStoreSecondaries()) {
                    std::cout << "UserTrackingAction: Storing trajectory for " << aTrack->GetTrackID() << " in region "
                            << aTrack->GetLogicalVolumeAtVertex()->GetRegion()->GetName()
                            << std::endl;
                }

                if (isPrimary) {
                    std::cout << "UserTrackingAction: Storing trajectory for primary track " << aTrack->GetTrackID() << std::endl;
                }
                */

                storeTrajectory(aTrack);

                info->setSaveFlag(true);

            } else {
                // Trajectory storage is turned off!
                fpTrackingManager->SetStoreTrajectory(false);

                info->setSaveFlag(false);
            }

            // Save the association between track ID and its parent ID for all tracks in the event.
            trackMap_.addSecondary(aTrack->GetTrackID(), aTrack->GetParentID());
        }

        TrackMap* getTrackMap() {
            return &trackMap_;
        }

        static UserTrackingAction* getUserTrackingAction() {
            return static_cast<UserTrackingAction*>(const_cast<G4UserTrackingAction*>(G4RunManager::GetRunManager()->GetUserTrackingAction()));
        }

        G4TrackingManager* getTrackingManager() {
            return fpTrackingManager;
        }

    private:

        TrackMap trackMap_;
};

}

#endif
