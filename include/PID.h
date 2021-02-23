#pragma once

//
// Lånad kod från internet:
// https://gist.github.com/bradley219/5373998

class PIDImpl;
class PID
{
    public:
        // Kp -  proportional gain
        // Ki -  Integral gain
        // Kd -  derivative gain
        // dt -  loop interval time
        // max - maximum value of manipulated variable
        // min - minimum value of manipulated variable
        PID( float dt, float max, float min, float Kp, float Kd, float Ki );

        // Returns the manipulated variable given a setpoint and current process value
        float calculate( float setpoint, float pv );
        ~PID();

    private:
        PIDImpl *pimpl;
};