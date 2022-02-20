#ifndef HPSSIM_PREASSIGNEDDECAY_H_
#define HPSSIM_PREASSIGNEDDECAY_H_ 1

#include "G4Decay.hh"
#include "G4DecayProducts.hh"

namespace hpssim {

/**
 * @brief Special handling of HPS-specific predefined decays.
 * 
 */
class PreassignedDecay : public G4Decay {

    public: 
    
        PreassignedDecay();
  
        virtual ~PreassignedDecay();

    public:

        G4VParticleChange* PostStepDoIt(const G4Track& aTrack, const G4Step&  aStep);

    protected:

        G4VParticleChange* DecayIt(const G4Track& aTrack, const G4Step& aStep);
};

}

#endif