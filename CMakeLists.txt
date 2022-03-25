cmake_minimum_required (VERSION 3.8)

project ("ReferenceTracker" VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets 3DCore Multimedia REQUIRED)
if (${Qt5_FOUND})
    message("Package Qt trouvé : " ${Qt5_VERSION})
else()
    message("Impossible de trouver le package Qt. Il faut vérifier qu'une version de Qt est dans la variable d'environnement CMAKE_PREFIX_PATH.")
endif()
find_package(OpenCV REQUIRED)

INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )

add_executable (ReferenceTracker
    # Les ressources.
    "resources.qrc"

    # Les données.
    "Data/TrackedPoint.cpp"
    "Data/TrackedPoint.h"

    "Data/Video.h"
    "Data/Video.cpp"

    "Data/Document.cpp" 
    "Data/Document.h"

    # Tracking.
    "Tracking/TrackingManager.h"
    "Tracking/TrackingManager.cpp"

    # Les actions.
    "Actions/TrackedPointCommands.h"
    "Actions/TrackedPointCommands.cpp"

    # L'interface graphique.
    "UI/TypeSafeSettings.h"
    "UI/TypeSafeSettings.cpp"

    "UI/ScrollableGraphicsView.h"
    "UI/ScrollableGraphicsView.cpp"

    "UI/ClickableLabel.h"
    "UI/ClickableLabel.cpp"

    "UI/DynamicSplitter.h" 
    "UI/DynamicSplitter.cpp"

    "UI/VideoPlayer.ui"
    "UI/VideoPlayer.cpp"
    "UI/VideoPlayer.h"

    "UI/TrackedPointsList.h"
    "UI/TrackedPointsList.cpp"

    "UI/GraphView.h"
    "UI/GraphView.cpp"

    "UI/MainWindow.ui"
    "UI/MainWindow.cpp"
    "UI/MainWindow.h"

    # Le fichier de démarrage.
    "main.cpp" "UI/AutomaticTrackingDisplay.h" "UI/AutomaticTrackingDisplay.cpp")

target_link_libraries(ReferenceTracker Qt5::Widgets Qt5::Multimedia Qt5::3DCore)
target_link_libraries(ReferenceTracker ${OpenCV_LIBS})
