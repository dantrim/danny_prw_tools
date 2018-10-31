cmake_minimum_required( VERSION 3.2 FATAL_ERROR )

atlas_subdir( inc )

atlas_depends_on_subdirs(
    PRIVATE
    Control/AthToolSupport/AsgTools
    Control/xAODRootAccess
    Event/xAOD/xAODEventInfo
    Event/xAOD/xAODCore
    PhysicsAnalysis/AnalysisCommon/PileupReweighting
    PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
    Tools/PathResolver
)

find_package( ROOT REQUIRED COMPONENTS RIO Hist Tree Net Core GPad)
set(_common
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES}
    xAODRootAccess
    PileupReweightingLib
    xAODEventInfo
    AsgTools
    PathResolver
)

atlas_add_executable( run_mu util/mu_looper.cxx ${_common})
