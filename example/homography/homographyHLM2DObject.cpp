/****************************************************************************
 *
 * ViSP, open source Visual Servoing Platform software.
 * Copyright (C) 2005 - 2023 by Inria. All rights reserved.
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
 * Example of the HLM (Malis) homography estimation algorithm.
 *
*****************************************************************************/

/*!
  \file homographyHLM2DObject.cpp

  \brief Example of the HLM (Malis) homography estimation algorithm with a
  planar object using vpHomography class.

*/

/*!
  \example homographyHLM2DObject.cpp

  Example of the HLM (Malis) homography estimation algorithm with a planar
  object using vpHomography class.

*/

#include <visp3/core/vpConfig.h>
#include <visp3/core/vpDebug.h>
#include <visp3/core/vpMath.h>
#include <visp3/core/vpRotationMatrix.h>
#include <visp3/core/vpThetaUVector.h>
#include <visp3/vision/vpHomography.h>

#include <stdlib.h>
#include <visp3/core/vpDebug.h>
#include <visp3/core/vpHomogeneousMatrix.h>
#include <visp3/core/vpMath.h>
#include <visp3/core/vpPoint.h>
#include <visp3/io/vpParseArgv.h>
// List of allowed command line options
#define GETOPTARGS "h"
#define L 0.1
#define nbpt 5

#ifdef ENABLE_VISP_NAMESPACE
using namespace VISP_NAMESPACE_NAME;
#endif

void usage(const char *name, const char *badparam);
bool getOptions(int argc, const char **argv);

/*!

  Print the program options.

  \param name : Program name.
  \param badparam : Bad parameter name.


*/
void usage(const char *name, const char *badparam)
{
  fprintf(stdout, "\n\
Test the HLM (Malis) homography estimation algorithm with a planar object.\n\
\n\
SYNOPSIS\n\
  %s [-h]\n",
          name);

  fprintf(stdout, "\n\
OPTIONS:                                               Default\n\
  -h\n\
     Print the help.\n");

  if (badparam) {
    fprintf(stderr, "ERROR: \n");
    fprintf(stderr, "\nBad parameter [%s]\n", badparam);
  }
}
/*!

  Set the program options.

  \param argc : Command line number of parameters.
  \param argv : Array of command line parameters.

  \return false if the program has to be stopped, true otherwise.

*/
bool getOptions(int argc, const char **argv)
{
  const char *optarg_;
  int c;
  while ((c = vpParseArgv::parse(argc, argv, GETOPTARGS, &optarg_)) > 1) {

    switch (c) {
    case 'h':
      usage(argv[0], nullptr);
      return false;
      break;

    default:
      usage(argv[0], optarg_);
      return false;
      break;
    }
  }

  if ((c == 1) || (c == -1)) {
    // standalone param or error
    usage(argv[0], nullptr);
    std::cerr << "ERROR: " << std::endl;
    std::cerr << "  Bad argument " << optarg_ << std::endl << std::endl;
    return false;
  }

  return true;
}

int main(int argc, const char **argv)
{
#if (defined(VISP_HAVE_LAPACK) || defined(VISP_HAVE_EIGEN3) || defined(VISP_HAVE_OPENCV))
  try {
    // Read the command line options
    if (getOptions(argc, argv) == false) {
      return EXIT_FAILURE;
    }

    vpPoint P[nbpt]; //  Point to be tracked
    std::vector<double> xa(nbpt), ya(nbpt);
    std::vector<double> xb(nbpt), yb(nbpt);

    vpPoint aP[nbpt]; //  Point to be tracked
    vpPoint bP[nbpt]; //  Point to be tracked

    P[0].setWorldCoordinates(-L, -L, 0);
    P[1].setWorldCoordinates(2 * L, -L, 0);
    P[2].setWorldCoordinates(L, L, 0);
    P[3].setWorldCoordinates(-L, 3 * L, 0);
    P[4].setWorldCoordinates(0, 0, 0);
    /*
    P[5].setWorldCoordinates(10,20, 0 ) ;
    P[6].setWorldCoordinates(-10,12, 0 ) ;
  */
    vpHomogeneousMatrix bMo(0, 0, 1, 0, 0, 0);
    vpHomogeneousMatrix aMb(1, 0, 0.0, vpMath::rad(10), 0, vpMath::rad(40));
    vpHomogeneousMatrix aMo = aMb * bMo;
    for (unsigned int i = 0; i < nbpt; i++) {
      P[i].project(aMo);
      aP[i] = P[i];
      xa[i] = P[i].get_x();
      ya[i] = P[i].get_y();
    }

    for (unsigned int i = 0; i < nbpt; i++) {
      P[i].project(bMo);
      bP[i] = P[i];
      xb[i] = P[i].get_x();
      yb[i] = P[i].get_y();
    }
    std::cout << "-------------------------------" << std::endl;
    std::cout << "aMb " << std::endl << aMb << std::endl;
    std::cout << "-------------------------------" << std::endl;
    vpHomography aHb;

    vpHomography::HLM(xb, yb, xa, ya, true, aHb);

    aHb /= aHb[2][2];
    std::cout << "aHb computed using the Malis paralax  algorithm: \n" << aHb << std::endl;

    vpRotationMatrix aRb;
    vpTranslationVector aTb;
    vpColVector n;

    std::cout << "-------------------------------" << std::endl;
    std::cout << "extract R, T and n " << std::endl;
    aHb.computeDisplacement(aRb, aTb, n);
    std::cout << "Rotation: aRb" << std::endl;
    std::cout << aRb << std::endl;
    std::cout << "Translation: aTb" << std::endl;
    std::cout << (aTb).t() << std::endl;
    std::cout << "Normal to the plane: n" << std::endl;
    std::cout << (n).t() << std::endl;

    std::cout << "-------------------------------" << std::endl;
    std::cout << "Compare with built homography H = R + t/d " << std::endl;
    vpPlane bp(0, 0, 1, 1);
    vpHomography aHb_built(aMb, bp);
    std::cout << "aHb built from the displacement " << std::endl;
    std::cout << std::endl << aHb_built / aHb_built[2][2] << std::endl;

    aHb_built.computeDisplacement(aRb, aTb, n);
    std::cout << "Rotation: aRb" << std::endl;
    std::cout << aRb << std::endl;
    std::cout << "Translation: aTb" << std::endl;
    std::cout << (aTb).t() << std::endl;
    std::cout << "Normal to the plane: n" << std::endl;
    std::cout << (n).t() << std::endl;

    std::cout << "-------------------------------" << std::endl;
    std::cout << "test if ap = aHb bp" << std::endl;

    for (unsigned int i = 0; i < nbpt; i++) {
      std::cout << "Point " << i << std::endl;
      vpPoint p;
      std::cout << "(";
      std::cout << aP[i].get_x() / aP[i].get_w() << ", " << aP[i].get_y() / aP[i].get_w();
      std::cout << ") =  (";
      p = aHb * bP[i];
      std::cout << p.get_x() / p.get_w() << ",  " << p.get_y() / p.get_w() << ")" << std::endl;
    }

    std::cout << "-------------------------------" << std::endl;
    std::cout << "test displacement" << std::endl;

    std::list<vpRotationMatrix> laRb;
    std::list<vpTranslationVector> laTb;
    std::list<vpColVector> lnb;

    vpHomography::computeDisplacement(aHb, bP[0].get_x(), bP[0].get_y(), laRb, laTb, lnb);

    std::list<vpRotationMatrix>::const_iterator it_laRb = laRb.begin();
    std::list<vpTranslationVector>::const_iterator it_laTb = laTb.begin();
    std::list<vpColVector>::const_iterator it_lnb = lnb.begin();

    int k = 1;
    while (it_lnb != lnb.end()) {
      std::cout << "Solution " << k++ << std::endl;

      aRb = *it_laRb;
      aTb = *it_laTb;
      n = *it_lnb;
      std::cout << "Rotation: aRb" << std::endl;
      std::cout << aRb << std::endl;
      std::cout << "Translation: aTb" << std::endl;
      std::cout << (aTb).t() << std::endl;
      std::cout << "Normal to the plane: n" << std::endl;
      std::cout << (n).t() << std::endl;

      ++it_laRb;
      ++it_laTb;
      ++it_lnb;
    }
    return EXIT_SUCCESS;
  }
  catch (const vpException &e) {
    std::cout << "Catch an exception: " << e << std::endl;
    return EXIT_FAILURE;
  }
#else
  (void)argc;
  (void)argv;
  std::cout << "Cannot run this example: install Lapack, Eigen3 or OpenCV" << std::endl;
  return EXIT_SUCCESS;
#endif
  }
