/*
 * reflow_oven_process.c
 *
 * Created on: Apr 22, 2025
 * Author: adrian
 *
 * Description: Implementation of reflow oven controller state machine
 *              for solder reflow process management.
 */

/******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "reflow_oven_process.h"

// Maximum safe temperature (°C) - emergency stop if exceeded
#define MAX_SAFE_TEMPERATURE 250.0f

// Maximum phase duration (s) - safety timeout
#define MAX_PHASE_DURATION 600 // 10 minutes

// Milliseconds to seconds conversion
#define MS_TO_S 0.001f

// SYSTEM DEFINITIONS
ReflowOven_t ReflowOven;

/******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES
 ******************************************************************************/
static void ReflowOven_transitionToPhase(ReflowPhases_t newPhase, float currentTemperature, uint32_t currentTimeMs);
static float ReflowOven_calculateSetpoint(float currentTemperature, uint32_t elapsedTimeMs);

/******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/
void ReflowOven_Init(void)
{
    /**
     * Configuration parameters for the reflow oven process with safety recommendations:
     *
     * - Pre_HeatUpRate: Recommended 0.5-1.0 °C/s to avoid thermal shock
     * - SoakTempeture: Typically 150-170°C depending on solder paste specs
     * - SoakTime: Usually 60-120s to ensure uniform heating of the PCB
     * - HeatUpRate: Recommended 1.0-2.0 °C/s for optimal reflow performance
     * - ReflowTempeture: Set according to solder paste datasheet (typically 220-240°C)
     * - ReflowTime: Usually 30-60s above liquidus temperature
     * - CoolDownRate: 1.0-4.0 °C/s, critical for good solder joint formation
     * - CoolDownTempeture: Safe handling temperature, usually around 50°C
     */

    // Initialize the reflow oven parameters
    ReflowOven.ReflowParameters.Pre_HeatUpRate = 0.5f;     // °C/s
    ReflowOven.ReflowParameters.SoakTempeture = 150.0f;    // °C
    ReflowOven.ReflowParameters.SoakTime = 60.0f;          // seconds
    ReflowOven.ReflowParameters.HeatUpRate = 1.0f;         // °C/s
    ReflowOven.ReflowParameters.ReflowTempeture = 220.0f;  // °C
    ReflowOven.ReflowParameters.ReflowTime = 30.0f;        // seconds
    ReflowOven.ReflowParameters.CoolDownRate = 1.0f;       // °C/s
    ReflowOven.ReflowParameters.CoolDownTempeture = 50.0f; // °C

    // Set the initial phase to idle and initialize other control variables
    ReflowOven.currentPhase = REFLOW_IDLE;
    ReflowOven.NextPhase = REFLOW_IDLE;
    ReflowOven.phaseStartTime = 0;
    ReflowOven.currentSetpoint = 25.0f; // Room temperature default
    ReflowOven.emergencyStop = false;
    ReflowOven.temperatureAtPhaseStart = 25.0f;
}

bool ReflowOven_modifyParameters(ReflowParameters_enum parameterUpdate, float newParameterValue)
{
    bool success = true;

    // Only allow parameter modification when in IDLE state
    if (ReflowOven.currentPhase != REFLOW_IDLE)
    {
        return false;
    }

    // Validate parameter value ranges
    switch (parameterUpdate)
    {
    case PARAM_Pre_HeatUpRate:
        if (newParameterValue > 0.0f && newParameterValue <= 2.0f)
        {
            ReflowOven.ReflowParameters.Pre_HeatUpRate = newParameterValue;
        }
        else
        {
            success = false;
        }
        break;

    case PARAM_SoakTempeture:
        if (newParameterValue >= 100.0f && newParameterValue <= 180.0f)
        {
            ReflowOven.ReflowParameters.SoakTempeture = newParameterValue;
        }
        else
        {
            success = false;
        }
        break;

    case PARAM_SoakTime:
        if (newParameterValue >= 30.0f && newParameterValue <= 180.0f)
        {
            ReflowOven.ReflowParameters.SoakTime = newParameterValue;
        }
        else
        {
            success = false;
        }
        break;

    case PARAM_HeatUpRate:
        if (newParameterValue > 0.0f && newParameterValue <= 3.0f)
        {
            ReflowOven.ReflowParameters.HeatUpRate = newParameterValue;
        }
        else
        {
            success = false;
        }
        break;

    case PARAM_ReflowTempeture:
        if (newParameterValue >= 200.0f && newParameterValue <= 240.0f)
        {
            ReflowOven.ReflowParameters.ReflowTempeture = newParameterValue;
        }
        else
        {
            success = false;
        }
        break;

    case PARAM_ReflowTime:
        if (newParameterValue >= 10.0f && newParameterValue <= 90.0f)
        {
            ReflowOven.ReflowParameters.ReflowTime = newParameterValue;
        }
        else
        {
            success = false;
        }
        break;

    case PARAM_CoolDownRate:
        if (newParameterValue > 0.0f && newParameterValue <= 5.0f)
        {
            ReflowOven.ReflowParameters.CoolDownRate = newParameterValue;
        }
        else
        {
            success = false;
        }
        break;

    case PARAM_CoolDownTempeture:
        if (newParameterValue >= 25.0f && newParameterValue <= 100.0f)
        {
            ReflowOven.ReflowParameters.CoolDownTempeture = newParameterValue;
        }
        else
        {
            success = false;
        }
        break;

    default:
        success = false;
        break;
    }

    return success;
}

bool ReflowOven_startProcess(void)
{
    // Only allow starting from IDLE state
    if (ReflowOven.currentPhase != REFLOW_IDLE)
    {
        return false;
    }

    // Transition to PREHEAT phase
    ReflowOven.NextPhase = REFLOW_PREHEAT;
    return true;
}

void ReflowOven_stopProcess(void)
{
    // Force transition to cooldown regardless of current state
    if (ReflowOven.currentPhase != REFLOW_IDLE)
    {
        ReflowOven.NextPhase = REFLOW_COOLDOWN;
    }
}

float ReflowOven_operate(PIDController *PID, float currentTemperature, uint32_t currentTimeMs)
{
    float pidOutput = 0.0f;
    uint32_t elapsedTimeMs;
    float targetSetpoint = 0.0f;

    // Safety check - emergency stop if temperature too high
    if (currentTemperature > MAX_SAFE_TEMPERATURE)
    {
        ReflowOven.emergencyStop = true;
        ReflowOven.NextPhase = REFLOW_IDLE;
    }

    // Handle phase transitions if needed
    if (ReflowOven.NextPhase != ReflowOven.currentPhase)
    {
        ReflowOven_transitionToPhase(ReflowOven.NextPhase, currentTemperature, currentTimeMs);
    }

    // Calculate elapsed time in current phase
    elapsedTimeMs = currentTimeMs - ReflowOven.phaseStartTime;

    // Safety timeout - prevent getting stuck in any phase
    if ((elapsedTimeMs > (MAX_PHASE_DURATION * 1000)) && (ReflowOven.currentPhase != REFLOW_IDLE))
    {
        ReflowOven.NextPhase = REFLOW_COOLDOWN;
    }

    // Process current phase
    switch (ReflowOven.currentPhase)
    {
    case REFLOW_PREHEAT:
        // Calculate target temperature based on ramp rate
        targetSetpoint = ReflowOven.temperatureAtPhaseStart +
                         (ReflowOven.ReflowParameters.Pre_HeatUpRate * elapsedTimeMs * MS_TO_S);

        // Cap at soak temperature
        if (targetSetpoint > ReflowOven.ReflowParameters.SoakTempeture)
        {
            targetSetpoint = ReflowOven.ReflowParameters.SoakTempeture;
        }

        // Update PID setpoint
        ReflowOven.currentSetpoint = targetSetpoint;

        // Check if preheat phase is complete
        if (currentTemperature >= ReflowOven.ReflowParameters.SoakTempeture)
        {
            ReflowOven.NextPhase = REFLOW_SOAK;
        }
        break;

    case REFLOW_SOAK:
        // Maintain constant soak temperature
        ReflowOven.currentSetpoint = ReflowOven.ReflowParameters.SoakTempeture;

        // Check if soak time has elapsed
        if (elapsedTimeMs >= (uint32_t)(ReflowOven.ReflowParameters.SoakTime * 1000))
        {
            ReflowOven.NextPhase = REFLOW_HEATUP;
        }
        break;

    case REFLOW_HEATUP:
        // Calculate target temperature based on ramp rate
        targetSetpoint = ReflowOven.temperatureAtPhaseStart +
                         (ReflowOven.ReflowParameters.HeatUpRate * elapsedTimeMs * MS_TO_S);

        // Cap at reflow temperature
        if (targetSetpoint > ReflowOven.ReflowParameters.ReflowTempeture)
        {
            targetSetpoint = ReflowOven.ReflowParameters.ReflowTempeture;
        }

        // Update PID setpoint
        ReflowOven.currentSetpoint = targetSetpoint;

        // Check if heat-up phase is complete
        if (currentTemperature >= ReflowOven.ReflowParameters.ReflowTempeture)
        {
            ReflowOven.NextPhase = REFLOW_REFLOW;
        }
        break;

    case REFLOW_REFLOW:
        // Maintain constant reflow temperature
        ReflowOven.currentSetpoint = ReflowOven.ReflowParameters.ReflowTempeture;

        // Check if reflow time has elapsed
        if (elapsedTimeMs >= (uint32_t)(ReflowOven.ReflowParameters.ReflowTime * 1000))
        {
            ReflowOven.NextPhase = REFLOW_COOLDOWN;
        }
        break;

    case REFLOW_COOLDOWN:
        // Calculate target temperature based on cooling rate
        targetSetpoint = ReflowOven.temperatureAtPhaseStart -
                         (ReflowOven.ReflowParameters.CoolDownRate * elapsedTimeMs * MS_TO_S);

        // Floor at cooldown target temperature
        if (targetSetpoint < ReflowOven.ReflowParameters.CoolDownTempeture)
        {
            targetSetpoint = ReflowOven.ReflowParameters.CoolDownTempeture;
        }

        // Update PID setpoint
        ReflowOven.currentSetpoint = targetSetpoint;

        // Check if cooldown is complete
        if (currentTemperature <= ReflowOven.ReflowParameters.CoolDownTempeture)
        {
            ReflowOven.NextPhase = REFLOW_IDLE;
        }
        break;

    case REFLOW_IDLE:
        // In idle state, maintain a safe room temperature
        ReflowOven.currentSetpoint = 25.0f;
        break;

    default:
        // Failsafe - return to idle if in unexpected state
        ReflowOven.NextPhase = REFLOW_IDLE;
        break;
    }

    // Update PID controller with current setpoint
    pidOutput = PID_Update(PID, ReflowOven.currentSetpoint, currentTemperature);

    return pidOutput;
}

ReflowPhases_t ReflowOven_getCurrentPhase(void)
{
    return ReflowOven.currentPhase;
}

uint32_t ReflowOven_getPhaseElapsedTime(uint32_t currentTimeMs)
{
    return (currentTimeMs - ReflowOven.phaseStartTime) / 1000; // Return in seconds
}

/******************************************************************************
 * PRIVATE FUNCTION IMPLEMENTATIONS
 ******************************************************************************/

/**
 * @brief Handle transition to a new phase
 *
 * @param newPhase The phase to transition to
 * @param currentTemperature Current temperature at transition
 * @param currentTimeMs Current system time in milliseconds
 */
static void ReflowOven_transitionToPhase(ReflowPhases_t newPhase, float currentTemperature, uint32_t currentTimeMs)
{
    // Record current state before transition
    ReflowOven.temperatureAtPhaseStart = currentTemperature;
    ReflowOven.phaseStartTime = currentTimeMs;

    // Update phase
    ReflowOven.currentPhase = newPhase;

    // Reset PID controller when entering new phase to prevent integral windup
    if (newPhase == REFLOW_IDLE || newPhase == REFLOW_PREHEAT)
    {
        PID_Reset(&PID);
    }

    // Initialize setpoint based on new phase
    switch (newPhase)
    {
    case REFLOW_PREHEAT:
        ReflowOven.currentSetpoint = currentTemperature; // Start from current temperature
        break;

    case REFLOW_SOAK:
        ReflowOven.currentSetpoint = ReflowOven.ReflowParameters.SoakTempeture;
        break;

    case REFLOW_HEATUP:
        ReflowOven.currentSetpoint = currentTemperature; // Start from current temperature
        break;

    case REFLOW_REFLOW:
        ReflowOven.currentSetpoint = ReflowOven.ReflowParameters.ReflowTempeture;
        break;

    case REFLOW_COOLDOWN:
        ReflowOven.currentSetpoint = currentTemperature; // Start from current temperature
        break;

    case REFLOW_IDLE:
        ReflowOven.currentSetpoint = 25.0f; // Room temperature
        ReflowOven.emergencyStop = false;   // Clear emergency flag
        break;

    default:
        // Failsafe default
        ReflowOven.currentSetpoint = 25.0f;
        break;
    }
}
