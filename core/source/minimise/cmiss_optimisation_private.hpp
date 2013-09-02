/**
 * @file cmiss_optimisation_private.hpp
 *
 * The private methods for the cmzn_optimisation object. Required to allow creation
 * of optimisation objects.
 *
 * @see-also api/zinc/optimisation.h
 *
 */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is cmgui.
 *
 * The Initial Developer of the Original Code is
 * Auckland Uniservices Ltd, Auckland, New Zealand.
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __CMZN_OPTIMISATION_PRIVATE_HPP__
#define __CMZN_OPTIMISATION_PRIVATE_HPP__

#include <list>
#include <sstream>
#include "zinc/optimisation.h"
#include "computed_field/field_module.hpp"

typedef std::list<cmzn_field_id> FieldList;

struct cmzn_optimisation
{
private:
	~cmzn_optimisation()
	{
		FieldList::iterator iter;
		for (iter = independentFields.begin(); iter != independentFields.end(); ++iter)
		{
			cmzn_field_destroy(&(*iter));
		}
		for (iter = objectiveFields.begin(); iter != objectiveFields.end(); ++iter)
		{
			cmzn_field_destroy(&(*iter));
		}
		cmzn_field_module_destroy(&fieldModule);
	}

public:
	cmzn_field_module_id fieldModule;
	enum cmzn_optimisation_method method;
	FieldList independentFields;
	FieldList objectiveFields;
	// Opt++ stopping tolerances
	double functionTolerance;
	double gradientTolerance;
	double stepTolerance;
	int maximumIterations;
	int maximumNumberFunctionEvaluations;
	// Opt++ steplength control
	double maximumStep;
	double minimumStep;
	// Opt++ globalisation strategy parameters
	double linesearchTolerance;
	int maximumBacktrackIterations;
	double trustRegionSize;
	std::stringbuf solution_report; // solution details output by Opt++ during and after solution
	int access_count;

	cmzn_optimisation(cmzn_field_module_id field_module) :
		fieldModule(cmzn_region_get_field_module(cmzn_field_module_get_master_region_internal(field_module))),
		access_count(1)
	{
		// initialise to default values
		method = CMZN_OPTIMISATION_METHOD_QUASI_NEWTON;
		functionTolerance = 1.49012e-8;
		gradientTolerance = 6.05545e-6;
		stepTolerance = 1.49012e-8;
		maximumIterations = 100;
		maximumNumberFunctionEvaluations = 1000;
		maximumStep = 1.0e3;
		minimumStep = 1.49012e-8;
		linesearchTolerance = 1.e-4;
		maximumBacktrackIterations = 5;
		trustRegionSize = 0.1;
	}

	cmzn_optimisation_id access()
	{
		++access_count;
		return this;
	}

	static int deaccess(cmzn_optimisation_id &optimisation)
	{
		if (!optimisation)
			return 0;
		--(optimisation->access_count);
		if (optimisation->access_count <= 0)
			delete optimisation;
		optimisation = 0;
		return 1;
	}

	cmzn_field_id getFirstIndependentField() const
	{
		cmzn_field_id field = 0;
		FieldList::const_iterator iter = independentFields.begin();
		if (iter != independentFields.end())
		{
			field = cmzn_field_access(*iter);
		}
		return field;
	}

	cmzn_field_id getNextIndependentField(cmzn_field_id ref_field) const
	{
		cmzn_field_id field = 0;
		FieldList::const_iterator iter = independentFields.begin();
		while (iter != independentFields.end())
		{
			if (*iter == ref_field)
			{
				++iter;
				if (iter != independentFields.end())
				{
					field = cmzn_field_access(*iter);
				}
				break;
			}
			++iter;
		}
		return field;
	}

	int addIndependentField(cmzn_field_id field)
	{
		if (!cmzn_field_module_contains_field(fieldModule, field))
			return 0;
		if (!(Computed_field_is_constant(field) || Computed_field_is_type_finite_element(field)))
			return 0;
		if (cmzn_field_get_value_type(field) != CMZN_FIELD_VALUE_TYPE_REAL)
			return 0;
		FieldList::const_iterator iter;
		for (iter = independentFields.begin(); iter != independentFields.end(); ++iter)
		{
			if (*iter == field)
				return 0;
		}
		independentFields.push_back(cmzn_field_access(field));
		return 1;
	}

	int removeIndependentField(cmzn_field_id field)
	{
		FieldList::iterator iter;
		for (iter = independentFields.begin(); iter != independentFields.end(); ++iter)
		{
			if (*iter == field)
			{
				cmzn_field_destroy(&(*iter));
				independentFields.erase(iter);
				return 1;
			}
		}
		return 0;
	}

	cmzn_field_id getFirstObjectiveField() const
	{
		cmzn_field_id field = 0;
		FieldList::const_iterator iter = objectiveFields.begin();
		if (iter != objectiveFields.end())
		{
			field = cmzn_field_access(*iter);
		}
		return field;
	}

	cmzn_field_id getNextObjectiveField(cmzn_field_id ref_field) const
	{
		cmzn_field_id field = 0;
		FieldList::const_iterator iter = objectiveFields.begin();
		while (iter != objectiveFields.end())
		{
			if (*iter == ref_field)
			{
				++iter;
				if (iter != objectiveFields.end())
				{
					field = cmzn_field_access(*iter);
				}
				break;
			}
			++iter;
		}
		return field;
	}

	int addObjectiveField(cmzn_field_id field)
	{
		if (!cmzn_field_module_contains_field(fieldModule, field))
			return 0;
		if (cmzn_field_get_value_type(field) != CMZN_FIELD_VALUE_TYPE_REAL)
			return 0;
		FieldList::const_iterator iter;
		for (iter = objectiveFields.begin(); iter != objectiveFields.end(); ++iter)
		{
			if (*iter == field)
				return 0;
		}
		objectiveFields.push_back(cmzn_field_access(field));
		return 1;
	}

	int removeObjectiveField(cmzn_field_id field)
	{
		FieldList::iterator iter;
		for (iter = objectiveFields.begin(); iter != objectiveFields.end(); ++iter)
		{
			if (*iter == field)
			{
				cmzn_field_destroy(&(*iter));
				objectiveFields.erase(iter);
				return 1;
			}
		}
		return 0;
	}

	char *getSolutionReport();

	int runOptimisation();
};

#endif // __CMZN_OPTIMISATION_PRIVATE_HPP__
