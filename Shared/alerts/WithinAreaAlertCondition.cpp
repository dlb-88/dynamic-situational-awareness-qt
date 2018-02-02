// Copyright 2017 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include "GraphicAlertSource.h"
#include "WithinAreaAlertCondition.h"
#include "WithinAreaAlertConditionData.h"

#include "GraphicsOverlay.h"
#include "GraphicListModel.h"

using namespace Esri::ArcGISRuntime;

/*!
  \class WithinAreaAlertCondition
  \inherits AlertCondition
  \brief Represents a spatial, "Geofence", condition which will be coninuosly monitored and will
  trigger an alert when a source object is within the area of a target object.

  This condition will create new \l WithinAreaAlertConditionData to track source and target objects.

  It can be tested using a \l WithinAreaAlertQuery.
  */

/*!
  \brief Constructor taking an \l AlertLevel (\a level) the \a name of the condition and
  an optional \a parent.
 */
WithinAreaAlertCondition::WithinAreaAlertCondition(AlertLevel level,
                                                   const QString& name,
                                                   QObject* parent):
  AlertCondition(level, name, parent)
{

}

/*!
  \brief Destructor.
 */
WithinAreaAlertCondition::~WithinAreaAlertCondition()
{

}

/*!
  \brief Creates a new \l WithinAreaAlertConditionData to track source and target objects.
 */
AlertConditionData* WithinAreaAlertCondition::createData(AlertSource* source, AlertTarget* target)
{
  return new WithinAreaAlertConditionData(newConditionDataName(), level(), source, target, this);
}