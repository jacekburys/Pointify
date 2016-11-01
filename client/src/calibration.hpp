#ifndef CALIBRATION_H 
#define CALIBRATION_H 

class Calibration 
{
private:
    // amount of markers in the x and y directions on the grid
    const int MARKER_X = 1;
    const int MARKER_Y = 1;
    
    // size of marker in meters(?)
    const int MARKER_LENGTH = 1;
    
    // distance between markers
    const int MARKER_SEPARATION = 1;
public:
    Calibration();
    void calibrate();
};

#endif
