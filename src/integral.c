#include "integral.h"

float integrate(float x1, float x2, float y1, float y2) {
    /**
     * Integrate over the given values
     * 
     * @param cur_value Current value
     * @param x1        X value 1
     * @param x2        X value 2
     * @param y1        Y value 1
     * @param y2        Y value 2
     * 
     * @return float    New value
    */
    return (y1+y2) * (x2-x1) * 0.5 * 0.001;
}