#include "Geo_Construct.hpp"
#include "meshes/Geo_Orb.hpp"

#include "program/Topl_Program.hpp"

struct Molecular_Construct : Geo_Construct {
    Molecular_Construct(unsigned short nodeCount){
        _hub = new Geo_Orb();
        _geoActors.push_back(Geo_Actor(_hub));
        for(unsigned n = 0; n < nodeCount; n++){ 
            _orbs.push_back(new Geo_Orb(1.0 / nodeCount));
            _geoActors.push_back(Geo_Actor(_orbs.back()));
        }
    }

private:
    Geo_Orb* _hub;
    std::vector<Geo_Orb*> _orbs;
};

struct Molecular_Demo : public Topl_Program {
    void init() override;
    void loop(double frameTime) override;

    Molecular_Construct construct1 = Molecular_Construct(3);
    Molecular_Construct construct2 = Molecular_Construct(5);
    Molecular_Construct construct3 = Molecular_Construct(8);
    Molecular_Construct construct4 = Molecular_Construct(12);
    Molecular_Construct construct5 = Molecular_Construct(16);
    Molecular_Construct construct6 = Molecular_Construct(20);
private:
    Topl_Scene scene;
};