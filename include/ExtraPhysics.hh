#ifndef HPSSIM_EXTRAPHYSICS_H_
#define HPSSIM_EXTRAPHYSICS_H_ 1

// Geant4
#include "G4VPhysicsConstructor.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4hMultipleScattering.hh"
#include "G4Decay.hh"
#include "G4DecayProducts.hh"
#include "G4ParticleTable.hh"

// hps-sim
#include "PreassignedDecay.h"

namespace hpssim {

#ifndef aParticleIterator
#define aParticleIterator ((subInstanceManager.offset[g4vpcInstanceID])._aParticleIterator)
#endif

/**
 * @brief Adds HPS-specific particles and processes.
 * 
 */
class ExtraPhysics : public G4VPhysicsConstructor {

    public:

        ExtraPhysics(const G4String& name = "extra") :
            G4VPhysicsConstructor(name) {
                this->preassignedDecayProcess_.SetVerboseLevel(3);
        }

        virtual ~ExtraPhysics() {
        }

        void ConstructParticle() {
          
            /*
            G4ParticleTable* ptbl = G4ParticleTable::GetParticleTable();
            if (ptbl->FindParticle(622)) {
                std::cout << ">>> particle 622 already exists!" << std::endl;
            }
            if (ptbl->FindParticle(623)) {
                std::cout << ">>>> particle 623 already exists!" << std::endl;
            }
            */

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
            // Extra decay for HPS-specific PDGIDs
            aParticleIterator->reset();
            while( (*aParticleIterator)() ) {
                G4ParticleDefinition* particle = aParticleIterator->value();
                G4ProcessManager* pmanager = particle->GetProcessManager();
                if (particle->GetParticleType() == "extra") {
                    std::cout << "[ ExtraPhysics ] Adding processes for extra particle " << particle->GetPDGEncoding() << std::endl;
                    pmanager->AddProcess(&msProcess_, -1, 1, 1); // multiple scattering to placate Geant4 tracking
                    /*pmanager->AddProcess(&decayProcess_, -1, -1, 2); // decay to trigger pre-assigned decays*/
                    if (particle->GetPDGEncoding() == 623) {
                        // FIXME: Any particle with possible pre-assigned decays needs to be added here
                        pmanager->AddProcess(&preassignedDecayProcess_, -1, -1, 2); // decay to trigger pre-assigned decays                    
                    }
                    //}
                }
            }

        }

    protected:
        //G4Decay decayProcess_;
        PreassignedDecay preassignedDecayProcess_;
        G4hMultipleScattering msProcess_;       
};

}

#endif
