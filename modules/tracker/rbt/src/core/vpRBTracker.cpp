/****************************************************************************
 *
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
*****************************************************************************/

#include <visp3/rbt/vpRBTracker.h>

#if defined(VISP_HAVE_NLOHMANN_JSON)
#include <nlohmann/json.hpp>
#endif

#include <visp3/core/vpExponentialMap.h>

#include <visp3/ar/vpPanda3DRendererSet.h>
#include <visp3/ar/vpPanda3DGeometryRenderer.h>
#include <visp3/ar/vpPanda3DRGBRenderer.h>

#include <visp3/rbt/vpColorHistogramMask.h>
#include <visp3/rbt/vpRBFeatureTrackerFactory.h>
#include <visp3/rbt/vpRBDriftDetectorFactory.h>
#include <visp3/rbt/vpObjectMaskFactory.h>

#include <visp3/rbt/vpRBInitializationHelper.h>

#define VP_DEBUG_RB_TRACKER 1


vpRBTracker::vpRBTracker() : m_lambda(1.0), m_vvsIterations(10), m_muInit(0.0), m_muIterFactor(0.5), m_imageHeight(480), m_imageWidth(640),
m_firstIteration(true), m_renderer(m_rendererSettings), m_trackers(0)
{
  m_rendererSettings.setClippingDistance(0.01, 1.0);
  const std::shared_ptr<vpPanda3DGeometryRenderer> geometryRenderer = std::make_shared<vpPanda3DGeometryRenderer>(
    vpPanda3DGeometryRenderer::vpRenderType::OBJECT_NORMALS);

  //geometryRenderer->setRenderOrder(-1000);
  m_renderer.addSubRenderer(geometryRenderer);
  // std::shared_ptr<vpPanda3DDepthGaussianBlur> blur = std::make_shared<vpPanda3DDepthGaussianBlur>(
  //   "depthBlur", geometryRenderer, true);
  // m_renderer.addSubRenderer(blur);
  m_renderer.addSubRenderer(std::make_shared<vpPanda3DDepthCannyFilter>(
    "depthCanny", geometryRenderer, true, 0.0));
  //m_renderer.addSubRenderer(std::make_shared<vpPanda3DRGBRenderer>(false));

  m_renderer.setRenderParameters(m_rendererSettings);
  //m_renderer.initFramework();
  m_driftDetector = nullptr;

}

void vpRBTracker::getPose(vpHomogeneousMatrix &cMo) const
{
  cMo = m_cMo;
}

void vpRBTracker::setPose(const vpHomogeneousMatrix &cMo)
{
  m_cMo = cMo;
  m_cMoPrev = cMo;
  m_renderer.setCameraPose(cMo.inverse());
}

vpCameraParameters vpRBTracker::getCameraParameters() const { return m_cam; }

void vpRBTracker::setCameraParameters(const vpCameraParameters &cam, unsigned h, unsigned w)
{
  m_cam = cam;
  m_imageHeight = h;
  m_imageWidth = w;
  m_rendererSettings.setCameraIntrinsics(m_cam);
  m_rendererSettings.setImageResolution(m_imageHeight, m_imageWidth);
  m_renderer.setRenderParameters(m_rendererSettings);
}

void vpRBTracker::setSilhouetteExtractionParameters(const vpSilhouettePointsExtractionSettings &settings)
{
  m_depthSilhouetteSettings = settings;
}


void vpRBTracker::reset()
{
  m_firstIteration = true;
}

void vpRBTracker::loadObjectModel(const std::string &file)
{
  m_renderer.initFramework();
  //m_renderer.enableSharedDepthBuffer(*m_renderer.getRenderer<vpPanda3DGeometryRenderer>());
  m_renderer.addLight(vpPanda3DAmbientLight("ambient", vpRGBf(0.4f)));
  //m_renderer.addLight(vpPanda3DDirectionalLight("dir", vpRGBf(1.f), vpColVector({ 0.0, -0.0, 1.0 })));
  //m_renderer.addLight(vpPanda3DPointLight("point", vpRGBf(8.f), vpColVector({ 0.0, 0.1, 0.1 }), vpColVector({ 1.0, 0.0, 0.0 })));
  m_renderer.addNodeToScene(m_renderer.loadObject("object", file));
  m_renderer.setFocusedObject("object");
}

void vpRBTracker::track(const vpImage<unsigned char> &I)
{
  for (std::shared_ptr<vpRBFeatureTracker> tracker : m_trackers) {
    if (tracker->requiresDepth() || tracker->requiresRGB()) {
      throw vpException(vpException::badValue, "Some tracked features require RGB or depth features");
    }
  }
  checkDimensionsOrThrow(I, "grayscale");
  vpRBFeatureTrackerInput frameInput;
  frameInput.I = I;
  frameInput.cam = m_cam;
  track(frameInput);
}

void vpRBTracker::track(const vpImage<unsigned char> &I, const vpImage<vpRGBa> &IRGB)
{
  for (std::shared_ptr<vpRBFeatureTracker> &tracker : m_trackers) {
    if (tracker->requiresDepth()) {
      throw vpException(vpException::badValue, "Some tracked features require depth features");
    }
  }
  checkDimensionsOrThrow(I, "grayscale");
  checkDimensionsOrThrow(IRGB, "color");
  vpRBFeatureTrackerInput frameInput;
  frameInput.I = I;
  frameInput.IRGB = IRGB;
  frameInput.cam = m_cam;
  track(frameInput);
}

void vpRBTracker::track(const vpImage<unsigned char> &I, const vpImage<vpRGBa> &IRGB, const vpImage<float> &depth)
{
  checkDimensionsOrThrow(I, "grayscale");
  checkDimensionsOrThrow(IRGB, "color");
  checkDimensionsOrThrow(depth, "depth");
  vpRBFeatureTrackerInput frameInput;
  frameInput.I = I;
  frameInput.IRGB = IRGB;
  frameInput.depth = depth;
  frameInput.cam = m_cam;
  track(frameInput);
}

void vpRBTracker::track(vpRBFeatureTrackerInput &input)
{
  m_logger.reset();

  m_logger.startTimer();
  updateRender(input);
  m_logger.setRenderTime(m_logger.endTimer());
  if (m_firstIteration) {
    m_firstIteration = false;
    m_previousFrame.I = input.I;
    m_previousFrame.IRGB = input.IRGB;
  }

  m_logger.startTimer();
  if (m_mask) {
    m_mask->updateMask(input, m_previousFrame, input.mask);
  }
  m_logger.setMaskTime(m_logger.endTimer());


  bool requiresSilhouetteCandidates = false;
  for (std::shared_ptr<vpRBFeatureTracker> &tracker : m_trackers) {
    if (tracker->requiresSilhouetteCandidates()) {
      requiresSilhouetteCandidates = true;
      break;
    }
  }

  m_logger.startTimer();
  if (requiresSilhouetteCandidates) {
    const vpHomogeneousMatrix cTcp = m_cMo * m_cMoPrev.inverse();
    input.silhouettePoints = extractSilhouettePoints(input.renders.normals, input.renders.depth,
                                                    input.renders.silhouetteCanny, input.renders.isSilhouette, input.cam, cTcp);
    if (input.silhouettePoints.size() == 0) {
      throw vpException(vpException::badValue, "Could not extract silhouette from depth canny: Object may not be in image");
    }
  }
  m_logger.setSilhouetteTime(m_logger.endTimer());



  for (std::shared_ptr<vpRBFeatureTracker> &tracker : m_trackers) {
    tracker->onTrackingIterStart();
  }

  int id = 0;
  for (std::shared_ptr<vpRBFeatureTracker> &tracker : m_trackers) {
    m_logger.startTimer();
    try {
      tracker->extractFeatures(input, m_previousFrame, m_cMo);
    }
    catch (vpException &) {
      std::cerr << "Tracker " << id << " raised an exception in extractFeatures" << std::endl;
    }
    m_logger.setTrackerFeatureExtractionTime(id, m_logger.endTimer());
    id += 1;
  }
  id = 0;
  for (std::shared_ptr<vpRBFeatureTracker> &tracker : m_trackers) {
    m_logger.startTimer();
    tracker->trackFeatures(input, m_previousFrame, m_cMo);
    m_logger.setTrackerFeatureTrackingTime(id, m_logger.endTimer());
    id += 1;
  }


  id = 0;
  for (std::shared_ptr<vpRBFeatureTracker> &tracker : m_trackers) {
    m_logger.startTimer();
    tracker->initVVS(input, m_previousFrame, m_cMo);
    m_logger.setInitVVSTime(id, m_logger.endTimer());
    //std::cout << "Tracker " << id << " has " << tracker->getNumFeatures() << " features" << std::endl;
    id += 1;
  }

  m_cMoPrev = m_cMo;
  double bestError = std::numeric_limits<double>::max();
  vpHomogeneousMatrix best_cMo = m_cMo;
  double mu = m_muInit;
  for (unsigned int iter = 0; iter < m_vvsIterations; ++iter) {
    id = 0;
    for (std::shared_ptr<vpRBFeatureTracker> &tracker : m_trackers) {
      m_logger.startTimer();
      try {
        tracker->computeVVSIter(input, m_cMo, iter);
      }
      catch (vpException &) {
        std::cerr << "Tracker " << id << " raised an exception in computeVVSIter" << std::endl;
        throw;
      }
      m_logger.addTrackerVVSTime(id, m_logger.endTimer());
      id += 1;
    }

    //! Check if all trackers have converged
    bool converged = true;
    for (std::shared_ptr<vpRBFeatureTracker> &tracker : m_trackers) {
      if (!tracker->vvsHasConverged()) {
        converged = false;
        break;
      }
    }
    if (converged) {
      break;
    }

    vpMatrix LTL(6, 6, 0.0);
    vpColVector LTR(6, 0.0);
    double error = 0.f;
    unsigned int numFeatures = 0;

    for (std::shared_ptr<vpRBFeatureTracker> &tracker : m_trackers) {
      if (tracker->getNumFeatures() > 0) {
        numFeatures += tracker->getNumFeatures();
        const double weight = tracker->getVVSTrackerWeight();
        LTL += weight * tracker->getLTL();
        LTR += weight * tracker->getLTR();
        error += (weight * tracker->getWeightedError()).sumSquare();
        //std::cout << "Error = " << (weight * tracker->getWeightedError()).sumSquare() << std::endl;
      }

    }

    if (numFeatures >= 6) {

      if (error < bestError) {
        bestError = error;
        best_cMo = m_cMo;
      }

      vpMatrix H(6, 6);
      H.eye(6);
      try {
        vpColVector v = -m_lambda * ((LTL + mu * H).pseudoInverse(LTL.getRows() * std::numeric_limits<double>::epsilon()) * LTR);
        m_cMo = vpExponentialMap::direct(v).inverse() * m_cMo;
      }
      catch (vpException &) {
        std::cerr << "Could not compute pseudo inverse" << std::endl;
      }
      mu *= m_muIterFactor;
    }
    else {
      return;
    }
  }

  //m_cMo = best_cMo;

  for (std::shared_ptr<vpRBFeatureTracker> &tracker : m_trackers) {
    tracker->onTrackingIterEnd();
  }
  //m_cMo = m_kalman.filter(m_cMo, 1.0 / 20.0);
#if VP_DEBUG_RB_TRACKER
  std::cout << m_logger << std::endl;
#endif
  if (m_currentFrame.I.getSize() == 0) {
    m_currentFrame = input;
    m_previousFrame = input;
  }
  else {
    m_previousFrame = std::move(m_currentFrame);
    m_currentFrame = std::move(input);
  }
  m_logger.startTimer();
  if (m_driftDetector) {
    m_driftDetector->update(m_previousFrame, m_currentFrame, m_cMo, m_cMoPrev);
  }
  m_logger.setDriftDetectionTime(m_logger.endTimer());
}

void vpRBTracker::updateRender(vpRBFeatureTrackerInput &frame)
{
  m_renderer.setCameraPose(m_cMo.inverse());

  // Update clipping distances
  frame.renders.normals.resize(m_imageHeight, m_imageWidth);
  frame.renders.silhouetteCanny.resize(m_imageHeight, m_imageWidth);
  float clipNear, clipFar;
  m_renderer.computeClipping(clipNear, clipFar);
  frame.renders.zNear = std::max(0.001f, clipNear);
  frame.renders.zFar = clipFar;
  m_rendererSettings.setClippingDistance(frame.renders.zNear, frame.renders.zFar);
  m_renderer.setRenderParameters(m_rendererSettings);

  // For silhouette extraction, update depth difference threshold
  double thresholdValue = m_depthSilhouetteSettings.getThreshold();
  if (m_depthSilhouetteSettings.thresholdIsRelative()) {
    m_renderer.getRenderer<vpPanda3DDepthCannyFilter>()->setEdgeThreshold((frame.renders.zFar - frame.renders.zNear) * thresholdValue);
  }
  else {
    m_renderer.getRenderer<vpPanda3DDepthCannyFilter>()->setEdgeThreshold(thresholdValue);
  }

  // Call Panda renderer
  m_renderer.renderFrame();

  frame.renders.boundingBox = m_renderer.getBoundingBox();

  // Extract data from Panda textures
#ifdef VISP_HAVE_OPENMP
#pragma omp parallel sections
#endif

  {
#ifdef VISP_HAVE_OPENMP
#pragma omp section
#endif
    {
      m_renderer.getRenderer<vpPanda3DGeometryRenderer>()->getRender(frame.renders.normals, frame.renders.depth, frame.renders.boundingBox, m_imageHeight, m_imageWidth);
    }
#ifdef VISP_HAVE_OPENMP
#pragma omp section
#endif
    {
      m_renderer.getRenderer<vpPanda3DDepthCannyFilter>()->getRender(frame.renders.silhouetteCanny, frame.renders.isSilhouette, frame.renders.boundingBox, m_imageHeight, m_imageWidth);
      // m_renderer.placeRenderInto(m_tempRenders.renders.silhouetteCanny, frame.renders.silhouetteCanny, vpRGBf(0.f));
      // m_renderer.placeRenderInto(m_tempRenders.renders.isSilhouette, frame.renders.isSilhouette, (unsigned char)(0));
    }
// #pragma omp section
//     {
//       vpImage<vpRGBa> renders.color;
//       m_renderer.getRenderer<vpPanda3DRGBRenderer>()->getRender(renders.color);
//       m_renderer.placeRendernto(renders.color, frame.renders.color, vpRGBa(0));
//     }
  }

}

std::vector<vpRBSilhouettePoint> vpRBTracker::extractSilhouettePoints(
  const vpImage<vpRGBf> &Inorm, const vpImage<float> &Idepth,
  const vpImage<vpRGBf> &silhouetteCanny, const vpImage<unsigned char> &Ivalid,
  const vpCameraParameters &cam, const vpHomogeneousMatrix &cTcp)
{
  std::vector<std::pair<unsigned int, unsigned int>> candidates =
    m_depthSilhouetteSettings.getSilhouetteCandidates(Ivalid, Idepth, cam, cTcp, m_previousFrame.silhouettePoints, 42);

  std::vector<vpRBSilhouettePoint> points;
  vpColVector norm(3);

  for (unsigned int i = 0; i < candidates.size(); ++i) {
    unsigned int n = candidates[i].first, m = candidates[i].second;
    double theta = silhouetteCanny[n][m].B;
    if (std::isnan(theta)) {
      continue;
    }

    norm[0] = Inorm[n][m].R;
    norm[1] = Inorm[n][m].G;
    norm[2] = Inorm[n][m].B;
    const double l = std::sqrt(norm[0] * norm[0] + norm[1] * norm[1] + norm[2] * norm[2]);

    if (l > 1e-1) {
      const double Z = Idepth[n][m];
      //bool noNeighbor = true;
      // double nx = cos(theta);
      // double ny = sin(theta);
      // const double Zn = Idepth[static_cast<unsigned int>(round(n + ny * 1))][static_cast<unsigned int>(round(m + nx * 2))];
#if VP_DEBUG_RB_TRACKER
      if (fabs(theta) > M_PI + 1e-6) {
        throw vpException(vpException::badValue, "Theta expected to be in -Pi, Pi range but was not");
      }
#endif
      points.push_back(vpRBSilhouettePoint(n, m, norm, theta, Z));
      // if (Zn > 0) {
      //   theta = -theta;
      // }
      // Code to filter when two edges are too close and should not be used
      // for (unsigned int normalOffset = 1; normalOffset <= 3; ++normalOffset) {
      //   unsigned char offY = static_cast<unsigned char>(round(n + normalOffset * ny));
      //   unsigned char offX = static_cast<unsigned char>(round(m + normalOffset * nx));
      //   unsigned char negOffY = static_cast<unsigned char>(round(n - normalOffset * ny));
      //   unsigned char negOffX = static_cast<unsigned char>(round(m - normalOffset * nx));
      //   if (offY == n || offX == m || negOffY == n||negOffX == m) {
      //     continue;
      //   }

      //   if (Ivalid(offY, offX) || Ivalid(negOffY, negOffX)) {
      //     noNeighbor = false;
      //     // std::cout << (unsigned int)(Ivalid(n + normalOffset * ny, m + normalOffset * nx)) << std::endl;
      //     break;
      //   }
      // }
      // if (noNeighbor) {
      //   points.push_back(vpRBSilhouettePoint(n, m, norm, theta, Z));
      // }
    }
  }

  return points;
}

void vpRBTracker::addTracker(std::shared_ptr<vpRBFeatureTracker> tracker)
{
  m_trackers.push_back(tracker);
}

void vpRBTracker::displayMask(vpImage<unsigned char> &Imask) const
{
  if (m_mask) {
    m_mask->display(m_currentFrame.mask, Imask);
  }
}

void vpRBTracker::display(const vpImage<unsigned char> &I, const vpImage<vpRGBa> &IRGB, const vpImage<unsigned char> &depth, const vpRBFeatureDisplayType type)
{
  if (m_currentFrame.renders.normals.getSize() == 0) {
    return;
  }

  // vpRect bb = m_currentFrame.boundingBox;
  // unsigned int bottom = bb.getBottom();
  // for (unsigned int i = bb.getTop(); i < bottom; ++i) {
  //   unsigned int linear_index = i * IRGB.getWidth() + static_cast<unsigned int>(bb.getLeft());
  //   unsigned int stop = linear_index + static_cast<unsigned int>(bb.getWidth());
  //   while (linear_index < stop) {
  //     const vpRGBf &normal = m_currentFrame.renders.normals.bitmap[linear_index];
  //     if (normal.R == 0.f && normal.G == 0.f && normal.B == 0.f) {
  //       ++linear_index;
  //       continue;
  //     }
  //     const vpRGBa rgb = IRGB.bitmap[linear_index];
  //     const float blendFactor = 0.5;
  //     const vpRGBf rgbF = vpRGBf(static_cast<float>(rgb.R), static_cast<float>(rgb.G), static_cast<float>(rgb.B));
  //     vpRGBf blendF = ((normal + vpRGBf(1.f)) * 127.5f) * blendFactor + rgbF * (1.f - blendFactor);
  //     IRGB.bitmap[linear_index] = vpRGBa(static_cast<unsigned char>(blendF.R), static_cast<unsigned char>(blendF.G), static_cast<unsigned char>(blendF.B));
  //     ++linear_index;
  //   }
  // }
  vpDisplay::display(IRGB);

  for (std::shared_ptr<vpRBFeatureTracker> &tracker : m_trackers) {
    tracker->display(m_currentFrame.cam, I, IRGB, depth, type);
  }

  if (m_driftDetector) {
    m_driftDetector->display(IRGB);
  }

  // vpDisplay::displayRectangle(IRGB, m_renderer.getBoundingBox(), vpColor::red);
}

vpObjectCentricRenderer &vpRBTracker::getRenderer()
{
  return m_renderer;
}

#if defined(VISP_HAVE_NLOHMANN_JSON)
void vpRBTracker::loadConfigurationFile(const std::string &filename)
{
  std::ifstream jsonFile(filename);
  if (!jsonFile.good()) {
    throw vpException(vpException::ioError, "Could not read from settings file " + filename + " to initialise the vpMbGenericTracker");
  }
  nlohmann::json settings;
  try {
    settings = nlohmann::json::parse(jsonFile);
  }
  catch (nlohmann::json::parse_error &e) {
    std::stringstream msg;
    msg << "Could not parse JSON file : \n";

    msg << e.what() << std::endl;
    msg << "Byte position of error: " << e.byte;
    throw vpException(vpException::ioError, msg.str());
  }
  loadConfiguration(settings);
  jsonFile.close();
}
void vpRBTracker::loadConfiguration(const nlohmann::json &j)
{
  std::cout << "Loading configuration file" << std::endl;
  m_firstIteration = true;
  nlohmann::json cameraSettings = j.at("camera");
  m_cam = cameraSettings.at("intrinsics");
  m_imageHeight = cameraSettings.value("height", m_imageHeight);
  m_imageWidth = cameraSettings.value("width", m_imageWidth);
  m_rendererSettings.setCameraIntrinsics(m_cam);
  m_rendererSettings.setImageResolution(m_imageHeight, m_imageWidth);
  m_renderer.setRenderParameters(m_rendererSettings);

  std::cout << "Loading object" << std::endl;
  if (j.contains("model")) {
    loadObjectModel(j.at("model"));
  }

  //TODO: Clear Panda3D renderer list?
  std::cout << "Loading vvs settings" << std::endl;
  nlohmann::json vvsSettings = j.at("vvs");
  m_vvsIterations = vvsSettings.value("maxIterations", m_vvsIterations);
  m_lambda = vvsSettings.value("gain", m_lambda);
  m_muInit = vvsSettings.value("mu", m_muInit);
  m_muIterFactor = vvsSettings.value("muIterFactor", m_muIterFactor);



  std::cout << "Loading silhouette extraction settings" << std::endl;
  m_depthSilhouetteSettings = j.at("silhouetteExtractionSettings");


  std::cout << "Loading the different trackers" << std::endl;;
  m_trackers.clear();
  nlohmann::json features = j.at("features");
  vpRBFeatureTrackerFactory &featureFactory = vpRBFeatureTrackerFactory::getFactory();
  for (const nlohmann::json &trackerSettings: features) {
    std::shared_ptr<vpRBFeatureTracker> tracker = featureFactory.buildFromJson(trackerSettings);
    if (tracker == nullptr) {
      throw vpException(vpException::badValue, "Cannot instanciate subtracker with the current settings, make sure that the type is registered. Settings: %s", trackerSettings.dump(2).c_str());
    }
    m_trackers.push_back(tracker);
  }

  if (j.contains("mask")) {
    vpObjectMaskFactory &maskFactory = vpObjectMaskFactory::getFactory();
    nlohmann::json maskSettings = j.at("mask");
    m_mask = maskFactory.buildFromJson(maskSettings);
    if (m_mask == nullptr) {
      throw vpException(vpException::badValue, "Cannot instanciate object mask with the current settings, make sure that the type is registered. Settings: %s", maskSettings.dump(2).c_str());
    }
  }
  if (j.contains("drift")) {
    vpRBDriftDetectorFactory &factory = vpRBDriftDetectorFactory::getFactory();
    nlohmann::json driftSettings = j.at("drift");
    m_driftDetector = factory.buildFromJson(driftSettings);
    if (m_driftDetector == nullptr) {
      throw vpException(vpException::badValue, "Cannot instanciate drift detection with the current settings, make sure that the type is registered in the factory");
    }
  }
}
#endif

#ifdef VISP_HAVE_MODULE_GUI
void vpRBTracker::initClick(const vpImage<unsigned char> &I, const std::string &initFile, bool displayHelp)
{
  vpRBInitializationHelper initializer;
  initializer.setCameraParameters(m_cam);
  initializer.initClick(I, initFile, displayHelp);
  m_cMo = initializer.getPose();
}

#endif
