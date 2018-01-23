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

#include "AbstractOverlayManager.h"
#include "ProximityPairAlert.h"

#include "GeoElement.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

ProximityPairAlert::ProximityPairAlert(GeoElement* element1,
                                       GeoElement* element2,
                                       AbstractOverlayManager* overlay1Manager,
                                       AbstractOverlayManager* overlay2Manager,
                                       double distance,
                                       QObject* parent):
  AbstractAlert(parent),
  m_element1(element1),
  m_element2(element2),
  m_overlay1Manager(overlay1Manager),
  m_overlay2Manager(overlay2Manager),
  m_distance(distance)
{
  auto onElementDestroyed = [this]()
  {
    m_element1 = nullptr;
    m_element2 = nullptr;
    emit noLongerValid();
  };

  connect(m_element1, &GeoElement::destroyed, this, onElementDestroyed);
  connect(m_element2, &GeoElement::destroyed, this, onElementDestroyed);
}

ProximityPairAlert::~ProximityPairAlert()
{

}

Geometry ProximityPairAlert::position() const
{
  return m_element1 ? m_element1->geometry() : Point();
}

QString ProximityPairAlert::description() const
{
  return QString(element1Description() + QString(" < %1 m of ").arg(QString::number(m_distance)) + element2Description());
}

void ProximityPairAlert::highlight(bool on)
{
  m_overlay1Manager->setSelected(m_element1, on);
}

Geometry ProximityPairAlert::position2() const
{
  return m_element2 ? m_element2->geometry() : Point();
}

double ProximityPairAlert::distance() const
{
  return m_distance;
}

QString ProximityPairAlert::element1Description() const
{
  return m_overlay1Manager->elementDescription(m_element1);
}

QString ProximityPairAlert::element2Description() const
{
  return m_overlay2Manager->elementDescription(m_element2);
}
