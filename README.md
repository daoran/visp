<h1>ViSP: Open source Visual Servoing Platform</h1>

[![Github Releases](https://img.shields.io/github/release/lagadic/visp.svg)](https://github.com/lagadic/visp/releases)
[![License](https://eddelbuettel.github.io/badges/GPL2+.svg)](https://opensource.org/license/gpl-2-0/)

Platform | Build Status |
-------- | ------------ |
Ubuntu 20.04, 22.04 (amd64)| [![ubuntu dep apt workflow](https://github.com/lagadic/visp/actions/workflows/ubuntu-dep-apt.yml/badge.svg)](https://github.com/lagadic/visp/actions/workflows/ubuntu-dep-apt.yml) [![ubuntu dep src workflow](https://github.com/lagadic/visp/actions/workflows/ubuntu-dep-src.yml/badge.svg)](https://github.com/lagadic/visp/actions/workflows/ubuntu-dep-src.yml)
macOS 13 and 14 | [![macos workflow](https://github.com/lagadic/visp/actions/workflows/macos.yml/badge.svg)](https://github.com/lagadic/visp/actions/workflows/macos.yml)
iOS on macOS 11.0| [![ios workflow](https://github.com/lagadic/visp/actions/workflows/ios.yml/badge.svg)](https://github.com/lagadic/visp/actions/workflows/ios.yml)
Windows 10 | [![Build status](https://ci.appveyor.com/api/projects/status/121dscdkryf5dbn0/branch/master?svg=true)](https://ci.appveyor.com/project/fspindle/visp/branch/master)
Other arch Ubuntu 22.04 (aarch64, s390x)| [![other arch workflow](https://github.com/lagadic/visp/actions/workflows/other-arch.yml/badge.svg)](https://github.com/lagadic/visp/actions/workflows/other-arch.yml)
ROS2 Humble Ubuntu 22.04 Jammy| [![Build Status](https://build.ros2.org/buildStatus/icon?job=Hdev__visp__ubuntu_jammy_amd64)](https://build.ros2.org/job/Hdev__visp__ubuntu_jammy_amd64/)
ROS2 Jazzy Ubuntu 24.04 Noble| [![Build Status](https://build.ros2.org/buildStatus/icon?job=Jdev__visp__ubuntu_noble_amd64)](https://build.ros2.org/job/Jdev__visp__ubuntu_noble_amd64/)
ROS2 Kilted Ubuntu 24.04 Noble| [![Build Status](https://build.ros2.org/buildStatus/icon?job=Kdev__visp__ubuntu_noble_amd64)](https://build.ros2.org/job/Kdev__visp__ubuntu_noble_amd64/)
ROS2 Rolling Ubuntu 24.04 Noble| [![Build Status](https://build.ros2.org/buildStatus/icon?job=Rdev__visp__ubuntu_noble_amd64)](https://build.ros2.org/job/Rdev__visp__ubuntu_noble_amd64)
Valgrind | [![valgrind workflow](https://github.com/lagadic/visp/actions/workflows/valgrind.yml/badge.svg)](https://github.com/lagadic/visp/actions/workflows/valgrind.yml)
Sanitizer | [![sanitizers workflow](https://github.com/lagadic/visp/actions/workflows/ubuntu-sanitizers.yml/badge.svg)](https://github.com/lagadic/visp/actions/workflows/ubuntu-sanitizers.yml)
Code coverage | [![Code coverage](https://codecov.io/gh/lagadic/visp/branch/master/graph/badge.svg?token=GQIiKbA3BC)](https://codecov.io/gh/lagadic/visp)

Other projects | Build Status |
-------------- | ------------ |
[UsTK](https://github.com/lagadic/ustk) | [![Ubuntu](https://github.com/lagadic/visp/actions/workflows/ubuntu-ustk.yml/badge.svg)](https://github.com/lagadic/visp/actions/workflows/ubuntu-ustk.yml)
[visp_contrib](https://github.com/lagadic/visp_contrib) | [![Ubuntu](https://github.com/lagadic/visp/actions/workflows/ubuntu-contrib.yml/badge.svg)](https://github.com/lagadic/visp/actions/workflows/ubuntu-contrib.yml)
[visp_sample](https://github.com/lagadic/visp_sample) | [![macos workflow](https://github.com/lagadic/visp/actions/workflows/macos.yml/badge.svg)](https://github.com/lagadic/visp/actions/workflows/macos.yml) [![ubuntu dep apt workflow](https://github.com/lagadic/visp/actions/workflows/ubuntu-dep-apt.yml/badge.svg)](https://github.com/lagadic/visp/actions/workflows/ubuntu-dep-apt.yml)
[camera_localization](https://github.com/lagadic/camera_localization) | [![ubuntu_3rdparty_workflow](https://github.com/lagadic/visp/actions/workflows/ubuntu-3rdparty.yml/badge.svg)](https://github.com/lagadic/visp/actions/workflows/ubuntu-3rdparty.yml)
[visp_started](https://github.com/lagadic/visp_started) | [![ubuntu_3rdparty_workflow](https://github.com/lagadic/visp/actions/workflows/ubuntu-3rdparty.yml/badge.svg)](https://github.com/lagadic/visp/actions/workflows/ubuntu-3rdparty.yml)


ViSP is a cross-platform library (Linux, Windows, MacOS, iOS, Android) that allows prototyping and developing applications using visual tracking and visual servoing technics at the heart of the researches done now by Inria <a href="https://team.inria.fr/rainbow">Rainbow team</a> and before 2018 by <a href="https://team.inria.fr/lagadic">Lagadic team</a>. ViSP is able to compute control laws that can be applied to robotic systems. It provides a set of visual features that can be tracked using real time image processing or computer vision algorithms. ViSP provides also simulation capabilities. ViSP can be useful in robotics, computer vision, augmented reality and computer animation. Our <a href="https://www.youtube.com/user/VispTeam">YouTube channel</a> gives an overview of the applications that could be tackled.

#### Citing ViSP
Please cite <a href="https://inria.hal.science/inria-00351899">ViSP</a> in your publications if it helps your research:
```
@article{Marchand05b,
   Author = {Marchand, E. and Spindler, F. and Chaumette, F.},
   Title = {ViSP for visual servoing: a generic software platform with a wide class of robot control skills},
   Journal = {IEEE Robotics and Automation Magazine},
   Volume = {12},
   Number = {4},
   Pages = {40--52},
   Publisher = {IEEE},
   Month = {December},
   Year = {2005}
}
```
To cite the <a href="https://inria.hal.science/hal-01853972v1">generic model-based tracker</a>:
```
@InProceedings{Trinh18a,
   Author = {Trinh, S. and Spindler, F. and Marchand, E. and Chaumette, F.},
   Title = {A modular framework for model-based visual tracking using edge, texture and depth features},
   BookTitle = {{IEEE/RSJ Int. Conf. on Intelligent Robots and Systems, IROS'18}},
   Address = {Madrid, Spain},
   Month = {October},
   Year = {2018}
}
```
To cite <a href="https://hal.science/hal-01246370v1">pose estimation algorithms and hands-on survey</a> illustrated with <a href="https://github.com/lagadic/camera_localization">ViSP examples</a>:
```
@article{Marchand16a,
   Author = {Marchand, E. and Uchiyama, H. and Spindler, F.},
   Title = {Pose estimation for augmented reality: a hands-on survey},
   Journal = {IEEE Trans. on Visualization and Computer Graphics},
   Volume = {22},
   Number = {12},
   Pages = {2633--2651},
   Month = {December},
   Year = {2016}
}
```

#### Resources
- Homepage: https://visp.inria.fr
- Wiki: https://github.com/lagadic/visp/wiki
- Code documentation: https://visp-doc.inria.fr/doxygen/visp-daily
- Q&A forum: https://github.com/lagadic/visp/discussions
- Issue tracking: https://github.com/lagadic/visp/issues
- YouTube: https://www.youtube.com/user/VispTeam

#### Contributing

Please read before starting work on a pull request: https://visp.inria.fr/contributing-code/
