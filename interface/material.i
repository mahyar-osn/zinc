/**
 * material.i
 *
 * Swig interface file for Zinc material.
 */
/*
 * OpenCMISS-Zinc Library
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

%module(package="opencmiss.zinc") material

%include "doublevaluesarraytypemap.i"

%import "field.i"
%import "font.i"

%{
#include "zinc/material.hpp"
#include "zinc/fieldconditional.hpp"
%}

%include "zinc/material.hpp"
