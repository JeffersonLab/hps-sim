#ifndef HPSSIM_UNKNOWNDECAYPHYSICS_H_
#define HPSSIM_UNKNOWNDECAYPHYSICS_H_ 1

#include "G4VPhysicsConstructor.hh"
#include "G4Decay.hh"
#include "G4ParticleTable.hh"
#include "G4UnknownDecay.hh"
#include "G4UnknownParticle.hh"

namespace hpssim {

class UnknownDecayPhysics : public G4VPhysicsConstructor {

    public:

        UnknownDecayPhysics(const G4String& name = "decay") :
            G4VPhysicsConstructor(name) {
        }

        virtual ~UnknownDecayPhysics() {
        }

        void ConstructParticle() {
            G4UnknownParticle::UnknownParticleDefinition();
          
            new G4ParticleDefinition("hps-622", 
                    0, // mass
                    0, // width
                    0, // charge
                    0, // 2*spin
                    0, // parity
                    0, // C-conjugation
                    0, // 2*isospin
                    0, // 2*isospin3
                    0, // G-parity
                    "extended", // type
                    0, // lepton number
                    0, // baryon number
                    622, // PDG encoding
                    false, // stable
                    DBL_MIN, // lifetime (can be overridden by predefined decay time)
                    0, // decay table
                    false // short lived
                    );

            new G4ParticleDefinition("hps-623", 
                    0, // mass
                    0, // width
                    0, // charge
                    0, // 2*spin
                    0, // parity
                    0, // C-conjugation
                    0, // 2*isospin
                    0, // 2*isospin3
                    0, // G-parity
                    "extended", // type
                    0, // lepton number
                    0, // baryon number
                    623, // PDG encoding
                    false, // stable
                    DBL_MIN, // lifetime (can be overridden by predefined decay time)
                    0, // decay table
                    false // short lived
                    );
        }       

        void addDecayProcess(G4ParticleDefinition* def) {
            if (def) {
                G4ProcessManager* procMgr = def->GetProcessManager();
                procMgr->AddProcess(&fDecayProcess);
                procMgr->SetProcessOrdering(&fDecayProcess, idxPostStep);
            }
        }

        void ConstructProcess() {
            G4ParticleTable* tbl = G4ParticleTable::GetParticleTable();
            G4ParticleDefinition* def = tbl->FindParticle("unknown");
            if (def) {
                G4ProcessManager* procMgr = def->GetProcessManager();
                procMgr->AddProcess(&fUnknownDecay);
                procMgr->SetProcessOrdering(&fUnknownDecay, idxPostStep);
            }

            addDecayProcess(tbl->FindParticle("622"));
            addDecayProcess(tbl->FindParticle("623"));
            
/*
            aParticleIterator->reset();
            while( (*aParticleIterator)() ){
                G4ParticleDefinition* particle = aParticleIterator->value();
                G4ProcessManager* pmanager = particle->GetProcessManager();
                if(particle->GetParticleName()=="unknown") {
                    pmanager ->AddProcess(&fUnknownDecay);
                    pmanager ->SetProcessOrdering(&fUnknownDecay, idxPostStep);
                }
            }
*/
        }

    protected:
        G4Decay fDecayProcess;
        G4UnknownDecay fUnknownDecay;
};

}

#endif
