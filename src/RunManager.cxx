#include "RunManager.h"

#include "UnknownDecayPhysics.h"

#include "G4StateManager.hh"
#include "FTFP_BERT.hh"

namespace hpssim {

void RunManager::InitializePhysics() {

    // For now, do not do anything here until "state" issues are sorted out.

    //G4StateManager* pStateManager = G4StateManager::GetStateManager();
    //G4cout << "state: " << pStateManager->GetStateString(pStateManager->GetCurrentState());
    //pStateManager->SetNewState(G4State_PreInit);

    //G4VUserPhysicsList* thePhysicsList = new FTFP_BERT;
    //G4VModularPhysicsList* modularPhysicsList = dynamic_cast<G4VModularPhysicsList*>(physicsList);
    //modularPhysicsList->RegisterPhysics(new UnknownDecayPhysics);

    G4RunManager::InitializePhysics();
}

}
