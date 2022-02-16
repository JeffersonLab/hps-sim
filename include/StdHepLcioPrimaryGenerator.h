#ifndef HPSSIM_STDHEPLCIOPRIMARYGENERATOR_H_
#define HPSSIM_STDHEPLCIOPRIMARYGENERATOR_H_

#include "G4RunManager.hh"
#include "G4VPrimaryGenerator.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

// LCIO
#include "UTIL/LCStdHepRdrNew.h"
#include "EVENT/MCParticle.h"
#include "IMPL/LCCollectionVec.h"

// hps-sim
#include "LcioPrimaryGenerator.h"

using UTIL::LCStdHepRdrNew;
using EVENT::MCParticle;
using EVENT::LCCollection;
using IMPL::LCCollectionVec;

namespace hpssim {

/**
 * @class StdHepLcioPrimaryGenerator
 * @brief Generates a Geant4 event from StdHep data
 */
class StdHepLcioPrimaryGenerator : public PrimaryGenerator {

    public:

        StdHepLcioPrimaryGenerator(std::string name) : PrimaryGenerator(name) {
        }

        virtual ~StdHepLcioPrimaryGenerator() {
            if (reader_ != nullptr) {
                delete reader_;            
            }
        }

        void GeneratePrimaryVertex(G4Event* anEvent) {
            LcioPrimaryGenerator::generateEvent(particles_, anEvent);
        }

        bool isFileBased() {
            return true;
        }

        bool supportsRandomAccess() {
            return false;
        }

        //int getNumEvents() {
        //    return records_.size();
        //}

        void readNextEvent() throw(EndOfFileException) {
            particles_ = reader_->readEvent();
        }

        void openFile(std::string file) {
            if (reader_ != nullptr) {
                delete reader_;
                reader_ = nullptr;
            }
            reader_ = new LCStdHepRdrNew(file.c_str());
        }

        //void readEvent(long index, bool removeEvent) throw(NoSuchRecordException) {
        //}

    private:

        LCStdHepRdrNew* reader_;
        LCCollectionVec* particles_;
};

}

#endif
