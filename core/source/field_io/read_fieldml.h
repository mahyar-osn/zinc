/***************************************************************************//**
 * FILE : read_fieldml.h
 * 
 * Functions for importing regions and fields from FieldML 0.4+ documents.
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
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
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

#if !defined (READ_FIELDML_H)
#define READ_FIELDML_H

struct Cmiss_region;

/***************************************************************************//**
 * Determines whether the named file is FieldML 0.4 format by the quick and
 * dirty method of finding a <Fieldml> tag near the beginning.
 * @return 1 if file is FieldML 0.4 format, 0 if not.
 */
int is_FieldML_file(const char *filename);

/***************************************************************************//**
 * Reads subregions and fields from FieldML 0.4 format file into the region.
 */
int parse_fieldml_file(struct Cmiss_region *region, const char *filename);

#endif /* !defined (READ_FIELDML_H) */
