/****************************************************************************************
 * @file PID.c
 * @brief Implementation of a PID controller for embedded systems, used
 *        in regulating temperature systems or physical variables.
 *        This file contains the functions that manage PID calculations,
 *        with support for dynamic gain adjustments and controller
 *        output limitation.
 *
 * @attention This code is open source under the license MIT.
 *            It can be modified and distributed for educational or commercial purposes.
 *
 * @note It is assumed that the necessary libraries such as "pid.h" and other
 *       dependencies are already correctly included and configured in the
 *       development environment.
 ***************************************************************************************/

#include "pid.h"

/**
 * @brief Initializes the PID controller with the specified parameters.
 *
 * @param pid Pointer to the PIDController structure to initialize.
 * @param kp Proportional gain.
 * @param ki Integral gain.
 * @param kd Derivative gain.
 * @param tau Low-pass filter time constant for the derivative term.
 * @param limMin Minimum output limit.
 * @param limMax Maximum output limit.
 * @param limMinInt Minimum integrator limit (anti-windup).
 * @param limMaxInt Maximum integrator limit (anti-windup).
 * @param t Sampling time (time interval between updates).
 */
void PID_Init(PIDController *pid, float kp, float ki, float kd,
              float tau,
              float limMin, float limMax,
              float limMinInt, float limMaxInt,
              float t)
{
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->tau = tau;
    pid->limMin = limMin;
    pid->limMax = limMax;
    pid->limMinInt = limMinInt;
    pid->limMaxInt = limMaxInt;
    pid->T = t;
    pid->integrator = 0.0f;
    pid->prevError = 0.0f;
    pid->differentiator = 0.0f;
    pid->prevMeasurement = 0.0f;
    pid->out = 0.0f;
}

/**
 * @brief Resets the PID controller's internal state.
 *
 * @param pid Pointer to the PIDController structure to reset.
 */
void PID_Reset(PIDController *pid)
{
    pid->integrator = 0.0f;
    pid->prevError = 0.0f;
    pid->differentiator = 0.0f;
    pid->prevMeasurement = 0.0f;
    pid->out = 0.0f;
}

/**
 * @brief Updates the PID controller and calculates the control output.
 *
 * @param pid Pointer to the PIDController structure.
 * @param setpoint Desired target value.
 * @param measurement Current measured value.
 * @return Control output to adjust the system.
 */
float PID_Update(PIDController *pid, float setpoint, float measurement)
{
    // Calculate the error between the setpoint and the current measurement
    float error = setpoint - measurement;
    // Proportional term
    float proportional = pid->Kp * error;

    pid->integrator += 0.5f * pid->Ki * pid->T * (error + pid->prevError);

    // Anti-windup: Limit the integrator value to prevent excessive error accumulation
    if (pid->integrator > pid->limMaxInt)
    {
        pid->integrator = pid->limMaxInt;
    }
    else if (pid->integrator < pid->limMinInt)
    {
        pid->integrator = pid->limMinInt;
    }

    // Derivative term with low-pass filter
    // The derivative term is calculated using the difference between the current and previous measurement
    // and is filtered using a first-order low-pass filter to reduce noise
    pid->differentiator = -(2.0f * pid->Kd * (measurement - pid->prevMeasurement) +
                            (2.0f * pid->tau - pid->T) * pid->differentiator) /
                          (2.0f * pid->tau + pid->T);

    pid->out = proportional + pid->integrator + pid->differentiator;

    // Limit the output to the specified limits
    if (pid->out > pid->limMax)
    {
        pid->out = pid->limMax;
    }
    else if (pid->out < pid->limMin)
    {
        pid->out = pid->limMin;
    }

    // Store the current error and measurement for the next iteration
    pid->prevError = error;
    // Store the current measurement for the next derivative calculation
    pid->prevMeasurement = measurement;

    return pid->out;
}

/**
 * @brief Updates multiple PID controllers in a cascaded manner.
 *
 * This function allows updating an array of PID controllers, where the output of one
 * controller can be used as the setpoint for the next one in the array.
 *
 * @param pid_array Pointer to an array of PIDController structures.
 * @param pid_array_size Size of the PID controller array.
 * @param controllers_setpoints Pointer to an array of setpoints for each PID controller.
 * @param sensors_feedback Pointer to an array of sensor feedback values.
 * @param output_array Pointer to an array to store the output values of each PID controller.
 * @param coupling_factor Coupling factor to adjust the influence of one controller's output
 *                        on the next controller's setpoint.
 */
void cascaded_PID_Update(PIDController *pid_array, uint8_t pid_array_size,
                         float *controllers_setpoints,
                         float *sensors_feedback,
                         float *output_array,
                         float coupling_factor)
{
    for (uint8_t i = 0; i < pid_array_size; i++)
    {
        // Update the PID controller with the current setpoint and sensor feedback
        output_array[i] = PID_Update(&pid_array[i], controllers_setpoints[i], sensors_feedback[i]);

        // Set the next controller's setpoint to the current output
        if (i < pid_array_size - 1)
        {
            controllers_setpoints[i + 1] = controllers_setpoints[i + 1] + output_array[i] * coupling_factor;
        }
    }
}

/***************************************************************
 * USERS' FUNCTIONS TO GET AND SET PID CONTROLLER'S PARAMETERS
 ****************************************************************/

/**
 * @brief Updates the PID controller's gains at runtime.
 *
 * @param pid Pointer to the PIDController structure.
 * @param kp New proportional gain.
 * @param ki New integral gain.
 * @param kd New derivative gain.
 */
void PID_UpdateGains(PIDController *pid, float kp, float ki, float kd)
{
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
}

/**
 * @brief Retrieves the proportional gain (Kp) of the PID controller.
 *
 * @param pid Pointer to the PIDController structure.
 * @return Proportional gain (Kp).
 */
float PID_GetKp(const PIDController *pid)
{
    return pid->Kp;
}

/**
 * @brief Retrieves the integral gain (Ki) of the PID controller.
 *
 * @param pid Pointer to the PIDController structure.
 * @return Integral gain (Ki).
 */
float PID_GetKi(const PIDController *pid)
{
    return pid->Ki;
}

/**
 * @brief Retrieves the derivative gain (Kd) of the PID controller.
 *
 * @param pid Pointer to the PIDController structure.
 * @return Derivative gain (Kd).
 */
float PID_GetKd(const PIDController *pid)
{
    return pid->Kd;
}

/**
 * @brief Retrieves all PID gains as a PIDGains structure.
 *
 * @param pid Pointer to the PIDController structure.
 * @return PIDGains structure containing Kp, Ki, and Kd.
 */
PIDGains PID_GetGains(const PIDController *pid)
{
    PIDGains gains;
    gains.Kp = pid->Kp;
    gains.Ki = pid->Ki;
    gains.Kd = pid->Kd;
    return gains;
}
