//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
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

#include <string>
#include "veins/modules/application/traci/TraCIDemoRSU11p.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

using namespace veins;
using namespace std;

Define_Module(veins::TraCIDemoRSU11p);


void TraCIDemoRSU11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
		recieved = false;
        delayVector.setName("delayVec");
    }
}


void TraCIDemoRSU11p::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

void TraCIDemoRSU11p::onWSM(BaseFrame1609_4* frame)
{
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);
    if (strcmp(wsm->getDemoData(), "help") == 0) {
        simtime_t delay = simTime() - wsm -> getCreationTime();
        delayVector.record(delay);
        cout<< "reu thp= "<< wsm->getBitLength()<< endl;

        findHost()->getDisplayString().updateWith("r=12,green");
        findHost()->bubble(wsm->getDemoData());
        Coord pos = check_and_cast<BaseMobility*>(getSimulation()->getModuleByPath("rsu[0].mobility"))->getCurrentPosition();
        
        TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
        populateWSM(wsm);
        wsm->setSenderAddress(0);
        wsm->setDemoCoord(pos);
        //wsm->setDemoData("helpbackHOS");
        sendDown(wsm);
      
    }

    if (strcmp(wsm->getDemoData(), "hos patient Data patient Data patient Data patient Data 0011 2235 445 54554 patient Data patient Data patient Data 55892 patient Data 654 patient Data 5582") == 0) {
        simtime_t delay = simTime() - wsm -> getCreationTime();
        delayVector.record(delay);
        cout<< "reu thp= "<< wsm->getBitLength()<< endl;
        cout<<"lastPacket ="<< simTime()<<endl;
        findHost()->getDisplayString().updateWith("r=12,blue");
    }
}



