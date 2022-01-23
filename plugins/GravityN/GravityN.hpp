// PluginGravityN.hpp
// Aleksandar Koruga (aleksandar.koruga@gmail.com)

//
//
#pragma once

#include "SC_PlugIn.hpp"
#include <vector>
#include <utility>
#include "GLM 0.9.9.8/g-truc-glm-bf71a83/glm/glm.hpp"


namespace GravityN {
    
#define MAX_PROXIMITY 1.0f
#define DEFAULT_TIMESTEP 0.0000226757369615f

    struct MassComponent 
    {
        float mass;        
        glm::vec2 p;
        glm::vec2 v;
        MassComponent() : mass(1), p(0), v(0) {}
        inline bool operator==(const MassComponent& rhs) { return glm::distance(this->p, rhs.p) < MAX_PROXIMITY; };
        inline bool operator!=(const MassComponent& rhs) { return !( *this == rhs); };

    };
    
    using GSystem = std::vector<MassComponent> ;
    using GSysPAir = std::vector<std::pair<MassComponent*, MassComponent*>>;

    struct GParams 
    {
    protected: glm::vec4 params;
    public:
        GParams(): params(glm::vec4(1e-6f, DEFAULT_TIMESTEP,1000.0f,0.0f)) {}
        float GetGravity(){ return params.x; };
        float GetDt()       { return params.y; };
        float GetMaxDistance() { return params.z; };
        float GetAttenuation() { return params.w; };

        void SetGravity( float g) {  params.x=g; };
        void SetDt(float dt) {  params.y=dt; };
        void SetMaxDistance(float d) { params.z=d; };
        void SetAttenuation(float a) { params.w = a; };
    };
        
    
    float GetRandom(SCUnit* unit) {
        return unit->mParent->mRGen->frand2();
    }

    void InitializeComponents(GSysPAir* _system, SCUnit* unit)
    {
        for (auto component : *_system) 
        {
            auto x = GetRandom(unit)*100.0f;
            auto y = GetRandom(unit)*100.0f;
            component.first->p.x = x;
            component.second->p.x = x;
            
            component.first->p.y = y;
            component.second->p.y = y;
        }
    };  
    void UpdateSystem(GSysPAir* systems,GParams* params);



class GravityN : public SCUnit {
public:
    GravityN();

    // Destructor
    // ~GravityN();

private:
    // Calc function
    void next(int nSamples);

    // Member variables
protected:
    GSystem system1;
    GSystem system2;
    GSysPAir system;
    int mnMasses;
    // gravity dt distance
    GParams parameters;
    inline void SetTimestep(float dT) { parameters.SetDt( (float)mRate->mSampleDur* dT ); }
};

} // namespace GravityN
