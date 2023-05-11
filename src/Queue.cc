#ifndef QUEUE
#define QUEUE

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Queue: public cSimpleModule {
private:
    cQueue buffer;
    cOutVector bufferSizeVector;
    cOutVector packetDropVector;
    cMessage *endServiceEvent;
    simtime_t serviceTime;
public:
    Queue();
    virtual ~Queue();
protected:
    virtual void initialize();
    virtual cQueue getBuffer(){ return this->buffer; };
    virtual simtime_t getServiceTime(){return this->serviceTime; };
    virtual cMessage *getEndServiceEvent() { return endServiceEvent; }
    virtual void setServiceTime(simtime_t serviceTime){ this->serviceTime = serviceTime; };
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Queue);

Queue::Queue() {
    endServiceEvent = NULL;
}

Queue::~Queue() {
    cancelAndDelete(endServiceEvent);
}

void Queue::initialize() {
    buffer.setName("buffer");
    packetDropVector.setName("packetDropVector");
    endServiceEvent = new cMessage("endService");
}

void Queue::finish() {
}

void Queue::handleMessage(cMessage *msg) {

    // if msg is signaling an endServiceEvent
    if (msg == endServiceEvent) {
        // if packet in buffer, send next one
        if (!buffer.isEmpty()) {
            // dequeue packet
            cPacket *pkt = (cPacket *)buffer.pop();
            // send packet
            send(pkt, "out");
            // start new service
            serviceTime = pkt->getDuration();
            scheduleAt(simTime() + serviceTime, endServiceEvent);
        }
    } else {  // if msg is a data packet
        // enqueue the packet

        // check buffer limit
        // cambiado a intValue pq sino no me tira error
        if (buffer.getLength() >= par("bufferSize").intValue()) {
            // drop the packet
            delete msg;
            this->bubble("packet dropped");
            packetDropVector.record(1);
        }
        else{
            buffer.insert(msg);
            bufferSizeVector.record(buffer.getLength());
            // if the server is idle
            if (!endServiceEvent->isScheduled()) {
                // start the service
                scheduleAt(simTime() + 0, endServiceEvent);
            }
        }
    }
};


class TransportPacket : public cPacket {
    bool slowDown;
    bool speedUp;
    int bufferSize;

    public:
        TransportPacket(const char *name = "TransportPacket", short kind = 2) : cPacket(name, kind) {}

        int getBufferSize() const { return this->bufferSize; }
        void setBufferSize(int bufferSize) {this->bufferSize = bufferSize; }

        bool getSlowDown() const { return this->slowDown; }
        void setSlowDown(bool b) { this->slowDown = b; }

        bool getSpeedUp() const { return this->speedUp; }
        void setSpeedUp(bool b) { this->speedUp = b; }
};

class TransportRx : Queue{

    void handleMessage() {
        TransportPacket *pkt = new TransportPacket();
        pkt->setByteLength(20);
        pkt->setBufferSize(par("buffersize").intValue() - this->getBuffer().getLength());
        pkt->setSlowDown(false);
        pkt->setSpeedUp(false);

        if (pkt->getBufferSize() < this->getBuffer().getLength() * 0.30) {
            pkt->setSpeedUp(true);
        };

        if (pkt->getBufferSize() > this->getBuffer().getLength() * 0.80) {
            pkt->setSlowDown(true);
        };
        send(pkt, "toOut$o");
    }
};


class TransportTx : Queue{

    void handleMessage(cMessage *msg) {
    // if msg is signaling an endServiceEvent
    if (msg->getKind() == 2){
        // msg is feedback
        TransportPacket* pkt = (TransportPacket*)msg;
        if (pkt->getSlowDown()) {
            // slow down
            this->setServiceTime(this->getServiceTime() * 1.5);
            scheduleAt(simTime() + this->getServiceTime(), this->getEndServiceEvent());
        }
        else if (pkt->getSpeedUp()) {
            // speed up
            this->setServiceTime(this->getServiceTime() * 0.5);
            scheduleAt(simTime() + this->getServiceTime(), this->getEndServiceEvent());
        }
        else {
            scheduleAt(simTime() + this->getServiceTime(), this->getEndServiceEvent());
        }
    }
    else if (msg->getKind() == 0) {
        this->handleMessage(msg);
    }
    }
};



#endif /* QUEUE */