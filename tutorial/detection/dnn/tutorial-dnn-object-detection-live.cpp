//! \example tutorial-dnn-object-detection-live.cpp
#include <visp3/core/vpConfig.h>
#include <visp3/detection/vpDetectorDNNOpenCV.h>
#include <visp3/core/vpIoTools.h>
#include <visp3/gui/vpDisplayGDI.h>
#include <visp3/gui/vpDisplayOpenCV.h>
#include <visp3/gui/vpDisplayX.h>

typedef enum ChosenDetectionContainer
{
  MAP    = 0,
  VECTOR = 1,
  BOTH   = 2,
  COUNT  = 3
}ChosenDetectionContainer;

std::string chosenDetectionContainerToString(const ChosenDetectionContainer& choice)
{
  switch(choice)
  {
    case MAP: 
      return "map";
    case VECTOR:
      return "vector";
    case BOTH:
      return "both";
    default:
      break;
  }
  return "unknown";
}

ChosenDetectionContainer chosenDetectionContainerFromString(const std::string& choiceStr)
{
  ChosenDetectionContainer choice(COUNT);
  bool hasFoundMatch = false;
  for(unsigned int i = 0; i < ChosenDetectionContainer::COUNT && !hasFoundMatch; i++)
  {
    ChosenDetectionContainer candidate = (ChosenDetectionContainer)i;
    hasFoundMatch = (chosenDetectionContainerToString(candidate) == vpIoTools::toLowerCase(choiceStr));
    if(hasFoundMatch)
    {
      choice = candidate;
    }
  }
  return choice;
}

std::string getAvailableDetectionContainer()
{
  std::string availableContainers("< ");
  for(unsigned int i = 0; i < ChosenDetectionContainer::COUNT - 1; i++)
  {
    std::string name = chosenDetectionContainerToString((ChosenDetectionContainer) i);
    availableContainers += name + " , ";
  }
  availableContainers += chosenDetectionContainerToString((ChosenDetectionContainer) (ChosenDetectionContainer::COUNT - 1)) + " >";
  return availableContainers;
}

int main(int argc, const char *argv[])
{
#if (VISP_HAVE_OPENCV_VERSION >= 0x030403) && defined(VISP_HAVE_OPENCV_DNN) && (VISP_CXX_STANDARD >= VISP_CXX_STANDARD_17)
  try {
    int opt_device = 0;
    std::string input = "";
    //! [OpenCV DNN face detector]
    std::string model = "opencv_face_detector_uint8.pb";
    std::string config = "opencv_face_detector.pbtxt";
    std::string framework = "tensorflow";
    std::string labelFile = "";
    vpDetectorDNNOpenCV::DNNResultsParsingType type = vpDetectorDNNOpenCV::RESNET_10;
    //! [OpenCV DNN face detector]
    int inputWidth = 300, inputHeight = 300;
    double meanR = 104.0, meanG = 177.0, meanB = 123.0;
    double scaleFactor = 1.0;
    bool swapRB = false;
    bool hasToWaitClick = false;
    float confThresh = 0.5f;
    float nmsThresh = 0.4f;
    double detectionFilter = 0.25;
    ChosenDetectionContainer containerType = ChosenDetectionContainer::MAP;
    bool verbose = false;

    for (int i = 1; i < argc; i++) {
      if (std::string(argv[i]) == "--device" && i + 1 < argc) {
        opt_device = atoi(argv[++i]);
      } else if (std::string(argv[i]) == "--waitForClick") {
        hasToWaitClick = true;
      } else if (std::string(argv[i]) == "--input" && i + 1 < argc) {
        input = std::string(argv[++i]);
      } else if (std::string(argv[i]) == "--model" && i + 1 < argc) {
        model = std::string(argv[++i]);
      } else if (std::string(argv[i]) == "--type" && i + 1 < argc) {
        type =  vpDetectorDNNOpenCV::dnnResultsParsingTypeFromString(std::string(argv[++i]));
      } else if (std::string(argv[i]) == "--config" && i + 1 < argc) {
        config = std::string(argv[++i]);
        if(config.find("none") != std::string::npos)
        {
          config = std::string();
        }
      } else if (std::string(argv[i]) == "--framework" && i + 1 < argc) {
        framework = std::string(argv[++i]);
        if(framework.find("none") != std::string::npos)
        {
          framework = std::string();
        }
      } else if (std::string(argv[i]) == "--width" && i + 1 < argc) {
        inputWidth = atoi(argv[++i]);
      } else if (std::string(argv[i]) == "--height" && i + 1 < argc) {
        inputHeight = atoi(argv[++i]);
      } else if (std::string(argv[i]) == "--mean" && i + 3 < argc) {
        meanR = atof(argv[++i]);
        meanG = atof(argv[++i]);
        meanB = atof(argv[++i]);
      } else if (std::string(argv[i]) == "--scale" && i + 1 < argc) {
        scaleFactor = atof(argv[++i]);
      } else if (std::string(argv[i]) == "--swapRB") {
        swapRB = true;
      } else if (std::string(argv[i]) == "--confThresh" && i + 1 < argc) {
        confThresh = (float)atof(argv[++i]);
      } else if (std::string(argv[i]) == "--nmsThresh" && i + 1 < argc) {
        nmsThresh = (float)atof(argv[++i]);
      } else if (std::string(argv[i]) == "--filterThresh" && i + 1 < argc) {
        detectionFilter = atof(argv[++i]);
      } else if (std::string(argv[i]) == "--labels" && i + 1 < argc) {
        labelFile = std::string(argv[++i]);
      } else if (std::string(argv[i]) == "--container" && i + 1 < argc) {
        containerType = chosenDetectionContainerFromString(std::string(argv[++i]));
      } else if (std::string(argv[i]) == "--verbose" || std::string(argv[i]) == "-v") {
        verbose = true;
      } else if (std::string(argv[i]) == "--help" || std::string(argv[i]) == "-h") {
        std::cout << argv[0]
                  << " --device <camera device number> --waitForClick --input <path to image or video>"
                     " (camera is used if input is empty) --model <path to net trained weights>"
                     " --type <type of DNN in " + vpDetectorDNNOpenCV::getAvailableDnnResultsParsingTypes() +
                     "> --config <path to net config file or \"none\" not to use one> --framework <framework name or \"none\" not to specify one>"
                     " --width <blob width> --height <blob height>"
                     " --mean <meanR meanG meanB> --scale <scale factor>"
                     " --swapRB --confThresh <confidence threshold>"
                     " --nmsThresh <NMS threshold> --filterThresh <threshold > 0., 0. to disable> --labels <path to label file>"
                     " --container " + getAvailableDetectionContainer() + "--verbose --help"
                  << std::endl;
        return EXIT_SUCCESS;
      }
    }

    std::cout << "Model                : " << model << std::endl;
    std::cout << "Type                 : " << vpDetectorDNNOpenCV::dnnResultsParsingTypeToString(type) << std::endl;
    std::cout << "Config               : " << (config.empty() ? "\"None\"" : config) << std::endl;
    std::cout << "Framework            : " << (framework.empty() ? "\"None\"" : framework) << std::endl;
    std::cout << "Label file (optional): " << (labelFile.empty() ? "None" : labelFile)  << std::endl;
    std::cout << "Width x Height       : " << inputWidth << " x " << inputHeight << std::endl;
    std::cout << "Mean                 : " << meanR << ", " << meanG << ", " << meanB << std::endl;
    std::cout << "Scale                : " << scaleFactor << std::endl;
    std::cout << "Swap RB?             : " << swapRB << std::endl;
    std::cout << "Confidence threshold : " << confThresh << std::endl;
    std::cout << "NMS threshold        : " << nmsThresh << std::endl;
    std::cout << "Filter threshold     : " << (detectionFilter > std::numeric_limits<double>::epsilon() ? std::to_string(detectionFilter) : "disabled") << std::endl;

    cv::VideoCapture capture;
    bool hasCaptureOpeningSucceeded;
    if (input.empty()) {
      hasCaptureOpeningSucceeded = capture.open(opt_device);
    } else {
      hasCaptureOpeningSucceeded = capture.open(input);
    }

    if(!hasCaptureOpeningSucceeded)
    {
      std::cout << "Capture from camera #" <<  (input.empty() ? std::to_string(opt_device) : input) << " didn't work" << std::endl;
      return 1;
    }

    vpImage<vpRGBa> I;
#if defined(VISP_HAVE_X11)
    vpDisplayX d;
#elif defined(VISP_HAVE_GDI)
    vpDisplayGDI d;
#elif defined(VISP_HAVE_OPENCV)
    vpDisplayOpenCV d;
#endif
    d.setDownScalingFactor(vpDisplay::SCALE_AUTO);

    if (! labelFile.empty() && !vpIoTools::checkFilename(labelFile)) {
      throw(vpException(vpException::fatalError, "The file containing the classes labels \"" + labelFile + "\" does not exist !"));
    }

    //! [DNN params]
    vpDetectorDNNOpenCV::NetConfig netConfig(confThresh, nmsThresh, labelFile, cv::Size(inputWidth, inputHeight), detectionFilter);
    vpDetectorDNNOpenCV dnn(netConfig, type);
    dnn.readNet(model, config, framework);
    dnn.setMean(meanR, meanG, meanB);
    dnn.setScaleFactor(scaleFactor);
    dnn.setSwapRB(swapRB);
    //! [DNN params]

    cv::Mat frame;
    while (true) {
      capture >> frame;
      if (frame.empty())
        break;

      if (I.getSize() == 0) {
        vpImageConvert::convert(frame, I);
        d.init(I);
        vpDisplay::setTitle(I, "DNN object detection");
        if (verbose) {
          std::cout << "Process image: " << I.getWidth() << " x " << I.getHeight() << std::endl;
        }
      } else {
        vpImageConvert::convert(frame, I);
      }
      if (verbose) {
        std::cout << "Process new image" << std::endl;
      }
      
      vpDisplay::display(I);
      

      if(containerType == ChosenDetectionContainer::MAP || containerType == ChosenDetectionContainer::BOTH)
      {
        double t = vpTime::measureTimeMs();
        //! [DNN object detection map mode]
        std::map<std::string, std::vector<vpDetectorDNNOpenCV::DetectedFeatures2D>> detections;
        dnn.detect(frame, detections);
        //! [DNN object detection map mode]
        t = vpTime::measureTimeMs() - t;

        //! [DNN class ids and confidences map mode]
        for( auto key_val : detections)
        {
          if (verbose) {
            std::cout << "  Class name      : " << key_val.first << std::endl;
          }
          for(vpDetectorDNNOpenCV::DetectedFeatures2D detection : key_val.second )
          {
            if (verbose) {
              std::cout << "  Bounding box    : " << detection.getBoundingBox() << std::endl;
              std::cout << "  Class Id        : " << detection.getClassId() << std::endl;
              if (detection.getClassName())
                std::cout << "  Class name      : " << detection.getClassName().value() << std::endl;
              std::cout << "  Confidence score: " << detection.getConfidenceScore() << std::endl;
            }
            detection.display(I);
          }
        }
        //! [DNN class ids and confidences map mode]

        std::ostringstream oss_map;
        oss_map << "Detection time (map): " << t << " ms";
        if (verbose) {
          // Displaying timing result in console
          std::cout << "  " << oss_map.str() << std::endl;
        }
        // Displaying timing result on the image
        vpDisplay::displayText(I, 60, 20, oss_map.str(), vpColor::red);
      }
      
      if(containerType == ChosenDetectionContainer::VECTOR || containerType == ChosenDetectionContainer::BOTH)
      {
        double t_vector = vpTime::measureTimeMs();
        //! [DNN object detection vector mode]
        std::vector<vpDetectorDNNOpenCV::DetectedFeatures2D> detections_vec;
        dnn.detect(frame, detections_vec);
        //! [DNN object detection vector mode]
        t_vector = vpTime::measureTimeMs() - t_vector;

        //! [DNN class ids and confidences vector mode]
        for( auto detection : detections_vec)
        {
          if (verbose) {
            std::cout << "  Bounding box    : " << detection.getBoundingBox() << std::endl;
            std::cout << "  Class Id        : " << detection.getClassId() << std::endl;
            std::optional<std::string> classname_opt = detection.getClassName();
            std::cout << "  Class name      : " << (classname_opt ? *classname_opt : "Not known") << std::endl;
            std::cout << "  Confidence score: " << detection.getConfidenceScore() << std::endl;
          }
          detection.display(I);
        }
        //! [DNN class ids and confidences vector mode]

        std::ostringstream oss_vec;
        oss_vec << "Detection time (vector): " << t_vector << " ms";
        if (verbose) {
          // Displaying timing result in console
          std::cout << "  " << oss_vec.str() << std::endl;
        }
        // Displaying timing result on the image
        vpDisplay::displayText(I, 80, 20, oss_vec.str(), vpColor::red);
      }
      
      // // UI display
      if(hasToWaitClick)
      {
        // hasToWaitClick => we are displaying images one by one
        vpDisplay::displayText(I, 20, 20, "Left click to display next image", vpColor::red);
      }
      vpDisplay::displayText(I, 40, 20, "Right click to quit", vpColor::red);
      
      vpDisplay::flush(I);
      vpMouseButton::vpMouseButtonType button;

      if (vpDisplay::getClick(I, button, hasToWaitClick))
      {
        if (button == vpMouseButton::button1)
        {
          // Left click => next image
          continue;
        }
        else if(button == vpMouseButton::button3)
        {
          // Right click => stop the program
          break;
        }
      }
    }

  } catch (const vpException &e) {
    std::cout << e.what() << std::endl;
  }
#else
  (void)argc;
  (void)argv;
#endif
  return EXIT_SUCCESS;
}
