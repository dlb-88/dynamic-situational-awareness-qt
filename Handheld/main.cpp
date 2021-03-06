
/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

// PCH header
#include "pch.hpp"

// dsa app headers
#include "AddLocalDataController.h"
#include "AlertConditionsController.h"
#include "AlertListController.h"
#include "AnalysisListController.h"
#include "AppInfo.h"
#include "BasemapPickerController.h"
#include "ObservationReportController.h"
#include "ContextMenuController.h"
#include "DsaResources.h"
#include "FollowPositionController.h"
#include "Handheld.h"
#include "HandheldStyles.h"
#include "IdentifyController.h"
#include "LineOfSightController.h"
#include "LocationController.h"
#include "LocationTextController.h"
#include "MarkupController.h"
#include "MessageFeedsController.h"
#include "NavigationController.h"
#include "OpenMobileScenePackageController.h"
#include "OptionsController.h"
#include "TableOfContentsController.h"
#include "ViewedAlertsController.h"
#include "ViewshedController.h"
#include "PackageImageProvider.h"

// toolkit headers
#include "ArcGISCompassController.h"
#include "ArcGISRuntimeToolkit.h"

// C++ API headers
#include "ArcGISRuntimeEnvironment.h"
#include "PopupManager.h"
#include "SceneQuickView.h"

// Qt headers
#include <QCommandLineParser>
#include <QDir>
#include <QGuiApplication>
#include <QMessageBox>
#include <QObject>
#include <QQmlEngine>
#include <QQuickView>
#include <QSettings>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

//------------------------------------------------------------------------------

#define kSettingsFormat                 QSettings::IniFormat

//------------------------------------------------------------------------------

#define kArgShowName                    "show"
#define kArgShowValueName               "showOption"
#define kArgShowDescription             "Show option maximized | minimized | fullscreen | normal | default"
#define kArgShowDefault                 "show"

#define kShowMaximized                  "maximized"
#define kShowMinimized                  "minimized"
#define kShowFullScreen                 "fullscreen"
#define kShowNormal                     "normal"
#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

//------------------------------------------------------------------------------

using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Toolkit;

QObject* dsaStylesProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
QObject* dsaResourcesProvider(QQmlEngine* engine, QJSEngine* scriptEngine);

int main(int argc, char *argv[])
{
#ifndef Q_OS_WIN
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  QGuiApplication app(argc, argv);

  QCoreApplication::setApplicationName(kApplicationName);
  QCoreApplication::setApplicationVersion(kApplicationVersion);
  QCoreApplication::setOrganizationName(kOrganizationName);
#ifdef Q_OS_MAC
  QCoreApplication::setOrganizationDomain(kOrganizationName);
#else
  QCoreApplication::setOrganizationDomain(kOrganizationDomain);
#endif
  QSettings::setDefaultFormat(kSettingsFormat);

  // Register the map view for QML
  qmlRegisterType<SceneQuickView>("Esri.Handheld", 1, 0, "SceneView");
  qRegisterMetaType<PopupManager*>("PopupManager*");

  // Register the Handheld (QQuickItem) for QML
  qmlRegisterType<Dsa::Handheld::Handheld>("Esri.Handheld", 1, 0, "Handheld");
  qmlRegisterType<Dsa::BasemapPickerController>("Esri.DSA", 1, 0, "BasemapPickerController");
  qmlRegisterType<Dsa::AddLocalDataController>("Esri.DSA", 1, 0, "AddLocalDataController");
  qmlRegisterType<Dsa::LocationController>("Esri.DSA", 1, 0, "LocationController");
  qmlRegisterType<Dsa::MessageFeedsController>("Esri.DSA", 1, 0, "MessageFeedsController");
  qmlRegisterType<Dsa::FollowPositionController>("Esri.DSA", 1, 0, "FollowPositionController");
  qmlRegisterType<Dsa::TableOfContentsController>("Esri.DSA", 1, 0, "TableOfContentsController");
  qmlRegisterType<Dsa::NavigationController>("Esri.DSA", 1, 0, "NavigationController");
  qmlRegisterType<Dsa::MarkupController>("Esri.DSA", 1, 0, "MarkupController");
  qmlRegisterType<Dsa::ViewshedController>("Esri.DSA", 1, 0, "ViewshedController");
  qmlRegisterType<Dsa::OptionsController>("Esri.DSA", 1, 0, "OptionsController");
  qmlRegisterSingletonType<Dsa::Handheld::HandheldStyles>("Esri.DSA", 1, 0, "DsaStyles", &dsaStylesProvider);
  qmlRegisterSingletonType<Dsa::DsaResources>("Esri.DSA", 1, 0, "DsaResources", &dsaResourcesProvider);
  qmlRegisterType<Dsa::IdentifyController>("Esri.DSA", 1, 0, "IdentifyController");
  qmlRegisterType<Dsa::AlertListController>("Esri.DSA", 1, 0, "AlertListController");
  qmlRegisterType<Dsa::ViewedAlertsController>("Esri.DSA", 1, 0, "ViewedAlertsController");
  qmlRegisterType<Dsa::LocationTextController>("Esri.DSA", 1, 0, "LocationTextController");
  qmlRegisterType<Dsa::AlertConditionsController>("Esri.DSA", 1, 0, "AlertConditionsController");
  qmlRegisterType<Dsa::LineOfSightController>("Esri.DSA", 1, 0, "LineOfSightController");
  qmlRegisterType<Dsa::ContextMenuController>("Esri.DSA", 1, 0, "ContextMenuController");
  qmlRegisterType<Dsa::AnalysisListController>("Esri.DSA", 1, 0, "AnalysisListController");
  qmlRegisterType<Dsa::ObservationReportController>("Esri.DSA", 1, 0, "ObservationReportController");
  qmlRegisterType<Dsa::OpenMobileScenePackageController>("Esri.DSA", 1, 0, "OpenMobileScenePackageController");

  // Register Toolkit Component Types
  ArcGISRuntimeToolkit::registerToolkitTypes();

  // Initialize application view
  QQuickView view;
  view.setResizeMode(QQuickView::SizeRootObjectToView);

  view.engine()->addImageProvider(QStringLiteral("packages"), new Dsa::PackageImageProvider());

#ifndef DEPLOYMENT_BUILD
  // Add the import Path
  view.engine()->addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));
  QString arcGISRuntimeImportPath = QUOTE(ARCGIS_RUNTIME_IMPORT_PATH);
  QString arcGISToolkitImportPath = QUOTE(ARCGIS_TOOLKIT_IMPORT_PATH);

#if defined(LINUX_PLATFORM_REPLACEMENT)
  // on some linux platforms the string 'linux' is replaced with 1
  // fix the replacement paths which were created
  QString replaceString = QUOTE(LINUX_PLATFORM_REPLACEMENT);
  arcGISRuntimeImportPath = arcGISRuntimeImportPath.replace(replaceString, "linux", Qt::CaseSensitive);
  arcGISToolkitImportPath = arcGISToolkitImportPath.replace(replaceString, "linux", Qt::CaseSensitive);
#endif

  // Add the Runtime and Extras path
  view.engine()->addImportPath(arcGISRuntimeImportPath);
  // Add the Toolkit path
  view.engine()->addImportPath(arcGISToolkitImportPath);
#endif // DEPLOYMENT_BUILD

  // To quit via Qt.quit() from QML, you must connect the QQmlEngine::quit()
  // signal to the QCoreApplication::quit() slot
  QObject::connect(view.engine(), &QQmlEngine::quit, &QCoreApplication::quit);

  // Set the source
  view.setSource(QUrl(kApplicationSourceUrl));

#if !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID)
  // Process command line
  QCommandLineOption showOption(kArgShowName, kArgShowDescription, kArgShowValueName, kArgShowDefault);

  QCommandLineParser commandLineParser;

  commandLineParser.setApplicationDescription(kApplicationDescription);
  commandLineParser.addOption(showOption);
  commandLineParser.addHelpOption();
  commandLineParser.addVersionOption();
  commandLineParser.process(app);

  // Show app window

  auto showValue = commandLineParser.value(kArgShowName).toLower();

  if (showValue.compare(kShowMaximized) == 0)
  {
    view.showMaximized();
  }
  else if (showValue.compare(kShowMinimized) == 0)
  {
    view.showMinimized();
  }
  else if (showValue.compare(kShowFullScreen) == 0)
  {
    view.showFullScreen();
  }
  else if (showValue.compare(kShowNormal) == 0)
  {
    view.showNormal();
  }
  else
  {
    view.show();
  }

#else
  view.show();
#endif

  return app.exec();
}

//------------------------------------------------------------------------------

// qml dsa styles provider
QObject* dsaStylesProvider(QQmlEngine* engine, QJSEngine*)
{
  static Dsa::Handheld::HandheldStyles* dsaStyles = new Dsa::Handheld::HandheldStyles(engine);
  return dsaStyles;
}

QObject* dsaResourcesProvider(QQmlEngine* engine, QJSEngine*)
{
  static Dsa::DsaResources* dsaResources = new Dsa::DsaResources(engine);
  return dsaResources;
}
