#include "PreassignedDecay.h"

namespace hpssim {

PreassignedDecay::PreassignedDecay() : G4Decay() {

}
  
PreassignedDecay::~PreassignedDecay() {

}

G4VParticleChange* PreassignedDecay::DecayIt(const G4Track& aTrack, const G4Step& aStep) {
 
    std::cout << "[ PreassignedDecay ] DecayIt handling track " << aTrack.GetTrackID() 
        << " with PDGID " << aTrack.GetParticleDefinition()->GetPDGEncoding() << std::endl;

    const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
    const G4DecayProducts* o_products = (aParticle->GetPreAssignedDecayProducts());
    G4bool isPreAssigned = (o_products != 0);   
    if (isPreAssigned) {

        if (this->verboseLevel > 2) {
            std::cout << "[ PreassignedDecay ] Decaying track " << aTrack.GetTrackID() 
                << " with PDGID " << aTrack.GetParticleDefinition()->GetPDGEncoding() 
                << " and " << o_products->entries() << " decay products" 
                << std::endl;
        }

        // Normal case with presassigned decay.
        return G4Decay::DecayIt(aTrack, aStep);
    } else {
        
        if (this->verboseLevel > 2) {
            std::cout << "[ PreassignedDecay ] Handling decay for track " << aTrack.GetTrackID() 
                << " with PDGID " << aTrack.GetParticleDefinition()->GetPDGEncoding() 
                << " and no decay products" 
                << std::endl;
        }

        // Handle case where a doc type particle has no decay products to avoid warning messages
        // by just killing it. Copied from G4Decay for "Error due to NO Decay Table" case. This is 
        // the same but just silently kills the track without a warning message.
        fParticleChangeForDecay.Initialize(aTrack);
        fParticleChangeForDecay.SetNumberOfSecondaries(0);
        fParticleChangeForDecay.ProposeLocalEnergyDeposit(0.0);
        ClearNumberOfInteractionLengthLeft();
        fParticleChangeForDecay.ProposeTrackStatus(fStopAndKill);
    }
    return &fParticleChangeForDecay;
}

G4VParticleChange* PreassignedDecay::PostStepDoIt(const G4Track& aTrack, const G4Step&  aStep) {
    std::cout << "[ PreassignedDecay ] PostStepDoIt for track " << aTrack.GetTrackID()
        << " with PDG ID " << aTrack.GetParticleDefinition()->GetPDGEncoding() << std::endl;

    if ((aTrack.GetTrackStatus() == fStopButAlive) ||
        (aTrack.GetTrackStatus() == fStopAndKill)) {
        std::cout << "    Just returning" << std::endl;
        fParticleChangeForDecay.Initialize(aTrack);
        return &fParticleChangeForDecay;
    } else {
        std::cout << "    Doing DecayIt" << std::endl;
        return DecayIt(aTrack, aStep);
    }
}

}