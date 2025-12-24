cmake_minimum_required(VERSION 3.14)
project(BringMeTheEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/bin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option pour activer AddressSanitizer
option(SANITIZE_ADDRESS "Enable AddressSanitizer" OFF)
# Option pour compiler les tests
option(BUILD_TESTS "Build tests" ON)

# Rechercher GLFW et GLM via le système
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Ajout de GLAD depuis un dossier externe
add_subdirectory(Bring_me_the_engine/external/glad)

# Récupère tous les fichiers .cpp dans src/
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS Bring_me_the_engine/src/*.cpp)

# Récupère tout ce qu'il faut pour ImGui
file(GLOB IMGUI_SRC
    Bring_me_the_engine/external/imgui/*.cpp
    Bring_me_the_engine/external/imgui/backends/imgui_impl_glfw.cpp
    Bring_me_the_engine/external/imgui/backends/imgui_impl_opengl3.cpp
)

# ========================================
# BIBLIOTHÈQUE DU MOTEUR (sans main)
# ========================================

# Séparer les sources du moteur (sans main) des sources avec main
file(GLOB_RECURSE ALL_ENGINE_SOURCES CONFIGURE_DEPENDS Bring_me_the_engine/src/*.cpp)

# Filtrer pour exclure les fichiers contenant main() de la bibliothèque
set(ENGINE_LIB_SOURCES)
foreach(source ${ALL_ENGINE_SOURCES})
    # Vous devrez peut-être ajuster cette condition selon votre structure
    if(NOT ${source} MATCHES "main\\.cpp$")
        list(APPEND ENGINE_LIB_SOURCES ${source})
    endif()
endforeach()

# Créer une bibliothèque statique pour le moteur (sans main)
add_library(BringMeTheEngine-lib STATIC ${ENGINE_LIB_SOURCES} ${IMGUI_SRC})

add_compile_definitions(_USE_MATH_DEFINES)

add_compile_definitions(PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}/Bring_me_the_engine")


# Include dirs pour la bibliothèque
target_include_directories(BringMeTheEngine-lib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/include
        ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/src
    PRIVATE
        ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/external/glad/include
        ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/external/imgui
        ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/external/imgui/backends
        ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/external/json
)

# Lier les bibliothèques à la lib du moteur
target_link_libraries(BringMeTheEngine-lib
    PUBLIC
        glfw
        glm::glm
    PRIVATE
        glad
)

# ========================================
# EXÉCUTABLE PRINCIPAL
# ========================================

# Créer un fichier main.cpp si nécessaire, ou utiliser un fichier existant
# Pour l'instant, on assume qu'il y a un main dans ENGINE_SOURCES
add_executable(${PROJECT_NAME} ${ENGINE_SOURCES} ${IMGUI_SRC})

# Include dirs pour l'exécutable
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/include
        ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/external/glad/include
        ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/src
        ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/external/imgui
        ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/external/imgui/backends
        ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/external/json
)

# Lier les bibliothèques à l'exécutable
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glad
        glfw
        glm::glm
)

# ========================================
# TESTS (optionnels)
# ========================================

if(BUILD_TESTS)
    # Ajout de Catch2 seulement si on compile les tests
    add_subdirectory(Bring_me_the_engine/external/Catch2-3.8.1)
    
    # Récupérer tous les fichiers de test
    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
    
    # Créer l'exécutable de tests
    add_executable(BringMeTheEngine-tests ${TEST_SOURCES})
    
    # Inclure les headers nécessaires
    target_include_directories(BringMeTheEngine-tests 
        PRIVATE
            ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/src
            ${CMAKE_SOURCE_DIR}/Bring_me_the_engine/include
    )
    
    # Lier la bibliothèque du moteur aux tests
    target_link_libraries(BringMeTheEngine-tests
        PRIVATE
            BringMeTheEngine-lib
            Catch2::Catch2WithMain
    )
    
    # Configuration CTest - mais sans catch_discover_tests pour éviter les problèmes
    include(CTest)
    
    # Ajouter un test simple sans découverte automatique
    add_test(NAME BringMeTheEngine-unit-tests COMMAND BringMeTheEngine-tests)
    
    # Si vous voulez utiliser catch_discover_tests plus tard, décommentez ceci
    # include(Catch)
    # catch_discover_tests(BringMeTheEngine-tests)
endif()

# ========================================
# CONFIGURATION ADDRESSSANITIZER
# ========================================

if(SANITIZE_ADDRESS)
    message(STATUS "🔍 AddressSanitizer activé")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        set(SANITIZE_FLAGS "-fsanitize=address" "-g")
        
        # Appliquer à l'exécutable principal
        target_compile_options(${PROJECT_NAME} PRIVATE ${SANITIZE_FLAGS})
        target_link_options(${PROJECT_NAME} PRIVATE "-fsanitize=address")
        
        # Appliquer à la bibliothèque
        target_compile_options(BringMeTheEngine-lib PRIVATE ${SANITIZE_FLAGS})
        target_link_options(BringMeTheEngine-lib PRIVATE "-fsanitize=address")
        
        # Appliquer aux tests si ils sont compilés
        if(BUILD_TESTS)
            target_compile_options(BringMeTheEngine-tests PRIVATE ${SANITIZE_FLAGS})
            target_link_options(BringMeTheEngine-tests PRIVATE "-fsanitize=address")
        endif()
    else()
        message(WARNING "AddressSanitizer n'est pas pris en charge par ce compilateur.")
    endif()
endif()