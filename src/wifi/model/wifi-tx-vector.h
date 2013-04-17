/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 CTTC
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
 * Author: Nicola Baldo <nbaldo@cttc.es>
 */

#ifndef WIFI_TX_VECTOR_H
#define WIFI_TX_VECTOR_H

#include <ns3/wifi-mode.h>
#include <ostream>

namespace ns3 {


/**
 * This class mimics the TXVECTOR which is to be
 * passed to the PHY in order to define the parameters which are to be
 * used for a transmission. See IEEE 802.11-2007 15.2.6 "Transmit PLCP",
 * and also 15.4.4.2 "PMD_SAP peer-to-peer service primitive
 * parameters".
 *
 * \note the above reference is valid for the DSSS PHY only (clause
 * 15). TXVECTOR is defined also for the other PHYs, however they
 * don't include the TXPWRLVL explicitly in the TXVECTOR. This is
 * somewhat strange, since all PHYs actually have a
 * PMD_TXPWRLVL.request primitive. We decide to include the power
 * level in WifiTxVector for all PHYs, since it serves better our
 * purposes, and furthermore it seems close to the way real devices
 * work (e.g., madwifi).
 */
struct WifiTxVector
{
  WifiTxVector ();
  WifiTxVector (WifiMode m, uint8_t l, uint8_t r);

  WifiMode mode;         /**< The DATARATE parameter in Table 15-4. 
                           It is the value that will be passed
                           to PMD_RATE.request */ 
  uint8_t  txPowerLevel;  /**< The TXPWR_LEVEL parameter in Table 15-4. 
                           It is the value that will be passed
                           to PMD_TXPWRLVL.request */ 
  uint8_t  retries;      /**< The DATA_RETRIES/RTS_RETRIES parameter
                           for Click radiotap information */
};

std::ostream & operator << (std::ostream & os, const WifiTxVector &v);

} // namespace ns3

#endif // WIFI_TX_VECTOR_H
