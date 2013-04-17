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
#ifndef MONITOR_WIFI_MAC_H
#define MONITOR_WIFI_MAC_H

#include "ns3/wifi-mac.h"
#include "ns3/radiotap-header.h"

#include "dca-txop.h"
#include "edca-txop-n.h"
#include "wifi-remote-station-manager.h"
#include "ssid.h"
#include "qos-utils.h"

#include <map>

namespace ns3 {

class Dcf;
class MacLow;
class MacRxMiddle;
class MacTxMiddle;
class DcfManager;

/**
 * \brief base class for Monitor Wifi Mac objects.
 *
 * This class is essentially a copy of the non-QoS
 * functionality of RegularWifiMac and serves as an
 * abstraction for Monitor mode usage.
 *
 * This mode is generally used for passive sniffing. 
 * The interface receives all packets in its listening 
 * channel, even though it may not be destined for it.
 * The 802.11 MAC packet contains extra header radiotap 
 * which includes physical layer information such as
 * received channel, signal quality, signal to noise 
 * ratio, antenna scheme, etc. 
 *
 * The other purpose of monitor mode is packet injection. 
 * It is possible to inject raw IEEE 802.11 MAC frames 
 * using the radiotap header and monitor mode WLAN network 
 * interface.
 *
 * Please note that WifiNetDevice::SetMonitorMode () must 
 * be invoked for this to work correctly.
 *
 * Functions that have been inherited but not used have been
 * given empty implementations owing to the fact that they 
 * are not applicable.
 *
 */

class MonitorWifiMac : public WifiMac
{
public:
  static TypeId GetTypeId (void);

  MonitorWifiMac ();
  virtual ~MonitorWifiMac ();

  /**
   * The following functions have been inherited from WifiMac.
   *
   */

  void SetSlot (Time slotTime);
  void SetSifs (Time sifs);
  void SetEifsNoDifs (Time eifsNoDifs);
  void SetPifs (Time pifs);
  void SetCtsTimeout (Time ctsTimeout);
  void SetAckTimeout (Time ackTimeout);
  Time GetPifs (void) const;
  Time GetSifs (void) const;
  Time GetSlot (void) const;
  Time GetEifsNoDifs (void) const;
  Time GetCtsTimeout (void) const;
  Time GetAckTimeout (void) const;
  virtual Mac48Address GetAddress (void) const;
  virtual void SetAddress (Mac48Address address);
  virtual void SetPromisc (void);
  
  /**
   * The following functions have been inherited from WifiMac
   * but are not applicable in Monitor Mode.
   */
  virtual Ssid GetSsid (void) const;
  virtual void SetSsid (Ssid ssid);
  virtual void SetBssid (Mac48Address bssid);
  virtual Mac48Address GetBssid (void) const;
  virtual void Enqueue (Ptr<const Packet> packet, Mac48Address to, Mac48Address from);
  virtual bool SupportsSendFrom (void) const;
  virtual void Enqueue (Ptr<const Packet> packet, Mac48Address to);
  virtual void DeaggregateAmsduAndForward (Ptr<Packet> aggregatedPacket,
                                           const WifiMacHeader *hdr);
  virtual void SendAddBaResponse (const MgtAddBaRequestHeader *reqHdr,
                                  Mac48Address originator);
  bool m_qosSupported;
  void SetQosSupported (bool enable);
  bool GetQosSupported () const;

  virtual void SetWifiPhy (Ptr<WifiPhy> phy);
  virtual void SetWifiRemoteStationManager (Ptr<WifiRemoteStationManager> stationManager);
  virtual Ptr<WifiRemoteStationManager> GetWifiRemoteStationManager (void) const;

  typedef Callback<void, Ptr<Packet>, Mac48Address, Mac48Address> ForwardUpCallback;
  virtual void SetForwardUpCallback (ForwardUpCallback upCallback);
  virtual void SetLinkUpCallback (Callback<void> linkUp);
  virtual void SetLinkDownCallback (Callback<void> linkDown);
  virtual void SetBasicBlockAckTimeout (Time blockAckTimeout);
  virtual Time GetBasicBlockAckTimeout (void) const;
  virtual void SetCompressedBlockAckTimeout (Time blockAckTimeout);
  virtual Time GetCompressedBlockAckTimeout (void) const;


protected:
  virtual void DoStart ();
  virtual void DoDispose ();

  MacRxMiddle *m_rxMiddle;
  MacTxMiddle *m_txMiddle;
  Ptr<MacLow> m_low;
  DcfManager *m_dcfManager;
  Ptr<WifiPhy> m_phy;

  Ptr<WifiRemoteStationManager> m_stationManager;

  ForwardUpCallback m_forwardUp;
  Callback<void> m_linkUp;
  Callback<void> m_linkDown;

  Ssid m_ssid;

  Ptr<DcaTxop> m_dca;

  virtual void FinishConfigureStandard (enum WifiPhyStandard standard);

  /**
   * This method acts as the MacRxMiddle receive callback and is
   * invoked to notify us that a frame has been received. The radiotap
   * header is added to the packet here and then sent up the stack after
   * extracting the origin and destination addresses from the MAC header.
   *
   * \param packet the packet that has been received.
   * \param hdr a pointer to the MAC header of the received frame.
   * \param radiotaphdr a pointer to the Radiotap Header of the packet.
   */
  virtual void Receive (Ptr<Packet> packet, const WifiMacHeader *hdr, const RadiotapHeader *radiotaphdr);
  virtual void TxOk (const WifiMacHeader &hdr);
  virtual void TxFailed (const WifiMacHeader &hdr);

  void ForwardUp (Ptr<Packet> packet, Mac48Address from, Mac48Address to);
private:
  /** Accessor for the DCF object */
  Ptr<DcaTxop> GetDcaTxop (void) const;
  /**
   * Sets the transmission parameters from radiotap information
   */
  void SetTxParametersFromRadiotapHeader (RadiotapHeader radiotaphdr);

  TracedCallback<const WifiMacHeader &> m_txOkCallback;
  TracedCallback<const WifiMacHeader &> m_txErrCallback;
};

} // namespace ns3

#endif /* MONITOR_WIFI_MAC_H */
