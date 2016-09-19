/**
 * FILE : element_field_template.hpp
 *
 * Describes parameter mapping and interpolation for a scalar
 * field or field component over an element.
 */
/* OpenCMISS-Zinc Library
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#if !defined (CMZN_ELEMENT_FIELD_TEMPLATE_HPP)
#define CMZN_ELEMENT_FIELD_TEMPLATE_HPP

#include "opencmiss/zinc/element.h"
#include "opencmiss/zinc/status.h"

struct FE_basis;
class FE_mesh;

class FE_element_field_template
{
private:

	FE_mesh *mesh; // accessed
	FE_basis *basis; // accessed
	const int numberOfFunctions; // number of basis functions, cached from basis

	bool locked; // set once in use by field or element template, so not modifiable
	cmzn_element_parameter_mapping_mode mappingMode; // NODE|ELEMENT|CONSTANT

	// for each basis function: number of terms summed to give parameter;
	// currently only supported for node mapping mode; all others have 1 term:
	int *termCounts;
	int totalTermCount; // sum of termCounts[fn] for all functions
	// for each basis function, offset into term arrays for the first term with others following
	// equals cummulative value of termCounts up to that function number:
	int *termOffsets;

	// node mapping information:
	int numberOfLocalNodes; // non-zero only for node mapping mode
	// packed arrays of termCounts[fn] local node index, value label and version
	// for each function, in order of function number (size = totalTermCount, looked up by termOffsets):
	int *localNodeIndexes;
	cmzn_node_value_label *nodeValueLabels;
	int *nodeVersions;

	// scale factor information: each term can be multiplied by zero or more scale factors
	int numberOfLocalScaleFactors; // zero if unscaled; scaling currently only supported with node mapping
	// metadata identifying type and version of each local scale factor, for merging:
	cmzn_element_scale_factor_type *scaleFactorTypes;
	int *scaleFactorVersions;
	// packed array of all local scale factor indexes to be multiplied for each
	// term for each function, in order of function then term then factor (size = totalLocalScaleFactorIndexes):
	int *localScaleFactorIndexes;
	// packed arrays of termCounts[fn] number of scale factors to multiply, and
	// offsets into above array where the indexes are held (size = totalTermCount, looked up by termOffsets):
	int *termScaleFactorCounts;
	int *termLocalScaleFactorIndexesOffsets;
	// sum of termScaleFactorCounts[termOffsets[fn] + term] = size of localScaleFactorIndexes array:
	int totalLocalScaleFactorIndexes;

	bool simpleUnscaledNodeOptimisation; // set once locked: flag for simple case of single term unscaled Lagrange/Simplex with standard node order

	int access_count;

	FE_element_field_template(FE_mesh *meshIn, FE_basis *basisIn);

	FE_element_field_template(const FE_element_field_template &source);

	~FE_element_field_template();

	FE_element_field_template &operator=(const FE_element_field_template &source); // not implemented

	void clearNodeMapping();

	void clearScaling();

	inline bool validTerm(int functionNumber, int term) const
	{
		return (0 <= functionNumber) && (functionNumber < this->numberOfFunctions) &&
			(0 <= term) && (term < this->termCounts[functionNumber]);
	}

public:

	static FE_element_field_template *create(FE_mesh *meshIn, FE_basis *basisIn);

	/** create an unlocked copy of the template for modification (copy-on-write) */
	FE_element_field_template *cloneForModify()
	{
		return new FE_element_field_template(*this);
	}

	FE_element_field_template *access()
	{
		++access_count;
		return this;
	}

	static int deaccess(FE_element_field_template* &eft)
	{
		if (!eft)
			return CMZN_ERROR_ARGUMENT;
		--(eft->access_count);
		if (eft->access_count <= 0)
			delete eft;
		eft = 0;
		return CMZN_OK;
	}

	/** Returns true if locked i.e. in use and read only; caller should clone to modify */
	bool isLocked() const
	{
		return this->locked;
	}

	cmzn_element_parameter_mapping_mode getElementParameterMappingMode() const
	{
		return this->mappingMode;
	}

	/** Sets the parameter mapping mode, resets to one term per basis function with no scaling hence should be the first setting changed. */
	int setElementParameterMappingMode(cmzn_element_parameter_mapping_mode modeIn);

	/** @return  Number of basis functions */
	int getNumberOfFunctions() const
	{
		return this->numberOfFunctions;
	}

	int getNumberOfLocalNodes() const
	{
		return this->numberOfLocalNodes;
	}

	/** Set the number of local nodes. If reducing number, template is only valid
	  * once all indexes are in range 0..number-1.
	  * Only valid in node mapping mode. */
	int setNumberOfLocalNodes(int number);

	/** Get the number of terms that are summed to give the element parameter weighting
	  * the given function number.
	  * @param functionNumber  Basis function number from 0 to numberOfFunctions - 1.
	  * @return  Number of terms >= 0, or -1 if invalid function number */
	int getFunctionNumberOfTerms(int functionNumber) const
	{
		if ((functionNumber < 0) || (functionNumber >= this->numberOfFunctions))
			return -1; // since can be zero terms
		return this->termCounts[functionNumber];
	}

	/** Set the number of terms that are summed to give the element parameter weighting
	  * the given function number. Currently only supported for node mapping.
	  * If reducing number, existing mappings for higher terms are discarded.
	  * If increasing number, new mappings must be completely specified by subsequent
	  * calls; new mappings are unscaled by default.
	  * @param functionNumber  Basis function number from 0 to numberOfFunctions - 1.
	  * @param newNumberOfTerms  New number of terms to be summed, >= 0.
	  * @return  Result OK on success, otherwise error code */
	int setFunctionNumberOfTerms(int functionNumber, int newNumberOfTerms);

	/** @return  Local node index starting at 0, or -1 if invalid function or term */
	int getTermLocalNodeIndex(int functionNumber, int term) const;

	/** @return  Node value label or INVALID if not set or invalid function or term */
	cmzn_node_value_label getTermNodeValueLabel(int functionNumber, int term) const;

	/** @return  Local node index starting at 0, or -1 if invalid function or term */
	int getTermNodeVersion(int functionNumber, int term) const;

	/** For node mapping mode, set the given term for function number to look up
	  * the value of the node value label & version at local node index.
	  * @param functionNumber Basis function number from 0 to numberOfFunctions - 1.
	  * @param term  Term number from 0 to function number of terms - 1 */
	int setTermNodeParameter(int functionNumber, int term, int localNodeIndex, cmzn_node_value_label valueLabel, int version);

	int getNumberOfLocalScaleFactors() const
	{
		return this->numberOfLocalScaleFactors;
	}

	/** Set the number of local scale factors. If reducing number, template is
	  * only valid once all indexes are in range 0..number-1.
	  * Only valid in node mapping mode. */
	int setNumberOfLocalScaleFactors(int number);

	cmzn_element_scale_factor_type getElementScaleFactorType(int localIndex) const
	{
		if ((0 <= localIndex) && (localIndex < this->numberOfLocalScaleFactors))
			return this->scaleFactorTypes[localIndex];
		return CMZN_ELEMENT_SCALE_FACTOR_TYPE_INVALID;
	}

	/** Set the type of scale factor mapped at the local index, used with the
	  * scale factor version, global node, derivative etc. to merge common
	  * scale factors from different fields */
	int setElementScaleFactorType(int localIndex, cmzn_element_scale_factor_type type);

	int getElementScaleFactorVersion(int localIndex) const
	{
		if ((0 <= localIndex) && (localIndex < this->numberOfLocalScaleFactors))
			return this->scaleFactorVersions[localIndex];
		return -1;
	}

	/** Set the version of scale factor mapped at the local index, used with the
	  * scale factor type, global node, derivative etc. to merge common
	  * scale factors from different fields */
	int setElementScaleFactorVersion(int localIndex, int version);

	/** @param startIndex  Set to 1 to offset to external indexes which start at 1
	  * @return  The actual number of scaling indexes for term, or -1 on error. */
	int getTermScaling(int functionNumber, int term, int indexesCount, int *indexes, int startIndex = 0);

	/** Set scaling of the function term by the product of scale factors at the
	  * given local scale factor indexes. Only valid in node mapping mode.
	  * Must have set positive number of local scale factors.
	  * @param startIndex  Set to 1 to offset from external indexes which start at 1 */
	int setTermScaling(int functionNumber, int term, int indexesCount, const int *indexes, int startIndex = 0);
};

struct cmzn_elementfieldtemplate
{
private:

	FE_element_field_template *impl;
	int access_count;

	cmzn_elementfieldtemplate(FE_element_field_template *implIn) :
		impl(implIn->access()),
		access_count(1)
	{
	}

	~cmzn_elementfieldtemplate()
	{
		FE_element_field_template::deaccess(this->impl);
	}

	/** Call before modifying object to ensure it is either unlocked or an unlocked copy is created */
	void copyOnWrite()
	{
		if (this->impl->isLocked())
		{
			FE_element_field_template *tmp = this->impl;
			this->impl = tmp->cloneForModify();
			FE_element_field_template::deaccess(tmp);
		}
	}

public:

	static cmzn_elementfieldtemplate *create(FE_mesh *meshIn, FE_basis *basisIn)
	{
		FE_element_field_template *impl = FE_element_field_template::create(meshIn, basisIn);
		if (impl)
			return new cmzn_elementfieldtemplate(impl);
		return 0;
	}

	cmzn_elementfieldtemplate *access()
	{
		++access_count;
		return this;
	}

	static int deaccess(cmzn_elementfieldtemplate* &eft)
	{
		if (!eft)
			return CMZN_ERROR_ARGUMENT;
		--(eft->access_count);
		if (eft->access_count <= 0)
			delete eft;
		eft = 0;
		return CMZN_OK;
	}

	cmzn_element_parameter_mapping_mode getElementParameterMappingMode() const
	{
		return this->impl->getElementParameterMappingMode();
	}

	int setElementParameterMappingMode(cmzn_element_parameter_mapping_mode mode)
	{
		this->copyOnWrite();
		return this->impl->setElementParameterMappingMode(mode);
	}

	int getNumberOfFunctions() const
	{
		return this->impl->getNumberOfFunctions();
	}

	int getNumberOfLocalNodes() const
	{
		return this->impl->getNumberOfLocalNodes();
	}

	int setNumberOfLocalNodes(int number)
	{
		this->copyOnWrite();
		return this->impl->setNumberOfLocalNodes(number);
	}

	int getFunctionNumberOfTerms(int functionNumber) const
	{
		return this->impl->getFunctionNumberOfTerms(functionNumber - 1);
	}

	int setFunctionNumberOfTerms(int functionNumber, int newNumberOfTerms)
	{
		this->copyOnWrite();
		return this->impl->setFunctionNumberOfTerms(functionNumber - 1, newNumberOfTerms);
	}

	int getTermLocalNodeIndex(int functionNumber, int term) const
	{
		return this->impl->getTermLocalNodeIndex(functionNumber - 1, term - 1) + 1;
	}

	cmzn_node_value_label getTermNodeValueLabel(int functionNumber, int term) const
	{
		return this->impl->getTermNodeValueLabel(functionNumber - 1, term - 1);
	}

	int getTermNodeVersion(int functionNumber, int term) const
	{
		return this->impl->getTermNodeVersion(functionNumber - 1, term - 1) + 1;
	}

	int setTermNodeParameter(int functionNumber, int term, int localNodeIndex, cmzn_node_value_label valueLabel, int version)
	{
		this->copyOnWrite();
		return this->impl->setTermNodeParameter(functionNumber - 1, term - 1, localNodeIndex - 1, valueLabel, version - 1);
	}

	int getNumberOfLocalScaleFactors() const
	{
		return this->impl->getNumberOfLocalScaleFactors();
	}

	int setNumberOfLocalScaleFactors(int number)
	{
		this->copyOnWrite();
		return this->impl->setNumberOfLocalScaleFactors(number);
	}

	cmzn_element_scale_factor_type getElementScaleFactorType(int localIndex) const
	{
		return this->impl->getElementScaleFactorType(localIndex - 1);
	}

	int setElementScaleFactorType(int localIndex, cmzn_element_scale_factor_type type)
	{
		this->copyOnWrite();
		return this->impl->setElementScaleFactorType(localIndex - 1, type);
	}

	int getElementScaleFactorVersion(int localIndex) const
	{
		return this->impl->getElementScaleFactorVersion(localIndex - 1) + 1;
	}

	int setElementScaleFactorVersion(int localIndex, int version)
	{
		this->copyOnWrite();
		return this->impl->setElementScaleFactorVersion(localIndex - 1, version - 1);
	}

	int getTermScaling(int functionNumber, int term, int indexesCount, int *indexes)
	{
		return this->impl->getTermScaling(functionNumber - 1, term - 1, indexesCount, indexes, /*startIndex*/1);
	}

	int setTermScaling(int functionNumber, int term, int indexesCount, const int *indexes)
	{
		this->copyOnWrite();
		return this->impl->setTermScaling(functionNumber - 1, term - 1, indexesCount, indexes, /*startIndex*/1);
	}

};

#endif /* !defined (CMZN_ELEMENT_FIELD_TEMPLATE_HPP) */
