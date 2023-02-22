// PluginGravityN.cpp
// Aleksandar Koruga (aleksandar.koruga@gmail.com)

#include "SC_PlugIn.hpp"
#include "GravityN.hpp"

static InterfaceTable* ft;
namespace GravityN {

GravityN::GravityN()  :parameters() {
    mCalcFunc = make_calc_function<GravityN, &GravityN::next>();

    mnMasses=(mNumInputs - 4);
    system1=GSystem(mnMasses);
    system2=GSystem(mnMasses);
    mg_system=GSysPAir(mnMasses);
    for (auto i = 0; i < mnMasses; PZ(i))
    {
        mg_system[i] = std::make_pair(&system1[i], &system2[i]);
    }
    InitializeComponents(&mg_system, this, 1000.0);

    
}

void GravityN::next(int nSamples) {
    

    for (int sCnt = 0; sCnt < nSamples; PZ(sCnt))
    {
        for (int i = 0; i < mnMasses; PZ(i))
        {
            mg_system[i].first->mass = in(i)[sCnt];
            mg_system[i].second->mass = in(i)[sCnt];
        }

        parameters.SetGravity    ((in(mnMasses)[sCnt]));
        SetTimestep              ((in(mnMasses +1)[sCnt]));
        parameters.SetAttenuation((in(mnMasses +2)[sCnt]));
        parameters.SetMaxDistance((in(mnMasses + 3)[sCnt]));

        UpdateSystem(&mg_system, &parameters);


        for (int i = 0; i < mnMasses; PZ(i))
        {
            //output the current state
            out(i * 2)[sCnt] = mg_system[i].first->p.x;
            out(i * 2 + 1)[sCnt] = mg_system[i].first->p.y;
        }


    }
}

void UpdateSystem(GSysPAir* systems, GParams* params)
{

    for (auto currentPair : *systems)
    {
        glm::vec2 forces(0);
        auto* current = currentPair.first;
    //    if ( std::abs(current->mass)< 1e-9)
    //    {
    //        continue;
    //    }
            for (auto otherPair : *systems)
            {
                if (otherPair == currentPair)
                    continue;

                auto* other = otherPair.first;


                auto dist = glm::distance(current->p, other->p);
                auto geomPart = glm::normalize(other->p - current->p); /// std::powf(dist, 3);

                if (dist > MAX_PROXIMITY)
                {
                   // geomPart.x = zapgremlins(geomPart.x);
                   // geomPart.y = zapgremlins(geomPart.y);
                    forces += ((params->GetGravity() * current->mass * other->mass * geomPart)/((float)( glm::pow(glm::distance(other->p ,current->p),2)+MAX_PROXIMITY)));
                }
                else {
                    //collision
                   // current->p = glm::vec2(((std::rand() / (float)RAND_MAX) - 0.5) * params->GetMaxDistance(), ((std::rand() / (float)RAND_MAX) - 0.5) * params->GetMaxDistance());
                   // current->v = glm::vec2(0, 0);
                   // forces = glm::vec2(0);

                  //  if( glm::length(current->v * other->v)>0 )
                    //    current->v=0.5f* glm::reflect(current->v, other->v);
                    
                    forces -= ((params->GetGravity() * current->mass * other->mass * geomPart) / ((float)(glm::pow(glm::distance(other->p, current->p), 2) + MAX_PROXIMITY)));

                    //else
                    //{

                       // current->p = glm::vec2(((std::rand() / (float)RAND_MAX) - 0.5) * params->GetMaxDistance(), ((std::rand() / (float)RAND_MAX) - 0.5) * params->GetMaxDistance());
                       // current->v = glm::vec2(0, 0);
                       // forces = glm::vec2(0);
                    //}
                }
            }

        //attenuate in opposite direction of velocity;
            if (glm::length(current->v) > 0)
            {
                forces += (-glm::normalize(current->v) * params->GetAttenuation());
                current->v *= std::min(1.0f-1e-6f, 1.0f - params->GetAttenuation());
            }
        auto* destination = currentPair.second;
        auto dT = params->GetDt();
        auto acceleration = glm::vec2(0);
        if(sc_abs(current->mass)>0)
             acceleration =  forces / current->mass;
        else
            acceleration = forces ;
        destination->v = current->v + acceleration * dT;

        destination->v = glm::length(destination->v) > params->GetMaxDistance() * 0.25f ? destination->v * params->GetMaxDistance()*0.25f / glm::length( destination->v) : destination->v;

       // auto magV = glm::length(destination->v);
        //destination->v=  glm::normalize(destination->v)* sc_clip(magV, 0.0, 1.0f/params->GetMaxDistance()/dT);
        destination->p = current->p+ destination->v * dT;
      //  destination->p = glm::mod(destination->p,glm::vec2(params->GetMaxDistance())) ;
        destination->p =glm::vec2(sc_wrap2(destination->p.x,  params->GetMaxDistance()), 
                                  sc_wrap2(destination->p.y,  params->GetMaxDistance()));

        /*if (glm::length(destination->p) > params->GetMaxDistance())
        {

            currentPair.first->p = glm::vec2(((std::rand() / (float)RAND_MAX) - 0.5) * params->GetMaxDistance(), ((std::rand() / (float)RAND_MAX )- 0.5) * params->GetMaxDistance());
            currentPair.second->p = currentPair.first->p;
            
            currentPair.first->v = glm::vec2(0, 0);
            currentPair.second->v = currentPair.first->v;
            //reverse and half speed vector
               // destination->v = glm::vec2(0, 0);//-glm::normalize(destination->v) * glm::length(destination->v);
               // destination->p = glm::vec2((std::rand() / (float)RAND_MAX - 0.5) * 2, (std::rand() / (float)RAND_MAX - 0.5) * 2);//destination->v * dT;

        }*/
    
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
