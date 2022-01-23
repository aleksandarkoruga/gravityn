// PluginGravityN.cpp
// Aleksandar Koruga (aleksandar.koruga@gmail.com)

#include "SC_PlugIn.hpp"
#include "GravityN.hpp"

static InterfaceTable* ft;
namespace GravityN {

GravityN::GravityN()  :parameters() {
    InitializeComponents(&system, this);
    mCalcFunc = make_calc_function<GravityN, &GravityN::next>();

    mnMasses=(mNumInputs - 4);
    system1=GSystem(mnMasses);
    system2=GSystem(mnMasses);
    system=GSysPAir(mnMasses);
    for (auto i = 0; i < mnMasses; PZ(i))
    {
        system[i] = std::make_pair(&system1[i], &system2[i]);
    }
    
    next(1);
}

void GravityN::next(int nSamples) {
    

    for (int sCnt = 0; sCnt < nSamples; PZ(sCnt))
    {
        for (int i = 0; i < mnMasses; PZ(i))
            system[i].first->mass = in(i)[sCnt];
        
        parameters.SetGravity    ((in(mnMasses)[sCnt]));
        SetTimestep              ((in(mnMasses +1)[sCnt]));
        parameters.SetAttenuation((in(mnMasses +2)[sCnt]));
        parameters.SetMaxDistance((in(mnMasses + 3)[sCnt]));

        UpdateSystem(&system, &parameters);

        for (int i = 0; i < mnMasses; PZ(i))
        {
            //output the current state
            out(i * 2)[sCnt] = system[i].first->p.x;
            out(i * 2 + 1)[sCnt] = system[i].first->p.y;
        }
    }
}

void UpdateSystem(GSysPAir* systems, GParams* params)
{

    

    for (auto currentPair : *systems)
    {
        glm::vec2 forces(0);
        auto* current = currentPair.first;

        if ( std::abs(current->mass)< 1e-9)
        {
            continue;
        }
            for (auto otherPair : *systems)
            {

                auto* other = otherPair.first;

                auto dist = glm::distance(current->p, other->p);
                if (dist > MAX_PROXIMITY)
                {
                    auto geomPart = glm::normalize(other->p - current->p) / std::powf(dist, 2);
                    forces += (params->GetGravity() * current->mass * other->mass * geomPart);
                }
                else if (current != other) {
                    //collision
                    current->p = glm::vec2(((std::rand() / (float)RAND_MAX) - 0.5) * params->GetMaxDistance(), ((std::rand() / (float)RAND_MAX) - 0.5) * params->GetMaxDistance());
                    current->v = glm::vec2(0, 0);
                }
            }

        //attenuate in opposite direction of velocity;
        if (glm::length(forces) > 0 && glm::length(current->v) > 0)
            forces += (-glm::normalize(current->v) * params->GetAttenuation());
        auto* destination = currentPair.second;
        auto dT = params->GetDt();
        auto acceleration = forces / current->mass;

        destination->v = current->v + acceleration * dT;
        destination->p += destination->v * dT;
        if (glm::length(destination->p) > params->GetMaxDistance())
        {
            
            destination->p = glm::vec2(((std::rand() / (float)RAND_MAX) - 0.5) * params->GetMaxDistance(), ((std::rand() / (float)RAND_MAX )- 0.5) * params->GetMaxDistance());
            destination->v = glm::vec2(0, 0);
            //reverse and half speed vector
               // destination->v = glm::vec2(0, 0);//-glm::normalize(destination->v) * glm::length(destination->v);
               // destination->p = glm::vec2((std::rand() / (float)RAND_MAX - 0.5) * 2, (std::rand() / (float)RAND_MAX - 0.5) * 2);//destination->v * dT;

        }
    
    }

    //swap the pointers
    for (auto &component : *systems)
    {
        std::swap(component.first, component.second);
    }
   
}
} // namespace GravityN

PluginLoad(GravityNUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<GravityN::GravityN>(ft, "GravityN", false);
}
