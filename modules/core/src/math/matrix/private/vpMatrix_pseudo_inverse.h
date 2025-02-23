/*
 * ViSP, open source Visual Servoing Platform software.
 * Copyright (C) 2005 - 2024 by Inria. All rights reserved.
 *
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file LICENSE.txt at the root directory of this source
 * distribution for additional information about the GNU GPL.
 *
 * For using ViSP with software that can not be combined with the GNU
 * GPL, please contact Inria about acquiring a ViSP Professional
 * Edition License.
 *
 * See https://visp.inria.fr for more information.
 *
 * This software was developed at:
 * Inria Rennes - Bretagne Atlantique
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * France
 *
 * If you have questions regarding the use of this file, please contact
 * Inria at visp@inria.fr
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Description:
 * Pseudo inverse computation.
 */

#ifndef VP_MATRIX_PSEUDO_INVERSE_H
#define VP_MATRIX_PSEUDO_INVERSE_H

#include <visp3/core/vpConfig.h>
#include <visp3/core/vpMatrix.h>

BEGIN_VISP_NAMESPACE

#ifndef DOXYGEN_SHOULD_SKIP_THIS

void compute_pseudo_inverse(const vpMatrix &U, const vpColVector &sv, const vpMatrix &V, unsigned int nrows,
                            unsigned int ncols, double svThreshold, vpMatrix &Ap, int &rank_out, int *rank_in,
                            vpMatrix *imA, vpMatrix *imAt, vpMatrix *kerAt);
#endif

END_VISP_NAMESPACE

#endif
