/*
 This file is part of OpenMalaria.
 
 Copyright (C) 2005-2009 Swiss Tropical Institute and Liverpool School Of Tropical Medicine
 
 OpenMalaria is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or (at
 your option) any later version.
 
 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef Hmod_util_ModelOptions
#define Hmod_util_ModelOptions

namespace OM { namespace util {
    
    /** Flags signalling which versions of some models to use. */
    enum ModelVersion {
	/* Values are written here using left-shifts. 1 << x is equal to pow(2,x)
	* for integers, so each value here has only one bit true in binary, allowing
	* the bits to be used as flags: http://en.wikipedia.org/wiki/Flag_byte
	* For historical reasons only, there is no 1 (=1<<0). */
	/** @brief Clinical episodes reduce the level of acquired immunity
	* 
	* Effective cumulative exposure to blood stage parasites is reduced during a
	* clinical episode, so that clinical episodes have a negative effect on
	* blood stage immunity.
	* 
	* (ImmediateOutcomes model: per event; EventScheduler: once per event.)
	* 
	* Default: Clinical events have no effect on immune status except
	* secondarily via effects of treatment. */
	PENALISATION_EPISODES = 1 << 1,
	
	/** @brief Baseline availability of humans is sampled from a gamma distribution
	* Infections introduced by mass action with negative binomial
	* variation in numbers of infection.
	* 
	* Default: New infections are introduced via a Poisson process as described
	* in AJTMH 75 (suppl 2) pp11-18. */
	NEGATIVE_BINOMIAL_MASS_ACTION = 1 << 2,
	
	/** @brief 
	* 
	* Does nothing if IPT is not present. */
	ATTENUATION_ASEXUAL_DENSITY = 1 << 3,
	
	/** @brief Baseline availability of humans is sampled from a log normal distribution
	* 
	* Infections introduced by mass action with log normal variation in
	* infection rate.
	* 
	* Default: New infections are introduced via a Poisson process as described
	* in AJTMH 75 (suppl 2) pp11-18. */
	LOGNORMAL_MASS_ACTION = 1 << 4,
	
	/** Infections are introduced without using preerythrocytic immunity. */
	NO_PRE_ERYTHROCYTIC = 1 << 5,
	
	/** @brief Bug fixes in Descriptive & DescriptiveIPT within-host models.
	*
	* Really, the code makes more sense if all these are used. But, they are
	* not always used in order to preserve consistant results.
	* 
	* MAX_DENS_RESET is not used since it is unneeded when MAX_DENS_CORRECTION
	* is present and wouldn't make sense when not. */
	// @{
	MAX_DENS_CORRECTION = 1 << 6,
	INNATE_MAX_DENS = 1 << 7,
	MAX_DENS_RESET = 1 << 8,
	//@}
	
	/** @brief Parasite densities are predicted from an autoregressive process
	*
	* Default: Parasite densities are determined from the descriptive model
	* given in AJTMH 75 (suppl 2) pp19-31 .*/
	DUMMY_WITHIN_HOST_MODEL = 1 << 9,
	
	/** Clinical episodes occur if parasitaemia exceeds the pyrogenic threshold.
	* 
	* Default: Clinical episodes are a stochastic function as described in AJTMH
	* 75 (suppl 2) pp56-62. */
	PREDETERMINED_EPISODES = 1 << 10,
	
	/** @brief The presentation model includes simulation of non-malaria fevers
	* 
	* Default: Non-malaria fevers are not simulated. */
	NON_MALARIA_FEVERS = 1 << 11,
	
	/** @brief Pharmacokinetic and pharmacodynamics of drugs are simulated
	* 
	* Default: Drugs have all or nothing effects (except in certain IPTi
	* models). */
	INCLUDES_PK_PD = 1 << 12,
	
	/** @brief Use revised clinical and case management model, ClinicalEventScheduler
	* 
	* Default: use the Tediosi et al case management model (Case management as
	* described in AJTMH 75 (suppl 2) pp90-103), ClinicalImmediateOutcomes. */
	CLINICAL_EVENT_SCHEDULER = 1 << 13,
	
	/** @brief Clinical episodes occur in response to a simple parasite density trigger
	* 
	* Default: Use the Ross et al presentation model (Clinical episodes are a
	* stochastic function as described in AJTMH 75 (suppl 2) pp56-62). */
	MUELLER_PRESENTATION_MODEL = 1 << 14,
	
	/** @brief Simple heterogeneity
	* 
	* Defaults: No heterogeneity.
	* 
	* (Transmission) heterogeneity is incompatible with
	* NEGATIVE_BINOMIAL_MASS_ACTION and LOGNORMAL_MASS_ACTION because both try
	* to adjust _EIRFactor and it is not confirmed that the ways they do this is
	* compatible. */
	// @{
	/// @brief Allow simple heterogeneity in transmission
	TRANS_HET = 1 << 15,
	/// @brief Allow simple heterogeneity in comorbidity
	COMORB_HET = 1 << 16,
	/// @brief Allow simple heterogeneity in treatment seeking
	TREAT_HET = 1 << 17,
	/// @brief Allow correlated heterogeneities in transmission and comorbidity
	COMORB_TRANS_HET = 1 << 18,
	/// @brief Allow correlated heterogeneities in transmission and treatment seeking
	TRANS_TREAT_HET = 1 << 19,
	/// @brief Allow correlated heterogeneities comorbidity and treatment seeking
	COMORB_TREAT_HET = 1 << 20,
	/// @brief Allow correlated heterogeneities in transmission, comorbidity and treatment seeking
	TRIPLE_HET = 1 << 21,
	
	/** @brief Parasite densities are predicted from an empirical model
	*/
	EMPIRICAL_WITHIN_HOST_MODEL = 1 << 22,
	
	/// Used to test if any heterogeneity is present
	ANY_HET = TRANS_HET|COMORB_HET|TREAT_HET|COMORB_TRANS_HET|TRANS_TREAT_HET|TRIPLE_HET,
	ANY_TRANS_HET =  TRANS_HET | COMORB_TRANS_HET | TRANS_TREAT_HET | TRIPLE_HET,
	// @}
	
	// Used by tests; should be 1 plus highest left-shift value of 1
	NUM_VERSIONS = 23,
    };
    
    
    /// Encapsulation for "modelVersion" xml attribute
    class ModelOptions {
    public:
	/** Return true if given option (from CommandLine::Options) is active. */
	static inline bool option(ModelVersion code) {
	    return modelVersion & code;
	}
	
	/// Set options from opts, checking for incompatible versions.
	static void set (int opts);
	
    private:
	/** Model version defines which implementations of hard-coded options should be
	* used. The integer value of modelVersion passed from the .xml is converted to
	* binary with each bit corresponding to a different dichotomous option.  The
	* original default model is modelVersion=0 */
	// Note: the representation could be changed to use bitset or something, allowing more than 32
	// options here. XML representation should also be changed (optional attributes? binary string?).
	static ModelVersion modelVersion;
    };
} }
#endif