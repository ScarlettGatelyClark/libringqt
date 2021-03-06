CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

PROJECT(ringqt)
SET(GENERIC_LIB_VERSION "1.0.0")
SET(QT_MIN_VERSION  "5.9.0" )

IF(POLICY CMP0053)
   CMAKE_POLICY(SET CMP0053 NEW)
ENDIF(POLICY CMP0053)

IF(POLICY CMP0022)
   CMAKE_POLICY(SET CMP0022 NEW)
ENDIF(POLICY CMP0022)

IF(POLICY CMP0043)
   CMAKE_POLICY(SET CMP0043 NEW)
ENDIF(POLICY CMP0043)

IF(POLICY CMP0020)
   CMAKE_POLICY(SET CMP0020 NEW)
ENDIF(POLICY CMP0020)

IF(POLICY CMP0063)
   CMAKE_POLICY(SET CMP0063 NEW)
ENDIF(POLICY CMP0063)

IF(POLICY CMP0023)
  CMAKE_POLICY(SET CMP0023 OLD)
ENDIF(POLICY CMP0023)

INCLUDE(GNUInstallDirs)
INCLUDE(CMakePackageConfigHelpers)
INCLUDE(GenerateExportHeader)
INCLUDE(CheckLibraryExists)

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_AUTOMOC TRUE)

OPTION(BUILD_SHARED_LIBS   "Build the shared library (instead of static)" ON )
OPTION(ENABLE_VIDEO        "Enable video support"                         ON )
OPTION(MUTE_DRING          "Do not print libring debug output"            OFF)
OPTION(VERBOSE_IPC         "Print all dring function calls (for debug)"   OFF)
OPTION(ENABLE_TEST_ASSERTS "Enable extra asserts (cpu intensive)"         OFF)
OPTION(USE_STATIC_LIBRING  "Always prefer the static libring (buggy)"     OFF)

# DBus is the default on Linux, LibRing on anything else
IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   OPTION(ENABLE_LIBWRAP    "Disable the daemon and use libring directly"  OFF)
   OPTION(DISABLE_EXPORT    "Do not install anything"                      OFF)
ELSE()
   OPTION(ENABLE_LIBWRAP    "Disable the daemon and use libring directly"  ON )
   OPTION(DISABLE_EXPORT    "Do not install anything"                      ON )
ENDIF()

# First, check is the compiler is new enough, most version of Clang are fine
# until problems arise, checking for GCC is enough
IF (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5)
   MESSAGE(FATAL_ERROR "Your version of GCC is too old, please install GCC 5 or later")
ENDIF()

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

FIND_PACKAGE(Ring REQUIRED)
FIND_PACKAGE(Qt5Core ${QT_MIN_VERSION} REQUIRED)
FIND_PACKAGE(Qt5LinguistTools QUIET   ) # translations

IF(NOT ENABLE_LIBWRAP)
   FIND_PACKAGE(Qt5DBus REQUIRED)
ENDIF()

# Make sure that the d_ptr work as intended
SET(CMAKE_C_VISIBILITY_PRESET hidden)
SET(CMAKE_CXX_VISIBILITY_PRESET hidden)
SET(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# Always print as many warning as possible
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/extrawarnings.cmake)

# Skip the daemon and use LibRing directly (require adding signal and slots support)
IF(ENABLE_LIBWRAP)
   INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/wraplibring.cmake)
ENDIF()

IF(ENABLE_VIDEO)
   MESSAGE(STATUS "Video enabled")
   add_definitions( -DENABLE_VIDEO=true )
ELSE()
   MESSAGE(STATUS "Video disabled")
ENDIF()

#File to compile
SET( libringqt_LIB_SRCS
  ${libringqt_LIB_SRCS}

  #Data objects
  src/call.cpp
  src/uri.cpp
  src/ringdevice.cpp
  src/account.cpp
  src/address.cpp
  src/credential.cpp
  src/person.cpp
  src/contactmethod.cpp
  src/numbercategory.cpp
  src/macro.cpp
  src/collectionextensioninterface.cpp
  src/video/rate.cpp
  src/video/device.cpp
  src/video/renderer.cpp
  src/certificate.cpp
  src/securityflaw.cpp
  src/ringtone.cpp
  src/infotemplate.cpp
  src/contactrequest.cpp
  src/presencestatus.cpp
  src/media/media.cpp
  src/media/audio.cpp
  src/media/video.cpp
  src/media/text.cpp
  src/media/file.cpp
  src/media/recording.cpp
  src/media/avrecording.cpp
  src/media/attachment.cpp
  src/media/textrecording.cpp
  src/media/mimemessage.cpp
  src/media/availabilitytracker.cpp

  #Models
  src/bootstrapmodel.cpp
  src/individualeditor.cpp
  src/ringdevicemodel.cpp
  src/accountmodel.cpp
  src/availableaccountmodel.cpp
  src/callmodel.cpp
  src/categorizedhistorymodel.cpp
  src/categorizedbookmarkmodel.cpp
  src/credentialmodel.cpp
  src/categorizedcontactmodel.cpp
  src/useractionmodel.cpp
  src/presencestatusmodel.cpp
  src/phonedirectorymodel.cpp
  src/historytimecategorymodel.cpp
  src/numbercategorymodel.cpp
  src/macromodel.cpp
  src/keyexchangemodel.cpp
  src/tlsmethodmodel.cpp
  src/protocolmodel.cpp
  src/numbercompletionmodel.cpp
  src/profilemodel.cpp
  src/ringtonemodel.cpp
  src/infotemplatemanager.cpp
  src/eventmodel.cpp
  src/securityevaluationmodel.cpp
  src/personmodel.cpp
  src/collectionmodel.cpp
  src/collectionextensionmodel.cpp
  src/collectionmanagerinterface.cpp
  src/networkinterfacemodel.cpp
  src/certificatemodel.cpp
  src/ciphermodel.cpp
  src/accountstatusmodel.cpp
  src/codecmodel.cpp
  src/chainoftrustmodel.cpp
  src/pendingcontactrequestmodel.cpp
  src/peerstimelinemodel.cpp
  src/individualtimelinemodel.cpp
  src/video/devicemodel.cpp
  src/video/sourcemodel.cpp
  src/video/channel.cpp
  src/video/resolution.cpp
  src/video/configurationproxy.cpp
  src/audio/alsapluginmodel.cpp
  src/audio/inputdevicemodel.cpp
  src/audio/managermodel.cpp
  src/audio/outputdevicemodel.cpp
  src/audio/ringtonedevicemodel.cpp
  src/audio/settings.cpp
  src/media/recordingmodel.cpp
  src/bannedcontactmodel.cpp
  src/individual.cpp

  # iCal/vCard/WebDav collections
  src/libcard/event.cpp
  src/libcard/calendar.cpp
  src/libcard/eventaggregate.cpp
  src/libcard/historyimporter.cpp
  src/libcard/private/icsloader.cpp
  src/libcard/private/icsbuilder.cpp

  # Error handling requiring user intervention
  src/troubleshoot/base.cpp
  src/troubleshoot/dispatcher.cpp
  src/troubleshoot/handshake.cpp
  src/troubleshoot/videostuck.cpp
  src/troubleshoot/generic.cpp
  src/troubleshoot/crequest.cpp
  src/troubleshoot/callstate.cpp
  src/troubleshoot/absent.cpp
  src/troubleshoot/unhold.cpp

  #Data collections
  src/transitionalpersonbackend.cpp
  src/collectioninterface.cpp
  src/collectioneditor.cpp
  src/fallbackpersoncollection.cpp
  src/daemoncertificatecollection.cpp
  src/foldercertificatecollection.cpp
  src/localrecordingcollection.cpp
  src/localtextrecordingcollection.cpp
  src/localhistorycollection.cpp
  src/localprofilecollection.cpp
  src/localmacrocollection.cpp
  src/localbookmarkcollection.cpp
  src/localnameservicecache.cpp
  src/localringtonecollection.cpp
  src/localinfotemplatecollection.cpp
  src/peerprofilecollection2.cpp

  #Communication
  src/dbus/configurationmanager.cpp
  src/dbus/callmanager.cpp
  src/dbus/instancemanager.cpp
  src/dbus/videomanager.cpp
  src/dbus/presencemanager.cpp

  #Default interface implementations
  src/globalinstances.cpp
  src/accountlistcolorizerdefault.cpp
  src/presenceserializerdefault.cpp
  src/pixmapmanipulatordefault.cpp
  src/shortcutcreatordefault.cpp
  src/actionextenderdefault.cpp
  src/dbuserrorhandlerdefault.cpp

  #Other
  src/itemdataroles.cpp # Because namespaced MOC need this as of CMake 3.10
  src/hookmanager.cpp
  src/namedirectory.cpp
  src/itembase.cpp
  src/private/vcardutils.cpp
  src/private/textrecordingcache.cpp
  src/private/textrecordingmodel.cpp
  src/private/videorenderermanager.cpp
  src/video/previewmanager.cpp
  src/private/sortproxies.cpp
  src/private/threadworker.cpp
  src/private/addressmodel.cpp
  src/mime.cpp
  src/smartinfohub.cpp

  #Extension
  src/extensions/presencecollectionextension.cpp
  src/extensions/securityevaluationextension.cpp
)

IF(ENABLE_LIBWRAP)
   SET(libringqt_LIB_SRCS ${libringqt_LIB_SRCS}
      src/private/directrenderer.cpp
   )
ELSE()
   SET(libringqt_LIB_SRCS ${libringqt_LIB_SRCS}
      src/private/shmrenderer.cpp
   )
ENDIF(ENABLE_LIBWRAP)

# Public API
SET( libringqt_LIB_HDRS
  src/account.h
  src/address.h
  src/ringdevice.h
  src/credential.h
  src/accountmodel.h
  src/availableaccountmodel.h
  src/individual.h
  src/call.h
  src/call.hpp
  src/callmodel.h
  src/categorizedhistorymodel.h
  src/person.h
  src/bootstrapmodel.h
  src/individualeditor.h
  src/ringdevicemodel.h
  src/collectioninterface.h
  src/collectioninterface.hpp
  src/categorizedbookmarkmodel.h
  src/credentialmodel.h
  src/categorizedcontactmodel.h
  src/useractionmodel.h
  src/presencestatusmodel.h
  src/presencestatus.h
  src/contactmethod.h
  src/phonedirectorymodel.h
  src/historytimecategorymodel.h
  src/numbercategorymodel.h
  src/keyexchangemodel.h
  src/codecmodel.h
  src/tlsmethodmodel.h
  src/protocolmodel.h
  src/numbercompletionmodel.h
  src/profilemodel.h
  src/numbercategory.h
  src/ringtonemodel.h
  src/infotemplatemanager.h
  src/eventmodel.h
  src/localrecordingcollection.h
  src/localbookmarkcollection.h
  src/localringtonecollection.h
  src/localinfotemplatecollection.h
  src/localmacrocollection.h
  src/localhistorycollection.h
  src/localprofilecollection.h
  src/localtextrecordingcollection.h
  src/peerprofilecollection2.h
  src/securityevaluationmodel.h
  src/certificate.h
  src/personmodel.h
  src/transitionalpersonbackend.h
  src/collectionmodel.h
  src/collectionextensionmodel.h
  src/collectionextensionmodel.hpp
  src/macromodel.h
  src/hookmanager.h
  src/namedirectory.h
  src/uri.h
  src/macro.h
  src/ringtone.h
  src/infotemplate.h
  src/itembase.h
  src/itembase.hpp
  src/mime.h
  src/collectionextensioninterface.h
  src/collectionmanagerinterface.h
  src/collectionmanagerinterface.hpp
  src/networkinterfacemodel.h
  src/certificatemodel.h
  src/ciphermodel.h
  src/accountstatusmodel.h
  src/collectionmediator.h
  src/collectionmediator.hpp
  src/collectioneditor.h
  src/collectioneditor.hpp
  src/fallbackpersoncollection.h
  src/daemoncertificatecollection.h
  src/foldercertificatecollection.h
  src/securityflaw.h
  src/collectioncreationinterface.h
  src/collectionconfigurationinterface.h
  src/peerstimelinemodel.h
  src/individualtimelinemodel.h
  src/chainoftrustmodel.h
  src/pendingcontactrequestmodel.h
  src/contactrequest.h
  src/globalinstances.h
  src/accountlistcolorizerdefault.h
  src/presenceserializerdefault.h
  src/pixmapmanipulatordefault.h
  src/shortcutcreatordefault.h
  src/dbuserrorhandlerdefault.h
  src/itemdataroles.h
  src/smartinfohub.h
  src/usagestatistics.h
  src/bannedcontactmodel.h
)

SET(libringqt_video_LIB_HDRS
  src/video/device.h
  src/video/devicemodel.h
  src/video/sourcemodel.h
  src/video/renderer.h
  src/video/resolution.h
  src/video/channel.h
  src/video/rate.h
  src/video/previewmanager.h
  src/video/configurationproxy.h
  #The renderer implementations are not exported on purpose
)

SET(libringqt_audio_LIB_HDRS
  src/audio/alsapluginmodel.h
  src/audio/inputdevicemodel.h
  src/audio/managermodel.h
  src/audio/outputdevicemodel.h
  src/audio/ringtonedevicemodel.h
  src/audio/settings.h
)

SET(libringqt_extensions_LIB_HDRS
  src/extensions/presencecollectionextension.h
  src/extensions/securityevaluationextension.h
)

SET(libringqt_media_LIB_HDRS
  src/media/media.h
  src/media/audio.h
  src/media/video.h
  src/media/text.h
  src/media/file.h
  src/media/attachment.h
  src/media/recording.h
  src/media/avrecording.h
  src/media/textrecording.h
  src/media/mimemessage.h
  src/media/recordingmodel.h
  src/media/availabilitytracker.h
)

SET(libringqt_card_LIB_HDRS
  src/libcard/flagutils.h
  src/libcard/event.h
  src/libcard/calendar.h
  src/libcard/eventaggregate.h
  src/libcard/historyimporter.h
)

SET(libringqt_troubleshoot_LIB_HDRS
  src/troubleshoot/dispatcher.h
  src/troubleshoot/base.h
)

SET(libringqt_interface_LIB_HDRS
  src/interfaces/accountlistcolorizeri.h
  src/interfaces/contactmethodselectori.h
  src/interfaces/presenceserializeri.h
  src/interfaces/itemmodelstateserializeri.h
  src/interfaces/pixmapmanipulatori.h
  src/interfaces/shortcutcreatori.h
  src/interfaces/actionextenderi.h
  src/interfaces/dbuserrorhandleri.h
)

SET( libringqt_extra_LIB_HDRS
  src/typedefs.h
)

# Build all dbus IPC interfaces
IF(NOT ENABLE_LIBWRAP)

   # Build dbus interfaces
   IF(DEFINED RING_XML_INTERFACES_DIR)
      SET (dbus_xml_introspecs_path ${RING_XML_INTERFACES_DIR})
   ELSEIF(EXISTS "${RING_BUILD_DIR}/../bin/dbus")
      SET (dbus_xml_introspecs_path ${RING_BUILD_DIR}/../bin/dbus)
   ELSE()
      SET (dbus_xml_introspecs_path ${CMAKE_INSTALL_PREFIX}/share/dbus-1/interfaces)
   ENDIF()

   # presence manager interface
   SET ( presencemanager_xml  ${dbus_xml_introspecs_path}/cx.ring.Ring.PresenceManager.xml )

   SET( dbus_metatype_path "${CMAKE_CURRENT_SOURCE_DIR}/src/dbus/metatypes.h")

   SET_SOURCE_FILES_PROPERTIES(
      ${presencemanager_xml}
      PROPERTIES
      CLASSNAME PresenceManagerInterface
      INCLUDE ${dbus_metatype_path}
   )

   QT5_ADD_DBUS_INTERFACE(
      libringqt_LIB_SRCS
      ${presencemanager_xml}
      presencemanager_dbus_interface
   )

   # configuration manager interface
   SET ( configurationmanager_xml  ${dbus_xml_introspecs_path}/cx.ring.Ring.ConfigurationManager.xml )

   SET_SOURCE_FILES_PROPERTIES(
      ${configurationmanager_xml}
      PROPERTIES
      CLASSNAME ConfigurationManagerInterface
      INCLUDE ${dbus_metatype_path}
   )

   QT5_ADD_DBUS_INTERFACE(
      libringqt_LIB_SRCS
      ${configurationmanager_xml}
      configurationmanager_dbus_interface
   )

   # call manager interface
   SET ( callmanager_xml  ${dbus_xml_introspecs_path}/cx.ring.Ring.CallManager.xml )

   SET_SOURCE_FILES_PROPERTIES(
      ${callmanager_xml}
      PROPERTIES
      CLASSNAME CallManagerInterface
      INCLUDE ${dbus_metatype_path}
   )

   QT5_ADD_DBUS_INTERFACE(
      libringqt_LIB_SRCS
      ${callmanager_xml}
      callmanager_dbus_interface
   )

   # video manager interface
   SET ( video_xml  ${dbus_xml_introspecs_path}/cx.ring.Ring.VideoManager.xml )

   SET_SOURCE_FILES_PROPERTIES(
      ${video_xml}
      PROPERTIES
      CLASSNAME VideoManagerInterface
      INCLUDE ${dbus_metatype_path}
   )

   QT5_ADD_DBUS_INTERFACE(
      libringqt_LIB_SRCS
      ${video_xml}
      video_dbus_interface
   )


   # instance interface
   SET ( instance_xml  ${dbus_xml_introspecs_path}/cx.ring.Ring.Instance.xml )

   SET_SOURCE_FILES_PROPERTIES(
      ${instance_xml}
      PROPERTIES
      CLASSNAME InstanceManagerInterface
      INCLUDE ${dbus_metatype_path}
   )

   QT5_ADD_DBUS_INTERFACE(
      libringqt_LIB_SRCS
      ${instance_xml}
      instance_dbus_interface
   )

ENDIF()

# Manually wrap private files and interfaces
SET(libringqt_PRIVATE_HDRS
   src/private/call_p.h
   src/private/ringdevicemodel_p.h
   src/private/namedirectory_p.h
   src/private/account_p.h
   src/private/sortproxies.h
   src/private/accountmodel_p.h
   src/private/phonedirectorymodel_p.h
   src/private/videorenderer_p.h
   src/private/textrecording_p.h
   src/private/videodevice_p.h
   src/private/collectionmodel_p.h
   src/private/contactmethod_p.h
   src/private/securityflaw_p.h
   src/private/macromodel_p.h
   src/private/person_p.h
   src/collectioncreationinterface.h
   src/private/securityevaluationmodel_p.h
   src/collectionconfigurationinterface.h
   src/private/imconversationmanagerprivate.h
   src/private/smartInfoHub_p.h
   src/private/certificatemodel_p.h
   src/usagestatistics.h
   src/private/personstatistics.hpp
   src/itemdataroles.h
)

QT5_WRAP_CPP(LIB_HEADER_MOC ${libringqt_PRIVATE_HDRS})

ADD_LIBRARY( ringqt ${libringqt_LIB_SRCS} ${LIB_HEADER_MOC} )

IF (DEFINED ring_INCLUDE_DIRS)
   TARGET_INCLUDE_DIRECTORIES( ringqt PRIVATE
      ${ring_INCLUDE_DIRS}
   )
ENDIF()

IF(ENABLE_LIBWRAP)
   TARGET_INCLUDE_DIRECTORIES( ringqt PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src/qtwrapper/
   )
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   FIND_PACKAGE(Qt5Concurrent)

   #For some reason, QSemaphore won't compile on Windows without QtConcurrent
   TARGET_LINK_LIBRARIES( ringqt
      Qt5::Concurrent
   )
ENDIF()

SET(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include)

SET(libringqt_CONFIG_PATH "${CMAKE_CURRENT_BINARY_DIR}/LibRingQtConfig.cmake")

CONFIGURE_PACKAGE_CONFIG_FILE(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/LibRingQtConfig.cmake.in" ${libringqt_CONFIG_PATH}
   INSTALL_DESTINATION ${LIB_INSTALL_DIR}/libringqt/cmake
   PATH_VARS INCLUDE_INSTALL_DIR
)

SET_TARGET_PROPERTIES(ringqt PROPERTIES
    PUBLIC_HEADER "${libringqt_LIB_HDRS};${libringqt_extra_LIB_HDRS}"
)

TARGET_LINK_LIBRARIES( ringqt
   -lpthread
   Qt5::Core
)

# Include as PRIVATE because of the /src/ component
#TODO figure out how to make them public without an error. This
#allows some extra macros to start working
TARGET_INCLUDE_DIRECTORIES( ringqt PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Use the daemon when libringqt doesn't link directly to libring
IF(NOT ENABLE_LIBWRAP)
   TARGET_LINK_LIBRARIES( ringqt
      Qt5::DBus
   )
ELSE()
   TARGET_LINK_LIBRARIES( ringqt
      ${ring_BIN}
   )
ENDIF()

# Make LRC slow, but catches issues
IF(${ENABLE_TEST_ASSERTS} MATCHES "ON")
   MESSAGE(STATUS "Adding more debug output")
   ADD_DEFINITIONS(-DENABLE_TEST_ASSERTS=true)
ENDIF()

# Fix some issues on Linux and Android
CHECK_LIBRARY_EXISTS(rt clock_gettime "time.h" NEED_LIBRT)
IF(NEED_LIBRT)
   TARGET_LINK_LIBRARIES(ringqt rt)

   IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
      ADD_DEFINITIONS(-D__ANDROID__=1)
   ELSE()
      ADD_DEFINITIONS(-D__ANDROID__=0)
   ENDIF()

ELSE()
   ADD_DEFINITIONS(-D__ANDROID__=0)
ENDIF()

EXPORT(TARGETS ringqt
   FILE "${PROJECT_BINARY_DIR}/LibRingQtTargets.cmake"
)

SET_TARGET_PROPERTIES( ringqt
   PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_VERSION}
)

IF("${DISABLE_EXPORT}" MATCHES "OFF")
    INSTALL(TARGETS ringqt
        EXPORT LibRingQtTargets
        PUBLIC_HEADER DESTINATION "${INCLUDE_INSTALL_DIR}/libringqt"
        LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}" COMPONENT ringqt
        ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
        COMPONENT Devel
    )

    INSTALL(EXPORT LibRingQtTargets
        DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/LibRingQt
        COMPONENT Devel
    )

    INSTALL( FILES ${libringqt_video_LIB_HDRS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/libringqt/video
        COMPONENT Devel
    )

    INSTALL( FILES ${libringqt_audio_LIB_HDRS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/libringqt/audio
        COMPONENT Devel
    )

    INSTALL( FILES ${libringqt_media_LIB_HDRS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/libringqt/media
        COMPONENT Devel
    )

    INSTALL( FILES ${libringqt_card_LIB_HDRS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/libringqt/libcard
        COMPONENT Devel
    )

    INSTALL( FILES ${libringqt_troubleshoot_LIB_HDRS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/libringqt/troubleshoot
        COMPONENT Devel
    )

    INSTALL( FILES ${libringqt_extensions_LIB_HDRS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/libringqt/extensions
        COMPONENT Devel
    )

    INSTALL( FILES ${libringqt_interface_LIB_HDRS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/libringqt/interfaces
        COMPONENT Devel
    )

    INSTALL( TARGETS ringqt
        ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
        DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    )

    INSTALL( FILES ${libringqt_CONFIG_PATH}
      DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/LibRingQt
      COMPONENT Devel
    )
ENDIF()

# translations
IF( Qt5LinguistTools_FOUND )
   # translation template file
   SET(TS_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/translations/lrc_en.ts)

   # get all translation files except the template
   FILE(GLOB TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/translations/*.ts)
   LIST(REMOVE_ITEM TS_FILES ${TS_TEMPLATE})

   # compiled .qm files
   QT5_ADD_TRANSLATION(QM_FILES ${TS_FILES})

   ADD_CUSTOM_TARGET(translations ALL DEPENDS ${QM_FILES})

   IF("${DISABLE_EXPORT}" MATCHES "OFF")
      INSTALL(FILES ${QM_FILES} DESTINATION "${CMAKE_INSTALL_PREFIX}/share/libringqt/translations")
   ENDIF()
ENDIF()

# uninstall target
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
)

ADD_CUSTOM_TARGET(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)
