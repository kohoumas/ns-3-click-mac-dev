/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 * Authors: Ashwin Narayan <ashwin.narayan89@gmail.com>
 */

#include "ns3/core-module.h"
#include "ns3/propagation-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/click-internet-stack-helper.h"
#include "ns3/ipv4-click-routing.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/log.h"
//#include "ns3/flow-monitor-helper.h"
//#include "ns3/netanim-module.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("RoofnetClickSimulator");

#ifdef NS3_CLICK

void WriteClick (uint8_t node_id, Ptr<Ipv4ClickRouting> clickRouter)
{
  char node_name[100];
  sprintf(node_name, "Node%3d ", node_id);
  NS_LOG_INFO (node_name);
  NS_LOG_INFO ("  data queues(s) :\n" << clickRouter->WriteHandler ("data_q", "change_route", "true"));
}

void ReadClick (uint8_t node_id, Ptr<Ipv4ClickRouting> clickRouter)
{
  char node_name[100];
  sprintf(node_name, "Node%3d ", node_id);
  NS_LOG_INFO (node_name);
  NS_LOG_INFO ("  floods :\n" << clickRouter->ReadHandler ("srcr/query_forwarder", "floods"));
  //NS_LOG_INFO ("  data queue :\n" << clickRouter->ReadHandler ("data_q", "highwater_length"));
  //NS_LOG_INFO ("  data forwd :\n" << clickRouter->ReadHandler ("srcr/forwarder", "stats"));
  //NS_LOG_INFO ("  outgoing :\n" << clickRouter->ReadHandler ("srcr/counter_outgoing", "stats"));
  //NS_LOG_INFO ("  incoming :\n" << clickRouter->ReadHandler ("srcr/counter_incoming", "stats"));
  //NS_LOG_INFO ("  rxstats :\n" << clickRouter->ReadHandler ("rxstats", "stats"));
  //NS_LOG_INFO ("  links :\n" << clickRouter->ReadHandler ("srcr/lt", "links"));
}

NodeContainer nodes;
//Ptr<MatrixPropagationLossModel> lossModel;

/*void setupLinkLoss (int i, int j, int loss)
{
  lossModel->SetLoss(nodes.Get (i)->GetObject<MobilityModel> (), nodes.Get (j)->GetObject<MobilityModel> (), loss, true);
}*/

int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);

  LogComponentEnable ("RoofnetClickSimulator", LOG_LEVEL_INFO);
  //LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);
  //LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);
  //LogComponentEnable ("Ipv4L3ClickProtocol", LOG_LEVEL_FUNCTION);
  //LogComponentEnable ("Ipv4ClickRouting", LOG_LEVEL_FUNCTION);
  //LogComponentEnable ("WifiNetDevice", LOG_LEVEL_FUNCTION);
  //LogComponentEnable ("MonitorWifiMac", LOG_LEVEL_FUNCTION);
  //LogComponentEnable ("DcaTxop", LOG_LEVEL_FUNCTION);
  //LogComponentEnable ("RadiotapHeader", LOG_LEVEL_FUNCTION);
  //LogComponentEnable ("YansWifiChannel", LOG_LEVEL_FUNCTION);
  //LogComponentEnable ("YansWifiPhy", LOG_LEVEL_FUNCTION);
  //LogComponentEnableAll (LOG_LEVEL_FUNCTION);
  LogComponentEnableAll (LOG_PREFIX_FUNC);
  LogComponentEnableAll (LOG_PREFIX_NODE);

  //Packet::EnablePrinting ();
  //PacketMetadata::Enable ();
  //PacketMetadata::EnableChecking ();
  //Packet::EnableChecking ();
  // Fix max size of Wifi Mac Queue
  //Config::SetDefault ("ns3::WifiMacQueue::MaxPacketNumber", UintegerValue (200));
  // Computes real checksums everywhere (IP, TCP, UDP)
  //GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));


  // Setup nodes
  uint32_t nWifi = 6;
  nodes.Create (nWifi);
  for (size_t i = 0; i < nWifi; ++i)
    nodes.Get (i)->AggregateObject (CreateObject<ConstantPositionMobilityModel> ());

  // Setup devices
  /*lossModel = CreateObject<MatrixPropagationLossModel> ();
  lossModel->SetDefaultLoss (200); // loss = 200 dB (no link), 50 dB (inside transmission range), 110 dB (inside interfering range)
  setupLinkLoss (0, 1, 50);*/

  YansWifiChannelHelper channelHelper = YansWifiChannelHelper::Default ();
  Ptr<YansWifiChannel> channel = channelHelper.Create ();  
  //channel->SetPropagationLossModel (lossModel);
  channel->SetPropagationLossModel (CreateObject <LogDistancePropagationLossModel> ()); // Default
  channel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ()); // Default

  YansWifiPhyHelper phyHelper =  YansWifiPhyHelper::Default ();
  phyHelper.SetChannel (channel);

  // Add a non-QoS upper mac, and disable rate control
  NqosWifiMacHelper macHelper = NqosWifiMacHelper::Default ();
  macHelper.SetType ("ns3::MonitorWifiMac");

  WifiHelper wifiHelper = WifiHelper::Default ();
  wifiHelper.SetStandard (WIFI_PHY_STANDARD_80211a);
  //string phyMode ("OfdmRate6Mbps");
  wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      //"DataMode", StringValue (phyMode),
                                      //"ControlMode", StringValue (phyMode),
                                      //"NonUnicastMode", StringValue (phyMode),
                                      "FragmentationThreshold", StringValue ("2200"),
                                      "RtsCtsThreshold", StringValue ("2200"),
                                      "MaxSsrc", UintegerValue (7), //The maximum number of retransmission attempts for an RTS
                                      "MaxSlrc", UintegerValue (7)); //The maximum number of retransmission attempts for a DATA packet.

  NetDeviceContainer devices;
  devices = wifiHelper.Install (phyHelper, macHelper, nodes);
  for (NetDeviceContainer::Iterator i = devices.Begin (); i != devices.End (); ++i) {
    Ptr<NetDevice> device = *i;
    Ptr<WifiNetDevice> wifiDevice = DynamicCast<WifiNetDevice> (device);
    wifiDevice->SetMonitorMode (); // Sets the NetDevice in Monitor mode
  }


  // Setup tracing
  phyHelper.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
  phyHelper.EnablePcap ("/tmp/roofnet-ns", devices);


  // Setup mobility models
  MobilityHelper mobilityHelper;
  mobilityHelper.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (100.0),
                                 "DeltaY", DoubleValue (100.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
  mobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityHelper.Install (nodes.Get (0));
  mobilityHelper.Install (nodes.Get (1));
  mobilityHelper.Install (nodes.Get (2));
  mobilityHelper.Install (nodes.Get (3));
  mobilityHelper.Install (nodes.Get (4));
  mobilityHelper.Install (nodes.Get (5));

  // Setup Click internet stack
  ClickInternetStackHelper clickInternetHelper;
  for (uint8_t i = 0; i < nWifi; ++i) {
    char filename[100];
    sprintf (filename, "/tmp/roofnet_bp_ns_%d.click", i+1);
    clickInternetHelper.SetClickFile (nodes.Get (i), filename);
  }
  clickInternetHelper.SetRoutingTableElement (nodes, "rt");
  clickInternetHelper.Install (nodes);

  
  // Configure IP addresses
  Ipv4AddressHelper ipv4Helper;
  ipv4Helper.SetBase ("5.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces;
  interfaces = ipv4Helper.Assign (devices);


  // Setup traffic application
  double startTime = 5.0;
  double endTime = 125.0;

  int source_id = 0, sink_id = nWifi-1;
  int on = 1;
  const char *rate = "100Kb/s";

  PacketSinkHelper sinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 9));
  sinkHelper.SetAttribute ("StartTime", TimeValue (Seconds (startTime + 0.001)));
  ApplicationContainer sink = sinkHelper.Install (nodes.Get (sink_id));

  OnOffHelper onoffHelper ("ns3::UdpSocketFactory", Address (InetSocketAddress (interfaces.GetAddress (sink_id), 9)));
  onoffHelper.SetAttribute ("OnTime", RandomVariableValue (ConstantVariable (on)));
  onoffHelper.SetAttribute ("OffTime", RandomVariableValue (ConstantVariable (!on)));
  onoffHelper.SetAttribute ("MaxBytes", UintegerValue (0));
  onoffHelper.SetAttribute ("PacketSize", StringValue ("1024"));
  onoffHelper.SetAttribute ("DataRate", StringValue (rate));
  onoffHelper.SetAttribute ("StartTime", TimeValue (Seconds (startTime + 0.003)));
  ApplicationContainer onoff = onoffHelper.Install (nodes.Get (source_id));

  /*// Setup flow monitor // Problem with click
  FlowMonitorHelper monitorHelper = FlowMonitorHelper ();
  Ptr<FlowMonitor> monitor = monitorHelper.Install (nodes.Get (0));
  monitor->SetAttribute ("DelayBinWidth", DoubleValue (0.001));
  monitor->SetAttribute ("JitterBinWidth", DoubleValue (0.001));
  monitor->SetAttribute ("PacketSizeBinWidth", DoubleValue (20));*/

  /* // Setup animation // Problem with wifi
  AnimationInterface anim;
  anim.SetOutputFile ("/tmp/animation");
  //anim.SetXMLOutput ();
  //anim.SetMobilityPollInterval (Seconds (1));	
  anim.StartAnimation ();*/


  // Simulation commands
  for (uint8_t i = 0; i < nWifi; ++i) {
//    Simulator::Schedule (Seconds (startTime), &WriteClick, (i+1), nodes.Get (i)->GetObject<Ipv4ClickRouting> ());
    Simulator::Schedule (Seconds (endTime), &ReadClick, (i+1), nodes.Get (i)->GetObject<Ipv4ClickRouting> ());
  }

  Simulator::Stop (Seconds (endTime));
  Simulator::Run ();
  //monitor->SerializeToXmlFile ("/tmp/results.xml", true , true);
  Simulator::Destroy ();
  //anim.StopAnimation ();

  NS_LOG_INFO ("flow " << source_id << " -> " << sink_id << " : " << DynamicCast<PacketSink> (sink.Get (0))->GetTotalRx () << " bytes in " << (endTime - startTime) << " seconds");
  NS_LOG_INFO (" throughput : " << DynamicCast<PacketSink> (sink.Get (0))->GetTotalRx () * 8 / (endTime - startTime) / 1024 << " Kbps");

  return 0;

}
#endif

