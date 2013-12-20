/**
 * timekeeper.i
 * 
 */
/*
 * OpenCMISS-Zinc Library
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

%module(package="opencmiss.zinc") timekeeper

%import "timenotifier.i"

%{
#include "zinc/timekeeper.hpp"
%}

%include "zinc/timekeeper.hpp"

