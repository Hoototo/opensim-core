/* -------------------------------------------------------------------------- *
 *                      OpenSim:  FatigableMuscle.cpp                         *
 * -------------------------------------------------------------------------- *
 * The OpenSim API is a toolkit for musculoskeletal modeling and simulation.  *
 * See http://opensim.stanford.edu and the NOTICE file for more information.  *
 * OpenSim is developed at Stanford University and supported by the US        *
 * National Institutes of Health (U54 GM072970, R24 HD065690) and by DARPA    *
 * through the Warrior Web program.                                           *
 *                                                                            *
 * Copyright (c) 2005-2012 Stanford University and the Authors                *
 * Author(s): Peter Loan, Ajay Seth											  *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0.         *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied    *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */

//=============================================================================
// INCLUDES
//=============================================================================
#include "FatigableMuscle.h"

//=============================================================================
// STATICS
//=============================================================================
using namespace std;
using namespace OpenSim;

//=============================================================================
// CONSTRUCTOR(S) AND DESTRUCTOR
//=============================================================================
//_____________________________________________________________________________
/*
 * Default constructor
 */
FatigableMuscle::FatigableMuscle()
{
	constructProperties();
}

//_____________________________________________________________________________
/*
 * Constructor.
 */
FatigableMuscle::FatigableMuscle(const std::string &name, 
					double maxIsometricForce, double optimalFiberLength, 
					double tendonSlackLength, double pennationAngle, 
					double fatigueFactor, double recoveryFactor) : 
	Super(name, maxIsometricForce, optimalFiberLength, tendonSlackLength, 
          pennationAngle)
{
	constructProperties();
	setFatigueFactor(fatigueFactor);
	setRecoveryFactor(recoveryFactor);
}

//_____________________________________________________________________________
/*
 * Construct and initialize properties.
 * All properties are added to the property set. Once added, they can be
 * read in and written to files.
 */
void FatigableMuscle::constructProperties()
{
    constructProperty_fatigue_factor(0.0);
    constructProperty_recovery_factor(0.0);
	constructProperty_default_active_motor_units(0.0);
    constructProperty_default_fatigued_motor_units(0.0);
}

// Define new states and their derivatives in the underlying system
void FatigableMuscle::addToSystem(SimTK::MultibodySystem& system) const
{
	// Allow Millard2012EquilibriumMuscle to add its states, cache, etc.
	// to the system
	Super::addToSystem(system);

	// Now add the states necessary to implement the fatigable behavior
	addStateVariable("target_activation");
	addStateVariable("active_motor_units");
	addStateVariable("fatigued_motor_units");
	// and their corresponding dervivatives
	addCacheVariable("target_activation_deriv", 0.0, SimTK::Stage::Dynamics);
	addCacheVariable("active_motor_units_deriv", 0.0, SimTK::Stage::Dynamics);
	addCacheVariable("fatigued_motor_units_deriv", 0.0, SimTK::Stage::Dynamics);
}

void FatigableMuscle::initStateFromProperties(SimTK::State& s) const
{
    Super::initStateFromProperties(s);
	setActiveMotorUnits(s, getDefaultActiveMotorUnits());
	setFatiguedMotorUnits(s, getDefaultFatiguedMotorUnits());
}

void FatigableMuscle::setPropertiesFromState(const SimTK::State& s)
{
    Super::setPropertiesFromState(s);
	setDefaultActiveMotorUnits(getActiveMotorUnits(s));
	setDefaultFatiguedMotorUnits(getFatiguedMotorUnits(s));
}

//--------------------------------------------------------------------------
// GET & SET Properties
//--------------------------------------------------------------------------
void FatigableMuscle::setFatigueFactor(double aFatigueFactor)
{
	set_fatigue_factor(aFatigueFactor);
}

void FatigableMuscle::setRecoveryFactor(double aRecoveryFactor)
{
	set_recovery_factor(aRecoveryFactor);
}

double FatigableMuscle::getDefaultActiveMotorUnits() const
{
	return get_default_active_motor_units();
}

void FatigableMuscle::setDefaultActiveMotorUnits(double activeMotorUnits) {
    set_default_active_motor_units(activeMotorUnits);
}

double FatigableMuscle::getDefaultFatiguedMotorUnits() const
{
	return get_default_fatigued_motor_units();
}

void FatigableMuscle::setDefaultFatiguedMotorUnits(double fatiguedMotorUnits) {
    set_default_fatigued_motor_units(fatiguedMotorUnits);
}

//--------------------------------------------------------------------------
// GET & SET States and their derivatives
//--------------------------------------------------------------------------
double FatigableMuscle::getTargetActivation(const SimTK::State& s) const 
{	return getStateVariable(s, "target_activation"); }

void FatigableMuscle::setTargetActivation(SimTK::State& s,
										  double fatiguedAct) const
{	setStateVariable(s, "target_activation", fatiguedAct); }

double FatigableMuscle::getTargetActivationDeriv(const SimTK::State& s) const 
{	return getStateVariableDeriv(s, "target_activation"); }

void FatigableMuscle::setTargetActivationDeriv(const SimTK::State& s,
										double fatiguedActDeriv) const 
{	setStateVariableDeriv(s, "target_activation", fatiguedActDeriv); }


double FatigableMuscle::getActiveMotorUnits(const SimTK::State& s) const 
{	return getStateVariable(s, "active_motor_units"); }

void FatigableMuscle::setActiveMotorUnits(SimTK::State& s,
										  double activeMotorUnits) const
{	setStateVariable(s, "active_motor_units", activeMotorUnits); }

double FatigableMuscle::getActiveMotorUnitsDeriv(const SimTK::State& s) const 
{	return getStateVariableDeriv(s, "active_motor_units"); }

void FatigableMuscle::setActiveMotorUnitsDeriv(const SimTK::State& s,
										double activeMotorUnitsDeriv) const 
{	setStateVariableDeriv(s, "active_motor_units", activeMotorUnitsDeriv); }

double FatigableMuscle::getFatiguedMotorUnits(const SimTK::State& s) const
{	return getStateVariable(s, "fatigued_motor_units"); }

void FatigableMuscle::setFatiguedMotorUnits(SimTK::State& s,
											double fatiguedMotorUnits) const
{	setStateVariable(s, "fatigued_motor_units", fatiguedMotorUnits); }

double FatigableMuscle::getFatiguedMotorUnitsDeriv(const SimTK::State& s) const 
{	 return getStateVariableDeriv(s, "fatigued_motor_units"); }

void FatigableMuscle::setFatiguedMotorUnitsDeriv(const SimTK::State& s,
										double fatiguedMotorUnitsDeriv) const
{	setStateVariableDeriv(s, "fatigued_motor_units", fatiguedMotorUnitsDeriv); }


//=============================================================================
// COMPUTATION
//=============================================================================
//_____________________________________________________________________________
/**
 * Compute the derivatives of the muscle states.
 *
 * @param s  system state
 */
SimTK::Vector FatigableMuscle::
computeStateVariableDerivatives(const SimTK::State& s) const
{
	// vector of the derivatives to be returned
	SimTK::Vector derivs(getNumStateVariables(), SimTK::NaN);
	int nd = derivs.size();

	SimTK_ASSERT1(nd == 5, "FatigableMuscle: Expected 5 state variables"
        " but encountered  %f.", nd);

	// compute the rates at which motor units are converted to/from active
    // and fatigued states based on Liu et al. 2008
	double activeMotorUnitsDeriv = - getFatigueFactor()*getActiveMotorUnits(s) 
			+ getRecoveryFactor() * getFatiguedMotorUnits(s);

	double fatigueMotorUnitsDeriv = getFatigueFactor()* getActiveMotorUnits(s) 
			- getRecoveryFactor() * getFatiguedMotorUnits(s);
    
	//Compute the target activation rate based on the given activation model
	const MuscleFirstOrderActivationDynamicModel& actMdl 
            = get_MuscleFirstOrderActivationDynamicModel();

	double excitation = getExcitation(s);
	// use the activation dynamics model to calculate the target activation
    double targetActivation = actMdl.clampActivation(getTargetActivation(s));
	double targetActivationRate = actMdl.calcDerivative(targetActivation, excitation);

	// specify the activation derivative based on the amount of active motor 
	// units and the rate at which motor units are becoming active.
	// we assume that the actual activation = Ma*a       then,
	// activationRate = dMa/dt*a + Ma*da/dt  where a is the target activation
	double activationRate = activeMotorUnitsDeriv*targetActivation + 
							getActiveMotorUnits(s)*targetActivationRate;

	// set the actual activation rate of the muscle to the fatigued one
	derivs[0] = activationRate;
	// fiber length derivative
	derivs[1] = getFiberVelocity(s);
	// the target activation derivative
	derivs[2] = targetActivationRate;
	derivs[3] = activeMotorUnitsDeriv;
	derivs[4] = fatigueMotorUnitsDeriv;

	// cache the results for fast access by reporting, etc...
	setTargetActivationDeriv(s, targetActivationRate);
	setActiveMotorUnitsDeriv(s, activeMotorUnitsDeriv);
	setFatiguedMotorUnitsDeriv(s, fatigueMotorUnitsDeriv);

	return derivs;
}

/* Determine the initial state values based on initial fiber equlibrium. */
void FatigableMuscle::computeInitialFiberEquilibrium(SimTK::State& s) const
{
	// initialize th target activation to be the actual.
	setTargetActivation(s, getActivation(s));
	// assume that all motor units can be activated initially and there is
	// no appreciable fatigue
	setActiveMotorUnits(s, 1.0);
	setFatiguedMotorUnits(s, 0.0);

	// Compute the fiber & tendon lengths according to the parent Muscle 
	Super::computeInitialFiberEquilibrium(s);
}