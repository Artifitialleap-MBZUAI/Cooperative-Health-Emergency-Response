//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "veins/modules/application/traci/TraCIDemo11p.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include <vector>
#include <string>


using namespace veins;
using namespace std;

Define_Module(veins::TraCIDemo11p);


void TraCIDemo11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessageDoc = false;
        sentMessageAmb= false;
        sentMessage2 = false;
        sendMe = false;
        sendHelpBack= false;
        replyToHelp = false;
        lastDroveAt = simTime();
        delayVector.setName("delayVec");
        currentSubscribedServiceId = -1;
        minSpace = -9999;
        minId = -9999;
        counter = 0;
        minCoord = {0,0,0};
        v0Coord = {0,0,0};
        totalNB = 0;
        firstPacket = simTime();
        lastPacket = simTime();
    }
}

void TraCIDemo11p::onWSA(DemoServiceAdvertisment* wsa)
{
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void TraCIDemo11p::onWSM(BaseFrame1609_4* frame)
{
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);
  
    if (strcmp(wsm->getDemoData(), "help") == 0) {
        findHost()->getDisplayString().setTagArg("i", 1, "green");

        simtime_t delay = simTime() - wsm -> getCreationTime();
        delayVector.record(delay);
        
        totalNB += wsm->getBitLength();
        //traciVehicle->setSpeed(0.8);

        TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
        populateWSM(wsm);

        // repeat the received traffic update once in 2 seconds plus some random delay
          if (!sentMessageDoc) {
              sentMessageDoc = true;
              if ((mobility->getExternalId() == "veh2")) {
//                
//
                //int myId = getParentModule()->getId();
                wsm->setSenderAddress(2);
	            wsm->setDemoCoord(mobility->getPositionAt(simTime()));
//                //wsm->setDemoData("SendHelpBackveh4");
	            sendDown(wsm);

                //sentMessageDoc = true;
            }
           if (mobility->getExternalId() == "veh4") {
                //int myId = getParentModule()->getId();
                wsm->setSenderAddress(4);
	            wsm->setDemoCoord(mobility->getPositionAt(simTime()));
                //wsm->setDemoData("SendHelpBackveh4");
                sendDelayedDown(wsm, 0.001);
                //sendDown(wsm);
            }
       }

    }


    //if (((strcmp(wsm->getDemoData(), "SendHelpBackveh4") == 0) && (mobility->getExternalId() == "veh0")) ||  ((strcmp(wsm->getDemoData(), "helpbackHOS") == 0) && (mobility->getExternalId() == "veh0"))    ) {
    //    findHost()->getDisplayString().setTagArg("i", 1, "yellow");
    //}

    if (( wsm->getDemoCoord().x >= 1) && (mobility->getExternalId() == "veh0") ) {
        simtime_t delay = simTime() - wsm -> getCreationTime();
        delayVector.record(delay);
        totalNB += wsm->getBitLength();
        
        findHost()->getDisplayString().setTagArg("i", 1, "yellow");
        v0Coord = mobility->getPositionAt(simTime());

        mobility = TraCIMobilityAccess().get(getParentModule());
        traci = mobility->getCommandInterface();
        int disTwo = traci->getDistance(wsm->getDemoCoord(), v0Coord, true );
        int dis = traci->getDistance(v0Coord, wsm->getDemoCoord(), true );
        if(dis<0 && disTwo>=0){
            dis= disTwo;
        }

        //findHost()->bubble(to_string(dis).c_str());

        if ((minSpace == -9999) && (dis>=0)){
            minSpace = dis;
            minId= wsm->getSenderAddress();
            minCoord= wsm->getDemoCoord();
        }
        if ((dis < minSpace) && (dis>=0)){
            minSpace = dis;
            minId= wsm->getSenderAddress();
            minCoord= wsm->getDemoCoord();
        }
        counter= counter+1;
        cout<<counter<<endl;
        //findHost()->bubble(to_string(dis).c_str());
        //TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
        //populateWSM(wsm);
        //wsm->setDemoData(to_string(minId).c_str());
        //sendDown(wsm);
        //for(std::size_t i = 0; i < data.size(); ++i) {
        //    findHost()->bubble(to_string(data[i]).c_str());
        //}

     }

    if (counter==3)
    {
        if ((mobility->getExternalId() == "veh0")) {
            if(minId==0){
                TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
                populateWSM(wsm);
                wsm->setDemoData("hos patient Data patient Data patient Data patient Data 0011 2235 445 54554 patient Data patient Data patient Data 55892 patient Data 654 patient Data 5582");
                sendDown(wsm);
                //mobility = TraCIMobilityAccess().get(getParentModule());
                //traci = mobility->getCommandInterface();
                //traciVehicle = mobility->getVehicleCommandInterface();
                //traciVehicle->changeVehicleRoute({"1to2","2to4"});
                cout<<"RSU-NB: "<<totalNB<<endl;


                mobility = TraCIMobilityAccess().get(getParentModule());
                traciVehicle = mobility->getVehicleCommandInterface();
                traciVehicle->changeVehicleRoute({"1to2","2to4"});
                traciVehicle->stopAt("2to4", 190, (uint8_t)atoi("2") , 45, 10000);

            }

            else if (minId==2)
            {
                TraCIDemo11pMessage* wsm1 = new TraCIDemo11pMessage();
                populateWSM(wsm1);
                wsm1->setDemoData("patient Data patient Data patient Data patient Data 0011 2235 445 54554 patient Data patient Data patient Data 55892 patient Data 654 patient Data 5582");
                sendDown(wsm1);

                TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
                populateWSM(wsm);
                wsm->setDemoData("setStop2");
                sendDown(wsm);
           
                mobility = TraCIMobilityAccess().get(getParentModule());
                traciVehicle = mobility->getVehicleCommandInterface();
                traciVehicle->changeVehicleRoute({"1to2","2to3"});
                traciVehicle->stopAt("2to3", 20, (uint8_t)atoi("2") , 45, 10000);
            }
            else if (minId==4)
            {
                TraCIDemo11pMessage* wsm1 = new TraCIDemo11pMessage();
                populateWSM(wsm1);
                wsm1->setDemoData("patient Data patient Data patient Data patient Data 0011 2235 445 54554 patient Data patient Data patient Data 55892 patient Data 654 patient Data 55824");
                sendDown(wsm1);

                TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
                populateWSM(wsm);
                wsm->setDemoData("setStop4");
                sendDown(wsm);
                
                mobility = TraCIMobilityAccess().get(getParentModule());
                traciVehicle = mobility->getVehicleCommandInterface();
                traciVehicle->changeVehicleRoute({"1to2","2to3"});
                traciVehicle->stopAt("2to3", 20, (uint8_t)atoi("2") , 45, 10000);
            }
       }

     }

    if((strcmp(wsm->getDemoData(), "patient Data patient Data patient Data patient Data 0011 2235 445 54554 patient Data patient Data patient Data 55892 patient Data 654 patient Data 5582") == 0) && (mobility->getExternalId() == "veh2")){
        simtime_t delay = simTime() - wsm -> getCreationTime();
        delayVector.record(delay);
        totalNB += wsm->getBitLength();
    }

    if((strcmp(wsm->getDemoData(), "patient Data patient Data patient Data patient Data 0011 2235 445 54554 patient Data patient Data patient Data 55892 patient Data 654 patient Data 55824") == 0) && (mobility->getExternalId() == "veh4")){
        simtime_t delay = simTime() - wsm -> getCreationTime();
        delayVector.record(delay);
        totalNB += wsm->getBitLength();
    }
    if((strcmp(wsm->getDemoData(), "setStop2") == 0) && (mobility->getExternalId() == "veh2")){
        lastPacket = simTime();
        simtime_t delay = simTime() - wsm -> getCreationTime();
        delayVector.record(delay);
        totalNB += wsm->getBitLength();
        
        mobility = TraCIMobilityAccess().get(getParentModule());
        traciVehicle = mobility->getVehicleCommandInterface();
        traciVehicle->changeVehicleRoute({"1to2","2to3"});
        traciVehicle->stopAt("2to3", 20, (uint8_t)atoi("2") , 45, 10000);

        cout<<"lastPacket ="<< simTime()<<endl;
        cout<<"fullPacket ="<< simTime()-firstPacket<<endl;
        cout<<"NB: "<<totalNB<<endl;
        simtime_t x = (lastPacket-firstPacket);
        simtime_t x2 = (simTime()-firstPacket);
        cout<<"lastPacket-firstPacket: "<<x<<endl;
        cout<<"simTime()-firstPacket x2: "<<x2<<endl;
        cout<<"simTime()-firstPacket: "<<(simTime()-firstPacket)<<endl;
    }
    if((strcmp(wsm->getDemoData(), "setStop4") == 0) && (mobility->getExternalId() == "veh4")){
        simtime_t delay = simTime() - wsm -> getCreationTime();
        delayVector.record(delay);
        lastPacket = simTime();
        mobility = TraCIMobilityAccess().get(getParentModule());
        traciVehicle = mobility->getVehicleCommandInterface();
        traciVehicle->changeVehicleRoute({"1to2","2to3"});
        traciVehicle->stopAt("2to3", 20, (uint8_t)atoi("2") , 45, 10000);

        cout<<"lastPacket ="<< simTime()<<endl;
        cout<<"fullPacket ="<< simTime()-firstPacket<<endl;
        cout<<"NB: "<<totalNB<<endl;
        simtime_t x = (lastPacket-firstPacket);
        simtime_t x2 = (simTime()-firstPacket);
        cout<<"lastPacket-firstPacket: "<<x<<endl;
        cout<<"simTime()-firstPacket x2: "<<x2<<endl;
        cout<<"simTime()-firstPacket: "<<(simTime()-firstPacket)<<endl;
    }

}

void TraCIDemo11p::handleSelfMsg(cMessage* msg)
{

}

void TraCIDemo11p::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
    if((mobility->getExternalId() == "veh0")){
        findHost()->getDisplayString().setTagArg("i", 1, "blue");
    }

        // stopped for for at least 10s?
    if (mobility->getSpeed() == 0.99) {

        if(sentMessage2==false){
            sentMessage2 = true;
            TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
            populateWSM(wsm);
            wsm->setDemoData("help");
            sendDown(wsm);
            firstPacket = simTime();
            cout<<"firstPacket ="<< simTime()<<endl;
        }    
    }

}





