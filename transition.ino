// **************************************************************************
// * teensyOAV - a port of David Thompsen's OpenAero code to the teensy 4
// *
// * John Ihlein  July 2021
// **************************************************************************

// **************************************************************************
// OpenAero code by David Thompson, included open-source code as per quoted references.
//
// **************************************************************************
// *             GNU GPL V3 notice
// **************************************************************************
// * Copyright (C) 2016 David Thompson
// * 
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * (at your option) any later version.
// * 
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// * 
// * You should have received a copy of the GNU General Public License
// * along with this program. If not, see <http://www.gnu.org/licenses/>.
// * 
// * tl;dr - all derivative code MUST be released with the source code!
// *
// **************************************************************************

// Transition matrix
// Usage: Transition_state = Trans_Matrix[Config.FlightSel][old_flight]
// Config.FlightSel is where you've been asked to go, and old_flight is where you were.
// Transition_state is where you end up :)
const int8_t transMatrix[3][3] PROGMEM =
{
  {TRANSITIONING,         TRANS_P1N_TO_P1_START, TRANS_P2_TO_P1_START},
  {TRANS_P1_TO_P1N_START, TRANSITIONING,         TRANS_P2_TO_P1N_START},
  {TRANS_P1_TO_P2_START,  TRANS_P1N_TO_P2_START, TRANSITIONING}
};

// Flags
bool transitionUpdated = false;

// Locals
int8_t   oldFlight = 3;     // Old flight profile
int8_t   oldTransMode = 0;  // Old transition mode
int16_t  oldTransition = 0;
uint8_t  oldTransitionState = TRANS_P1;
uint8_t  transitionDirection = P2;
uint8_t  transitionState = TRANS_P1;
uint16_t transitionTime = 0;

void profileTransition(void)
{
  //************************************************************
  //* Flight profile / transition state selection
  //*
  //* When transitioning, the flight profile is a moving blend of
  //* Flight profiles P1 to P2. The transition speed is controlled
  //* by the Config.TransitionSpeed setting.
  //* The transition will hold at P1n position if directed to.
  //************************************************************

  // P2 transition point hard-coded to 50% above center
  if (rcInputs[config.flightChan] > 500)
  {
    config.flightSel = 2;  // Flight mode 2 (P2)
  }
  // P1.n transition point hard-coded to 50% below center
  else if (rcInputs[config.flightChan] > -500)
  {
    config.flightSel = 1;  // Flight mode 1 (P1.n)
  }
  // Otherwise the default is P1
  else
  {
    config.flightSel = 0;  // Flight mode 0 (P1)
  }

  // Reset update request each loop
  transitionUpdated = false;

  //************************************************************
  //* Transition state setup/reset
  //*
  //* Set up the correct state for the current setting.
  //* Check for initial startup - the only time that old_flight should be "3".
  //* Also, re-initialise if the transition setting is changed
  //************************************************************
  if ((oldFlight == 3) || (oldTransMode != config.transitionSpeedOut))
  {
    switch (config.flightSel)
    {
      case 0:
        transitionState = TRANS_P1;
        transitionCounter = config.transitionP1;
        break;
      case 1:
        transitionState = TRANS_P1N;
        transitionCounter = config.transitionP1n;  // Set transition point to the user-selected point
        break;
      case 2:
        transitionState = TRANS_P2;
        transitionCounter = config.transitionP2;
        break;
      default:
        break;
    }
    oldFlight = config.flightSel;
    oldTransMode = config.transitionSpeedOut;
  }

  //************************************************************
  //* Transition state handling
  //************************************************************

  // Update timed transition when changing flight modes
  if (config.flightSel != oldFlight)
  {
    // Flag that update is required if mode changed
    transitionUpdated = true;
  }

  // Work out transition number when manually transitioning
  // Convert number to percentage (0 to 100%)
  if (config.transitionSpeedOut == 0)
  {
    // Update the transition variable based on the selected RC channel
    updateTransition();
  }
  else
  {
    // transition_counter counts from 0 to 100 (101 steps)
    transition = transitionCounter;
  }

  // Always in the TRANSITIONING state when Config.TransitionSpeed is 0
  // This prevents state changes when controlled by a channel
  if (config.transitionSpeedOut == 0)
  {
    transitionState = TRANSITIONING;
  }

  // Update transition state change when control value or flight mode changes
  if (transitionUpdated)
  {
    // Update transition state from matrix
    transitionState = (uint8_t)pgm_read_byte(&transMatrix[config.flightSel][oldFlight]);
  }

  // Calculate transition time from user's setting based on the direction of travel
  if (transitionDirection == P2)
  {
    transitionTime = 10 * config.transitionSpeedOut;  // Outbound transition speed
  }
  else
  {
    transitionTime = 10 * config.transitionSpeedIn;  // Inbound transition speed
  }

  // Update state, values and transition_counter every Config.TransitionSpeed if not zero.
  if (((config.transitionSpeedOut != 0) && (transitionTimeout > transitionTime)) ||
      // Update immediately
      transitionUpdated)
  {
    transitionTimeout = 0;
    transitionUpdated = false;

    // Fixed, end-point states
    if (transitionState == TRANS_P1)
    {
      transitionCounter = config.transitionP1;
    }
    else if (transitionState == TRANS_P1N)
    {
      transitionCounter = config.transitionP1n;
    }
    else if (transitionState == TRANS_P2)
    {
      transitionCounter = config.transitionP2;
    }

    // Handle timed transition towards P1
    if ((transitionState == TRANS_P1N_TO_P1_START) || (transitionState == TRANS_P2_TO_P1_START))
    {
      if (transitionCounter > config.transitionP1)
      {
        transitionCounter--;

        // Check end point
        if (transitionCounter <= config.transitionP1)
        {
          transitionCounter = config.transitionP1;
          transitionState = TRANS_P1;
        }
      }
      else
      {
        transitionCounter++;

        // Check end point
        if (transitionCounter >= config.transitionP1)
        {
          transitionCounter = config.transitionP1;
          transitionState = TRANS_P1;
        }
      }

      transitionDirection = P1;
    }

    // Handle timed transition between P1 and P1.n
    if (transitionState == TRANS_P1_TO_P1N_START)
    {
      if (transitionCounter > config.transitionP1n)
      {
        transitionCounter--;

        // Check end point
        if (transitionCounter <= config.transitionP1n)
        {
          transitionCounter = config.transitionP1n;
          transitionState = TRANS_P1N;
        }
      }
      else
      {
        transitionCounter++;

        // Check end point
        if (transitionCounter >= config.transitionP1n)
        {
          transitionCounter = config.transitionP1n;
          transitionState = TRANS_P1N;
        }
      }

      transitionDirection = P2;
    }

    // Handle timed transition between P2 and P1.n
    if (transitionState == TRANS_P2_TO_P1N_START)
    {
      if (transitionCounter > config.transitionP1n)
      {
        transitionCounter--;

        // Check end point
        if (transitionCounter <= config.transitionP1n)
        {
          transitionCounter = config.transitionP1n;
          transitionState = TRANS_P1N;
        }
      }
      else
      {
        transitionCounter++;

        // Check end point
        if (transitionCounter >= config.transitionP1n)
        {
          transitionCounter = config.transitionP1n;
          transitionState = TRANS_P1N;
        }
      }

      transitionDirection = P1;
    }

    // Handle timed transition towards P2
    if ((transitionState == TRANS_P1N_TO_P2_START) || (transitionState == TRANS_P1_TO_P2_START))
    {
      if (transitionCounter > config.transitionP2)
      {
        transitionCounter--;

        // Check end point
        if (transitionCounter <= config.transitionP2)
        {
          transitionCounter = config.transitionP2;
          transitionState = TRANS_P2;
        }
      }
      else
      {
        transitionCounter++;

        // Check end point
        if (transitionCounter >= config.transitionP2)
        {
          transitionCounter = config.transitionP2;
          transitionState = TRANS_P2;
        }
      }

      transitionDirection = P2;
    }

  } // Update transition_counter

  // Zero the I-terms of the opposite state so as to ensure a bump-less transition
  if ((transitionState == TRANS_P1) || (transition == config.transitionP1))
  {
    // Clear P2 I-term while fully in P1
    memset(&integralGyro[P2][ROLL], 0, sizeof(int32_t) * NUMBEROFAXIS);
    integralAccelVertF[P2] = 0.0;
  }
  else if ((transitionState == TRANS_P2) || (transition == config.transitionP2))
  {
    // Clear P1 I-term while fully in P2
    memset(&integralGyro[P1][ROLL], 0, sizeof(int32_t) * NUMBEROFAXIS);
    integralAccelVertF[P1] = 0.0;
  }

  //**********************************************************************
  //* Reset the IMU when using two orientations and just leaving P1 or P2
  //**********************************************************************

  if (config.p1Reference != NO_ORIENT)
  {
    // If Config.FlightSel has changed (switch based) and TransitionSpeed not set to zero, the transition state will change.
    if ((config.transitionSpeedOut != 0) && (transitionState != oldTransitionState) && ((oldTransitionState == TRANS_P1) || (oldTransitionState == TRANS_P2)))
    {
      resetIMU();
    }

    // If TransitionSpeed = 0, the state is always TRANSITIONING so we can't use the old/new state changes.
    // If user is using a knob or TX-slowed switch, TransitionSpeed will be 0.
    else if (
      (config.transitionSpeedOut == 0) &&                                               // Manual transition mode and...
      (((oldTransition == config.transitionP1) && (transition > config.transitionP1)) ||  // Was in P1 or P2
       ((oldTransition == config.transitionP2) && (transition < config.transitionP2)))    // Is not somewhere in-between.
    )
    {
      resetIMU();
    }
  }

  // Save current flight mode
  oldFlight = config.flightSel;

  // Save old transtion state;
  oldTransitionState = transitionState;

  // Save last transition value
  oldTransition = transition;

}
