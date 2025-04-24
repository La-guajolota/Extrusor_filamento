/****************************************************************************************
 * File: PID.c
 * Description: Implementation of a PID controller for embedded systems, used
 *              in regulating temperature systems or physical variables.
 *              This file contains the functions that manage PID calculations,
 *              with support for dynamic gain adjustments and controller
 *              output limitation.
 *
 * Author: Adrian Silva Palafox
 * Creation date: November 2024
 *
 * License: This code is open source under the license [Your License Here].
 *          It can be modified and distributed for educational or commercial purposes.
 *
 * Note: It is assumed that the necessary libraries such as "pid.h" and other
 *       dependencies are already correctly included and configured in the
 *       development environment.
 ***************************************************************************************/

// Include the header file for the PID implementation
#include "pid.h"

// Function to initialize the PID controller
void PID_Init(PIDController* pid, float kp, float ki, float kd,
              float tau,
              float limMin, float limMax,
              float limMinInt, float limMaxInt,
              float t)
{
    // Initialize PID gains (proportional, integral, derivative)
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;

    // Low-pass filter time constant (for the derivative)
    pid->tau = tau;

    // Controller output limits
    pid->limMin = limMin;
    pid->limMax = limMax;

    // Integrator limits (wind-up prevention)
    pid->limMinInt = limMinInt;
    pid->limMaxInt = limMaxInt;

    // Sampling time (time interval between updates)
    pid->T = t;

    // Initialize integrator (accumulates error)
    pid->integrator = 0.0f;

    // Initialize previous error (for derivative calculation)
    pid->prevError = 0.0f;

    // Initialize differentiator (for derivative term)
    pid->differentiator = 0.0f;

    // Initialize previous measurement (for derivative calculation)
    pid->prevMeasurement = 0.0f;

    // Initialize controller output
    pid->out = 0.0f;
}

// Function to reset the PID controller
void PID_Reset(PIDController* pid)
{
    pid->integrator = 0.0f;      // Reset integrator
    pid->prevError = 0.0f;       // Reset previous error
    pid->differentiator = 0.0f;  // Reset differentiator
    pid->prevMeasurement = 0.0f; // Reset previous measurement
    pid->out = 0.0f;             // Reset output
}

// Function that updates the PID controller each time it is called
float PID_Update(PIDController* pid, float setpoint, float measurement)
{
    // Calculate error (difference between setpoint and measurement)
    float error = setpoint - measurement;

    // Calculate proportional term (Kp * error)
    float proportional = pid->Kp * error;

    // Calculate integral term (integration of error)
    pid->integrator += 0.5f * pid->Ki * pid->T * (error + pid->prevError); // Trapezoidal integration method (average sum of errors)

    // Anti-windup: Limit the integrator value to prevent excessive error accumulation
    if (pid->integrator > pid->limMaxInt)
    {
        pid->integrator = pid->limMaxInt; // Limit integrator to maximum allowed value
    }
    else if (pid->integrator < pid->limMinInt)
    {
        pid->integrator = pid->limMinInt; // Limit integrator to minimum allowed value
    }

    // Calculate derivative term (with low-pass filter to avoid noise)
    pid->differentiator = -(2.0f * pid->Kd * (measurement - pid->prevMeasurement) +
                         (2.0f * pid->tau - pid->T) * pid->differentiator) /
                         (2.0f * pid->tau + pid->T);

    // Calculate total controller output (Sum of proportional, integral, and derivative)
    pid->out = proportional + pid->integrator + pid->differentiator;

    // Apply limits to the output (controller output must be within defined limits)
    if (pid->out > pid->limMax)
    {
        pid->out = pid->limMax; // Limit output to maximum
    }
    else if (pid->out < pid->limMin)
    {
        pid->out = pid->limMin; // Limit output to minimum
    }

    // Update controller memory (errors and previous measurements for the next cycle)
    pid->prevError = error;
    pid->prevMeasurement = measurement;

    // Return controller output (how much the controlled variable should be adjusted)
    return pid->out;
}

// Function to update Kp, Ki, and Kd gains at runtime
void PID_UpdateGains(PIDController* pid, float kp, float ki, float kd)
{
    pid->Kp = kp; // Update proportional gain
    pid->Ki = ki; // Update integral gain
    pid->Kd = kd; // Update derivative gain
}

// Functions to get Kp, Ki, and Kd gains individually
float PID_GetKp(const PIDController* pid)
{
    return pid->Kp; // Return proportional gain
}

float PID_GetKi(const PIDController* pid)
{
    return pid->Ki; // Return integral gain
}

float PID_GetKd(const PIDController* pid)
{
    return pid->Kd; // Return derivative gain
}

// Function to get all gains together as a struct (useful for sending via serial or other methods)
PIDGains PID_GetGains(const PIDController* pid)
{
    PIDGains gains;
    gains.Kp = pid->Kp;
    gains.Ki = pid->Ki;
    gains.Kd = pid->Kd;
    return gains; // Return all three gains as a struct
}
