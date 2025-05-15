/****************************************************************************************
 * @file pid.h
 * @brief Header file for the PID controller implementation for embedded systems.
 *        This file contains the declarations of the structures and functions used
 *        to implement a PID controller, including proportional, integral, and
 *        derivative calculations, dynamic gain updates, and integrator wind-up protection.
 *
 * @author Adrian Silva Palafox
 * @date November 2024
 *
 * @attention MIT License
 *            This code is open source and can be modified and distributed for educational
 *            or commercial purposes.
 *
 * @note Ensure that the necessary libraries are correctly included and configured
 *       in your development environment.
 ***************************************************************************************/

#ifndef PID_H
#define PID_H

#include <stdint.h>

/**
 * @brief Structure to group the PID controller gains.
 *
 * This structure is optional and allows returning the three gains (Kp, Ki, Kd)
 * in a single object.
 */
typedef struct
{
    float Kp; ///< Proportional gain
    float Ki; ///< Integral gain
    float Kd; ///< Derivative gain
} PIDGains;

/**
 * @brief Structure for the PID controller.
 *
 * This structure contains all the parameters, limits, and internal state variables
 * required for the PID controller's operation.
 */
typedef struct
{
    // PID parameters
    float Kp; ///< Proportional gain
    float Ki; ///< Integral gain
    float Kd; ///< Derivative gain

    float tau; ///< Low-pass filter time constant for the derivative term

    // Controller output limits
    float limMin; ///< Minimum output limit
    float limMax; ///< Maximum output limit

    // Integrator limits (to prevent wind-up)
    float limMinInt; ///< Minimum integrator limit
    float limMaxInt; ///< Maximum integrator limit

    // Sampling time (in seconds)
    float T; ///< Sampling time interval between updates

    // Internal memory variables
    float integrator;      ///< Integral term accumulator
    float prevError;       ///< Previous iteration error (used for derivative calculation)
    float differentiator;  ///< Derivative term value
    float prevMeasurement; ///< Previous measurement (used for derivative calculation)

    // Controller output
    float out; ///< Final output of the PID controller
} PIDController;

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
              float tau, float limMin, float limMax,
              float limMinInt, float limMaxInt, float t);

/**
 * @brief Resets the PID controller to its initial state.
 *
 * This function clears the integrator, previous error, and other internal state variables.
 *
 * @param pid Pointer to the PIDController structure to reset.
 */
void PID_Reset(PIDController *pid);

/**
 * @brief Updates the PID controller and calculates the control output.
 *
 * This function computes the PID output based on the current setpoint and measurement.
 *
 * @param pid Pointer to the PIDController structure.
 * @param setpoint Desired target value.
 * @param measurement Current measured value.
 * @return Control output to adjust the system.
 */
float PID_Update(PIDController *pid, float setpoint, float measurement);

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
                         float coupling_factor);

/***************************************************************
 * USERS' FUNCTIONS TO GET AND SET PID CONTROLLER'S PARAMETERS
 ****************************************************************/

/**
 * @brief Updates the PID controller's gains at runtime.
 *
 * This function allows modifying the proportional, integral, and derivative gains
 * dynamically during runtime.
 *
 * @param pid Pointer to the PIDController structure.
 * @param kp New proportional gain.
 * @param ki New integral gain.
 * @param kd New derivative gain.
 */
void PID_UpdateGains(PIDController *pid, float kp, float ki, float kd);

/**
 * @brief Retrieves the proportional gain (Kp) of the PID controller.
 *
 * @param pid Pointer to the PIDController structure.
 * @return Proportional gain (Kp).
 */
float PID_GetKp(const PIDController *pid);

/**
 * @brief Retrieves the integral gain (Ki) of the PID controller.
 *
 * @param pid Pointer to the PIDController structure.
 * @return Integral gain (Ki).
 */
float PID_GetKi(const PIDController *pid);

/**
 * @brief Retrieves the derivative gain (Kd) of the PID controller.
 *
 * @param pid Pointer to the PIDController structure.
 * @return Derivative gain (Kd).
 */
float PID_GetKd(const PIDController *pid);

/**
 * @brief Retrieves all PID gains as a PIDGains structure.
 *
 * This function returns the proportional, integral, and derivative gains
 * grouped in a single structure.
 *
 * @param pid Pointer to the PIDController structure.
 * @return PIDGains structure containing Kp, Ki, and Kd.
 */
PIDGains PID_GetGains(const PIDController *pid);

#endif // PID_H
