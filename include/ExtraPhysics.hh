#ifndef HPSSIM_EXTRAPHYSICS_H_
#define HPSSIM_EXTRAPHYSICS_H_ 1

#include "G4VPhysicsConstructor.hh"
#include "G4ParticleTable.hh"
#include "G4UnknownDecay.hh"
#include "G4UnknownParticle.hh"
#include "G4ProcessManager.hh"
#include "G4hMultipleScattering.hh"
#include "G4Decay.hh"
#include "G4DecayProducts.hh"

namespace hpssim {

#ifndef aParticleIterator
#define aParticleIterator ((subInstanceManager.offset[g4vpcInstanceID])._aParticleIterator)
#endif

/**
 * @brief Wrapper for unknown decay for HPS-specific PDGIDs
 * 
 */
class ExtraDecay: public G4UnknownDecay {
    public:

        /**
         * Class constructor.
         */
        ExtraDecay() :
                G4UnknownDecay("ExtraDecay") {
            // DEBUG
            SetVerboseLevel(2);
        }

        G4double GetMeanFreePath(const G4Track& aTrack, G4double, G4ForceCondition* condition) {
            std::cout << "[ ExtraDecay ] GetMeanFreePath for " << aTrack.GetTrackID() << std::endl; 
            *condition = StronglyForced;
            return 0;
        }

        /**
         * Determine if the extended decay process is applicable to the given particle.
         * @param[in] pdef The particle definition.
         */
        G4bool IsApplicable(const G4ParticleDefinition &pdef) {
            if (pdef.GetParticleType() == "extra") {
                std::cout << "[ ExtraDecay ] Process is applicable to " << pdef.GetPDGEncoding() << std::endl;
            } else {
                std::cout << "[ ExtraDecay ] Process NOT applicable to " << pdef.GetPDGEncoding() << std::endl;
            }
            return (pdef.GetParticleType() == "extra");
        }
        
        G4VParticleChange* DecayIt(const G4Track &aTrack, const G4Step &aStep) {
            std::cout << "[ ExtraDecay ] DecayIt for track " << aTrack.GetTrackID() << std::endl;
            const G4DecayProducts* o_products = (aTrack.GetDynamicParticle()->GetPreAssignedDecayProducts());
            bool isPreAssigned = (o_products != 0);
            if (isPreAssigned) {
                std::cout << "    Pre-assigned decay products: " << o_products->entries() << std::endl; 
            } else {
                std::cout << "    No pre-assigned decay products" << std::endl;
            }
            G4UnknownDecay::DecayIt(aTrack, aStep);
        }

        
        virtual G4VParticleChange* AtRestDoIt(const G4Track& track, const G4Step& stepData) {
            std::cout << "[ ExtraDecay ] AtRestDoIt is forcing decay for " << track.GetTrackID() << std::endl;
            this->DecayIt(track, stepData);
        }
};

/**
 * @brief Add HPS-specific particles and processes
 * 
 */
class ExtraPhysics : public G4VPhysicsConstructor {

    public:

        ExtraPhysics(const G4String& name = "extra") :
            G4VPhysicsConstructor(name) {

                this->decayProcess_.SetVerboseLevel(3);
        }

        virtual ~ExtraPhysics() {
        }

        void ConstructParticle() {
          
            std::cout << "[ ExtraPhysics ] ConstructParticle" << std::endl;

            //G4UnknownParticle::Definition();

            new G4ParticleDefinition("hps_622", 
                    100*GeV, // mass
                    0, // width
                    0, // charge
                    0, // 2*spin
                    0, // parity
                    0, // C-conjugation
                    0, // 2*isospin
                    0, // 2*isospin3
                    0, // G-parity
                    "extra", // type
                    0, // lepton number
                    0, // baryon number
                    622, // PDG encoding
                    false, // stable
                    DBL_MIN, // lifetime (can be overridden by predefined decay time)
                    nullptr, // decay table
                    false // short lived
                    );

            new G4ParticleDefinition("hps_623", 
                    100*GeV, // mass
                    0, // width
                    0, // charge
                    0, // 2*spin
                    0, // parity
                    0, // C-conjugation
                    0, // 2*isospin
                    0, // 2*isospin3
                    0, // G-parity
                    "extra", // type
                    0, // lepton number
                    0, // baryon number
                    623, // PDG encoding
                    false, // stable
                    DBL_MIN, // lifetime (can be overridden by predefined decay time)
                    nullptr, // decay table
                    false // short lived
                    );
        }       
        
        void ConstructProcess() {
            
            std::cout << "[ ExtraPhysics ] ConstructProcess" << std::endl;

            // Unknown decay
            /*
            G4ParticleTable* tbl = G4ParticleTable::GetParticleTable();
            G4ParticleDefinition* def = tbl->FindParticle("unknown");
            if (def) {
                G4ProcessManager* procMgr = def->GetProcessManager();
                procMgr->AddProcess(&unknownDecay_);
                procMgr->SetProcessOrdering(&unknownDecay_, idxPostStep);
            }
            */

            // Extra decay for HPS-specific PDGIDs
            aParticleIterator->reset();
            while( (*aParticleIterator)() ){
                G4ParticleDefinition* particle = aParticleIterator->value();
                G4ProcessManager* pmanager = particle->GetProcessManager();
                if (particle->GetParticleType() == "extra") {
                    std::cout << "[ ExtraPhysics ] Adding processes for " << particle->GetPDGEncoding() << std::endl;
                    pmanager->AddProcess(&msProcess_, -1, 1, 1); // multiple scattering
                    //pmanager->AddProcess(&decayProcess_, -1, -1, 2);
                    pmanager->AddProcess(&decayProcess_, -1, -1, 2);
                }
            }

        }

    protected:
        G4Decay decayProcess_;
        //G4UnknownDecay unknownDecay_;
        ExtraDecay extraDecayProcess_;
        G4hMultipleScattering msProcess_;       
};

}

#endif
