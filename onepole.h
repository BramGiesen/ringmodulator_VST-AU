//written by: Nigel Redmon
//source from: www.earlevel.com/main/2012/12/15/a-one-pole-filter/
//
//  OnePole.h
//

#ifndef OnePole_h
#define OnePole_h

#include <math.h>

class OnePole {
public:
    OnePole() {a0 = 1.0; b1 = 0.0; z1 = 0.0;};
    OnePole(double Fc) {z1 = 0.0; setFc(Fc);};
    ~OnePole();
    void setFc(double Fc);
    float process(float in);
    
protected:
    double a0, b1, z1;
};

inline void OnePole::setFc(double Fc) {
    b1 = exp(-2.0 * M_PI * Fc);
    a0 = 1.0 - b1;
}

inline float OnePole::process(float in) {
    return z1 = in * a0 + z1 * b1;
}

#endif
