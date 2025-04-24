/****************************************************************************************
 * File: PID.h
 * Description: This file contains the declaration of the structure and functions to
 *              implement a PID controller for embedded systems. The controller includes
 *              calculations for the proportional, integral, and derivative terms
 *              and has support for dynamic gain updates, as well as protection
 *              against integrator wind-up.
 *
 * Author: Adrian Silva Palafox
 * Creation date: November 2024
 *
 * License: This code is open source under the license [Your License Here].
 *          It can be modified and distributed for educational or commercial purposes.
 *
 * Note: Make sure the necessary libraries are correctly included and
 *       configured in your development environment.
 ***************************************************************************************/

#ifndef PID_H
#define PID_H

// Structure to group the PID controller gains
// This structure is optional and allows returning the three gains (Kp, Ki, Kd) in a single object
typedef struct {
    float Kp; // Proportional gain
    float Ki; // Integral gain
    float Kd; // Derivative gain
} PIDGains;

// Structure for the PID controller
typedef struct {
    // PID parameters
    float Kp; // Proportional gain
    float Ki; // Integral gain
    float Kd; // Derivative gain

    float tau; // Low-pass filter time constant for the derivative term

    // Controller output limits
    float limMin; // Minimum output limit
    float limMax; // Maximum output limit

    // Integrator limits (to prevent wind-up)
    float limMinInt; // Minimum integrator limit
    float limMaxInt; // Maximum integrator limit

    // Sampling time (in seconds)
    float T;

    // Internal memory variables
    float integrator;      // Integral term accumulator
    float prevError;       // Previous iteration error (needed to calculate the derivative term)
    float differentiator;  // Stores the value of the derivative term
    float prevMeasurement; // Previous measurement (needed to calculate the derivative)

    // Controller output (final result after applying the PID formula)
    float out;
} PIDController;

// Function prototypes:

// Initialize the PID controller with the specified parameters
void PID_Init(PIDController* pid, float kp, float ki, float kd,
              float tau,                        // Low-pass filter time constant for the derivative
              float limMin, float limMax,       // Controller output limits
              float limMinInt, float limMaxInt, // Integrator limits to prevent wind-up
              float t);                         // Sampling time (in seconds)

// Reset the PID controller to its initial state (clears integrator, previous error, etc.)
void PID_Reset(PIDController* pid);

// Update the PID controller output based on the setpoint and current measurement
float PID_Update(PIDController* pid, float setpoint, float measurement);

// Update the PID controller gains (Kp, Ki, Kd) in real time
void PID_UpdateGains(PIDController* pid, float kp, float ki, float kd);

// Methods to get the current PID gains:
float PID_GetKp(const PIDController* pid); // Returns the proportional gain (Kp)
float PID_GetKi(const PIDController* pid); // Returns the integral gain (Ki)
float PID_GetKd(const PIDController* pid); // Returns the derivative gain (Kd)

// Function to get all gains together as a struct
// This is useful for sending all gains in a single package, e.g., via serial communication
PIDGains PID_GetGains(const PIDController* pid);

#endif // PID_H
