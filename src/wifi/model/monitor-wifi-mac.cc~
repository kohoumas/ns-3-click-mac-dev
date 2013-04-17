/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>, 
 *         Ashwin Narayan <ashwin.narayan89@gmail.com>
 */
#include "monitor-wifi-mac.h"

#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/pointer.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/abort.h"

#include "mac-rx-middle.h"
#include "mac-tx-middle.h"
#include "mac-low.h"
#include "dcf.h"
#include "dcf-manager.h"
#include "wifi-phy.h"

#include "msdu-aggregator.h"

NS_LOG_COMPONENT_DEFINE ("MonitorWifiMac");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MonitorWifiMac);

TypeId
MonitorWifiMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MonitorWifiMac")
    .SetParent<WifiMac> ()
    .AddConstructor<MonitorWifiMac> ()
  ;
  return tid;
}

/*
 * Please note that WifiNetDevice::SetMonitorMode () must 
 * be invoked for this to work correctly.
 *
 * Functions that have been inherited but not used have been
 * given empty implementations owing to the fact that they 
 * are not applicable.
 *
 */

MonitorWifiMac::MonitorWifiMac ()
{
  NS_LOG_FUNCTION (this);
  m_rxMiddle = new MacRxMiddle ();
  m_rxMiddle->SetForwardCallback (MakeCallback (&MonitorWifiMac::Receive, this));

  m_txMiddle = new MacTxMiddle ();

  m_low = CreateObject<MacLow> ();
  m_low->SetRxCallback (MakeCallback (&MacRxMiddle::Receive, m_rxMiddle));
  m_low->SetPromisc (); // Enables Monitor Mode functionality

  m_dcfManager = new DcfManager ();
  m_dcfManager->SetupLowListener (m_low);

  m_dca = CreateObject<DcaTxop> ();
  m_dca->SetLow (m_low);
  m_dca->SetManager (m_dcfManager);
  m_dca->SetTxOkCallback (MakeCallback (&MonitorWifiMac::TxOk, this));
  m_dca->SetTxFailedCallback (MakeCallback (&MonitorWifiMac::TxFailed, this));
}

MonitorWifiMac::~MonitorWifiMac ()
{
  NS_LOG_FUNCTION (this);
}

void
MonitorWifiMac::DoStart ()
{
  NS_LOG_FUNCTION (this);

  m_dca->Start ();
}

void
MonitorWifiMac::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_rxMiddle;
  m_rxMiddle = NULL;

  delete m_txMiddle;
  m_txMiddle = NULL;

  delete m_dcfManager;
  m_dcfManager = NULL;

  m_low->Dispose ();
  m_low = NULL;

  m_phy = NULL;
  m_stationManager = NULL;

  m_dca->Dispose ();
  m_dca = NULL;
}

Ptr<DcaTxop>
MonitorWifiMac::GetDcaTxop () const
{
  return m_dca;
}

void
MonitorWifiMac::SetForwardUpCallback (ForwardUpCallback upCallback)
{
  NS_LOG_FUNCTION (this);
  m_forwardUp = upCallback;
}

void
MonitorWifiMac::SetLinkUpCallback (Callback<void> linkUp)
{
  NS_LOG_FUNCTION (this);
  m_linkUp = linkUp;
}

void
MonitorWifiMac::SetLinkDownCallback (Callback<void> linkDown)
{
  NS_LOG_FUNCTION (this);
  m_linkDown = linkDown;
}

void
MonitorWifiMac::SetQosSupported (bool enable)
{
  NS_ABORT_MSG ("QoS Functionality not supported in MonitorWifiMac");
}

bool
MonitorWifiMac::GetQosSupported () const
{
  return false;
}

void
MonitorWifiMac::SetSlot (Time slotTime)
{
  NS_LOG_FUNCTION (this << slotTime);
  m_dcfManager->SetSlot (slotTime);
  m_low->SetSlotTime (slotTime);
}

Time
MonitorWifiMac::GetSlot (void) const
{
  return m_low->GetSlotTime ();
}

void
MonitorWifiMac::SetSifs (Time sifs)
{
  NS_LOG_FUNCTION (this << sifs);
  m_dcfManager->SetSifs (sifs);
  m_low->SetSifs (sifs);
}

Time
MonitorWifiMac::GetSifs (void) const
{
  return m_low->GetSifs ();
}

void
MonitorWifiMac::SetEifsNoDifs (Time eifsNoDifs)
{
  NS_LOG_FUNCTION (this << eifsNoDifs);
  m_dcfManager->SetEifsNoDifs (eifsNoDifs);
}

Time
MonitorWifiMac::GetEifsNoDifs (void) const
{
  return m_dcfManager->GetEifsNoDifs ();
}

void
MonitorWifiMac::SetPifs (Time pifs)
{
  NS_LOG_FUNCTION (this << pifs);
  m_low->SetPifs (pifs);
}

Time
MonitorWifiMac::GetPifs (void) const
{
  return m_low->GetPifs ();
}

void
MonitorWifiMac::SetAckTimeout (Time ackTimeout)
{
  NS_LOG_FUNCTION (this << ackTimeout);
  m_low->SetAckTimeout (ackTimeout);
}

Time
MonitorWifiMac::GetAckTimeout (void) const
{
  return m_low->GetAckTimeout ();
}

void
MonitorWifiMac::SetCtsTimeout (Time ctsTimeout)
{
  NS_LOG_FUNCTION (this << ctsTimeout);
  m_low->SetCtsTimeout (ctsTimeout);
}

Time
MonitorWifiMac::GetCtsTimeout (void) const
{
  return m_low->GetCtsTimeout ();
}

void
MonitorWifiMac::SetBasicBlockAckTimeout (Time blockAckTimeout)
{
  NS_LOG_FUNCTION (this << blockAckTimeout);
  m_low->SetBasicBlockAckTimeout (blockAckTimeout);
}

Time
MonitorWifiMac::GetBasicBlockAckTimeout (void) const
{
  return m_low->GetBasicBlockAckTimeout ();
}

void
MonitorWifiMac::SetCompressedBlockAckTimeout (Time blockAckTimeout)
{
  NS_LOG_FUNCTION (this << blockAckTimeout);
  m_low->SetCompressedBlockAckTimeout (blockAckTimeout);
}

Time
MonitorWifiMac::GetCompressedBlockAckTimeout (void) const
{
  return m_low->GetCompressedBlockAckTimeout ();
}

void
MonitorWifiMac::SetAddress (Mac48Address address)
{
  NS_LOG_FUNCTION (this << address);
  m_low->SetAddress (address);
}

Mac48Address
MonitorWifiMac::GetAddress (void) const
{
  return m_low->GetAddress ();
}

/**
 * Not applicable in Monitor Mode
 */
void
MonitorWifiMac::SetSsid (Ssid ssid)
{
}
/**
 * Not applicable in Monitor Mode
 */
Ssid
MonitorWifiMac::GetSsid (void) const
{
  return m_ssid;
}
/**
 * Not applicable in Monitor Mode
 */
void
MonitorWifiMac::SetBssid (Mac48Address bssid)
{
}
/**
 * Not applicable in Monitor Mode
 */
Mac48Address
MonitorWifiMac::GetBssid (void) const
{
  return m_low->GetBssid ();
}

void
MonitorWifiMac::SetWifiRemoteStationManager (Ptr<WifiRemoteStationManager> stationManager)
{
  NS_LOG_FUNCTION (this << stationManager);
  m_stationManager = stationManager;
  m_low->SetWifiRemoteStationManager (stationManager);

  m_dca->SetWifiRemoteStationManager (stationManager);
}

Ptr<WifiRemoteStationManager>
MonitorWifiMac::GetWifiRemoteStationManager () const
{
  return m_stationManager;
}

void
MonitorWifiMac::SetWifiPhy (Ptr<WifiPhy> phy)
{
  NS_LOG_FUNCTION (this << phy);
  m_phy = phy;
  m_dcfManager->SetupPhyListener (phy);
  m_low->SetPhy (phy);
}

void
MonitorWifiMac::SetPromisc (void)
{
  m_low->SetPromisc ();
}

/**
 * Not applicable in Monitor Mode
 */
void
MonitorWifiMac::Enqueue (Ptr<const Packet> packet,
                         Mac48Address to, Mac48Address from)
{
  NS_ABORT_MSG ("Not applicable in MonitorMacWifi");
}

void
MonitorWifiMac::Enqueue (Ptr<const Packet> packet,
                         Mac48Address to)
{
  NS_LOG_FUNCTION (this << packet << to);
  
  if (m_stationManager->IsBrandNew (to))
  {
    for (uint32_t i = 0; i < m_phy->GetNModes (); i++)
      {
        m_stationManager->AddSupportedMode (to, m_phy->GetMode (i));
      }
    m_stationManager->RecordDisassociated (to);
  }   

  Ptr<Packet> p = packet->Copy ();
  RadiotapHeader radiotaphdr;
  WifiMacHeader wifimacheader;
  p->RemoveHeader (radiotaphdr);
  p->RemoveHeader (wifimacheader);

  SetTxParametersFromRadiotapHeader (radiotaphdr);
  
  m_dca->Queue (p, wifimacheader);
}

/**
 * Not applicable in Monitor Mode
 */
bool
MonitorWifiMac::SupportsSendFrom (void) const
{
  return false;
}

void
MonitorWifiMac::ForwardUp (Ptr<Packet> packet, Mac48Address from, Mac48Address to)
{
  NS_LOG_FUNCTION (this << packet << from);
  m_forwardUp (packet, from, to);
}

void
MonitorWifiMac::Receive (Ptr<Packet> packet, const WifiMacHeader *hdr, const RadiotapHeader *radiotaphdr)
{

  Mac48Address to = hdr->GetAddr1 ();
  Mac48Address from = hdr->GetAddr2 ();
  NS_LOG_DEBUG ("Received packet to: " << to << " from: " << from << " at:" << m_low->GetAddress ());
  packet->AddHeader (*hdr);
  packet->AddHeader (*radiotaphdr);
  ForwardUp (packet, from, to);
}

void
MonitorWifiMac::DeaggregateAmsduAndForward (Ptr<Packet> aggregatedPacket,
                                            const WifiMacHeader *hdr)
{
}

void
MonitorWifiMac::SendAddBaResponse (const MgtAddBaRequestHeader *reqHdr,
                                   Mac48Address originator)
{
}

void
MonitorWifiMac::FinishConfigureStandard (enum WifiPhyStandard standard)
{
  uint32_t cwmin;
  uint32_t cwmax;

  switch (standard)
    {
    case WIFI_PHY_STANDARD_80211p_CCH:
    case WIFI_PHY_STANDARD_80211p_SCH:
      cwmin = 15;
      cwmax = 511;
      break;

    case WIFI_PHY_STANDARD_holland:
    case WIFI_PHY_STANDARD_80211a:
    case WIFI_PHY_STANDARD_80211g:
    case WIFI_PHY_STANDARD_80211_10Mhz:
    case WIFI_PHY_STANDARD_80211_5Mhz:
      cwmin = 15;
      cwmax = 1023;
      break;

    case WIFI_PHY_STANDARD_80211b:
      cwmin = 31;
      cwmax = 1023;
      break;

    default:
      NS_FATAL_ERROR ("Unsupported WifiPhyStandard in MonitorWifiMac::FinishConfigureStandard ()");
    }

  // The special value of AC_BE_NQOS which exists in the Access
  // Category enumeration allows us to configure plain old DCF.
  ConfigureDcf (m_dca, cwmin, cwmax, AC_BE_NQOS);
}

void
MonitorWifiMac::TxOk (const WifiMacHeader &hdr)
{
  NS_LOG_FUNCTION (this << hdr);
  m_txOkCallback (hdr);
}

void
MonitorWifiMac::TxFailed (const WifiMacHeader &hdr)
{
  NS_LOG_FUNCTION (this << hdr);
  m_txErrCallback (hdr);
}

/*
 * Sets the TX parameters in WifiRemoteStationManager from the radiotap information
 */

void
MonitorWifiMac::SetTxParametersFromRadiotapHeader (RadiotapHeader radiotaphdr)
{
  NS_LOG_FUNCTION (this << radiotaphdr);

  if (radiotaphdr.IsRtsRetriesSet ())
    {  
      m_stationManager->SetMaxSsrc (radiotaphdr.GetRtsRetries ());
    }
  else
    {
      m_stationManager->SetMaxSsrc (7);
    }

  if (radiotaphdr.IsDataRetriesSet ())
    {
      m_stationManager->SetMaxSlrc (radiotaphdr.GetDataRetries ());
    }
  else
    {
      m_stationManager->SetMaxSlrc (7);
    }

  if (radiotaphdr.IsTxPowerSet ())
    {
      m_stationManager->SetDefaultTxPowerLevel (radiotaphdr.GetTxPower ());
    }
  else
    {
      m_stationManager->SetDefaultTxPowerLevel (0);
    }
}

} // namespace ns3
