/* This file is part of OpenMalaria.
 * 
 * Copyright (C) 2005-2014 Swiss Tropical and Public Health Institute
 * Copyright (C) 2005-2014 Liverpool School Of Tropical Medicine
 * 
 * OpenMalaria is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "Clinical/CaseManagementCommon.h"
#include "Clinical/ImmediateOutcomes.h"
#include "Clinical/ESCaseManagement.h"
#include "util/ModelOptions.h"
#include "util/errors.h"
#include <limits>
#include <boost/format.hpp>

namespace OM { namespace Clinical {
    using util::AgeGroupInterpolation;
    
    util::AgeGroupInterpolation* CaseManagementCommon::caseFatalityRate = AgeGroupInterpolation::dummyObject();
    double CaseManagementCommon::_oddsRatioThreshold;
    util::AgeGroupInterpolation* CaseManagementCommon::pSeqInpatient = AgeGroupInterpolation::dummyObject();
    
    // -----  functions  -----
    
    void CaseManagementCommon::initCommon( const OM::Parameters& parameters ){
	_oddsRatioThreshold = exp (parameters[Parameters::LOG_ODDS_RATIO_CF_COMMUNITY]);
    }
    void CaseManagementCommon::cleanupCommon (){
        AgeGroupInterpolation::freeObject( caseFatalityRate );
        AgeGroupInterpolation::freeObject( pSeqInpatient );
    }
    
    void CaseManagementCommon::changeHealthSystem( const scnXml::HealthSystem& healthSystem ){
	readCommon( healthSystem );
	
	if (util::ModelOptions::option (util::CLINICAL_EVENT_SCHEDULER)){
	    ESCaseManagement::setHealthSystem(healthSystem);
        }else{
	    ClinicalImmediateOutcomes::setHealthSystem(healthSystem);
        }
    }
    
    void CaseManagementCommon::readCommon (const scnXml::HealthSystem& healthSystem)
    {
	// -----  case fatality rates  -----
        AgeGroupInterpolation::freeObject( caseFatalityRate );
	caseFatalityRate = AgeGroupInterpolation::makeObject( healthSystem.getCFR(), "CFR" );
	
	// -----  sequelae  -----
        AgeGroupInterpolation::freeObject( pSeqInpatient );
        pSeqInpatient = AgeGroupInterpolation::makeObject( healthSystem.getPSequelaeInpatient(), "pSequelaeInpatient" );
    }
    
    double CaseManagementCommon::getCommunityCaseFatalityRate (double caseFatalityRatio)
    {
	double x = caseFatalityRatio * _oddsRatioThreshold;
	return x / (1 - caseFatalityRatio + x);
    }
} }
