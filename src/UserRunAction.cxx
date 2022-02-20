#include "UserRunAction.h"

// hps-sim
#include "LcioPersistencyManager.h"
#include "PrimaryGeneratorAction.h"

// Geant4
#include "G4EventManager.hh"
#include "G4ParticleTable.hh"

namespace hpssim {

UserRunAction::UserRunAction() {
}

UserRunAction::~UserRunAction() {
}

void UserRunAction::BeginOfRunAction(const G4Run* aRun) {

    /*
    G4PrimaryTransformer* trans = G4EventManager::GetEventManager()->GetPrimaryTransformer();
    trans->SetUnknnownParticleDefined(true);
    trans->SetVerboseLevel(3);
    */

    // DEBUG: Print particle table
    //G4ParticleTable::GetParticleTable()->DumpTable();

    // init LCIO persistence engine
    LcioPersistencyManager::getInstance()->Initialize();

    // init the primary generators
    PrimaryGeneratorAction::getPrimaryGeneratorAction()->initialize();

    // init sim plugins e.g. read parameter settings into variables, etc.
    PluginManager::getPluginManager()->initializePlugins();

    // activate plugin manager's begin run action
    PluginManager::getPluginManager()->beginRun(aRun);
}

void UserRunAction::EndOfRunAction(const G4Run* aRun) {
    PluginManager::getPluginManager()->endRun(aRun);
}
}
