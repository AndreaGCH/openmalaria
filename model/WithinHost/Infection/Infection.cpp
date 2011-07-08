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

#include "WithinHost/Infection/Infection.h"
#include "inputData.h"
#include "util/ModelOptions.h"
#include "util/StreamValidator.h"

#include <cmath>

namespace OM { namespace WithinHost {
    
float Infection::cumulativeYstar;
float Infection::cumulativeHstar;
double Infection::alpha_m;
double Infection::decayM;
TimeStep Infection::latentp( TimeStep::never );

void Infection::init () {
  latentp=TimeStep(InputData().getModel().getParameters().getLatentp());
  cumulativeYstar = (float) InputData.getParameter (Params::CUMULATIVE_Y_STAR);
  cumulativeHstar = (float) InputData.getParameter (Params::CUMULATIVE_H_STAR);
  alpha_m = 1.0 - exp(-InputData.getParameter (Params::NEG_LOG_ONE_MINUS_ALPHA_M));
  decayM = InputData.getParameter (Params::DECAY_M);
}


double Infection::immunitySurvivalFactor (double ageInYears, double cumulativeh, double cumulativeY) {
  //Documentation: AJTMH pp22-23
  //effect of cumulative Parasite density (named Dy in AJTM)
  double dY;
  //effect of number of infections experienced since birth (named Dh in AJTM)
  double dH;
  //effect of age-dependent maternal immunity (named Dm in AJTM)
  double dA;
  
  if (cumulativeh <= 1.0) {
    dY=1.0;
    dH=1.0;
  } else {
    dH=1.0 / (1.0 + (cumulativeh-1.0) / cumulativeHstar);
    dY=1.0 / (1.0 + (cumulativeY-_cumulativeExposureJ) / cumulativeYstar);
  }
  dA = 1.0 - alpha_m * exp(-decayM * ageInYears);
  double ret = std::min(dY*dH*dA, 1.0);
  util::streamValidate( ret );
  return ret;
}


Infection::Infection (istream& stream) : _startdate(TimeStep::never) {
    _startdate & stream;
    proteome_ID & stream;
    _density & stream;
    _cumulativeExposureJ & stream; 
}
void Infection::checkpoint (ostream& stream) {
    _startdate & stream;
    proteome_ID & stream;
    _density & stream;
    _cumulativeExposureJ & stream; 
}

} }