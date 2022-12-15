#ifndef NODES_HPP
#define NODES_HPP

#include "packet.h"
#include "nodeitem.h"

#include <string>
#include <map>
#include <queue>
#include <iostream>


#include <QThread>




extern std::map<std::string, unsigned short> DNS;

class Node {

protected:
    unsigned short id_;
    std::queue<Packet*> packets_;
    unsigned int max_capacity_; // KILObytes
    unsigned int current_capacity_; // KILObytes
    unsigned short connection_id_; // for routers end node id, end nodes router id
    std::pair<unsigned int, unsigned short> connection_cost_;
    NodeItem *nodeitem_;



public:
    Node(unsigned short id, unsigned int max_capacity, unsigned short connection_id, std::pair<unsigned int, unsigned short> connection_cost, NodeItem *nodeitem)
        :   id_(id), max_capacity_(max_capacity), connection_id_(connection_id), current_capacity_(0), connection_cost_(connection_cost), nodeitem_(nodeitem) {}

    virtual void send() = 0; // pop from queue, call receiver receive()
    virtual void send(Packet* packet) = 0; // cheat version
    virtual void receive(Packet* packet) = 0; // different for different nodes

    virtual void print() = 0;

    virtual ~Node() = default;

    // getters, setters etc.

    // COMPARING HAPPENS IN KILOBYTES
    bool not_full_after_add(unsigned int added_size) { // arg = KB
        return current_capacity_  + added_size <= max_capacity_;
    }


    void change_max_capacity(unsigned int new_capacity) { // kilobytes
        max_capacity_ = new_capacity;
    }

    void change_current_capacity(unsigned int change) {
        current_capacity_ += change;
    }

    unsigned short getId() const { return id_; }
    unsigned int getCurrentCapacity() const { return current_capacity_; }
    unsigned int getMaxCapacity() const { return max_capacity_; }
    size_t queue_packet_count() const { return packets_.size(); }
    unsigned short getConnectionId() const { return connection_id_; }

    virtual std::string getName() const = 0;
    NodeItem* getNodeItem() { return nodeitem_; }

};

class WaitThread : public QThread
{
    Q_OBJECT
    void run() override
    {
        unsigned int transmission_delay = size_ * 8 / bandwidth_; // KB -> Kb

        if (transmission_delay < 1) {
            double t_d = size_ * 8 / bandwidth_; // same as above, seconds
            t_d *= 1000000; // seconds to microseconds
            transmission_delay = t_d; // to unsigned int
            QThread::usleep(transmission_delay); // sleep microseconds
        } else {
            QThread::sleep(transmission_delay); // sleep seconds
        }

        QThread::usleep(1000 * latency_); // sleep microseconds
        node_->receive(packet_);
        return;
    }

public:
    WaitThread(Node* node, Packet* packet, unsigned int bandwidth, unsigned short latency, QObject* parent = nullptr) : QThread(parent), node_(node), packet_(packet), bandwidth_(bandwidth), latency_(latency) { size_ = packet->getSize(); }


private:
    unsigned int size_;
    unsigned int bandwidth_;
    unsigned short latency_;
    Node *node_;
    Packet *packet_;
};


extern std::map<unsigned short, Node*> main_map; // all nodes in simulation <node_id, Node>

class Router : public Node {

private:
    std::map<unsigned short, unsigned short> routing_table_; // <receiver_id, where_to_send_id>
    std::map<unsigned short, std::pair<unsigned int, unsigned short>> neighbors_; // <neighbor_id, <bandwidth, latency>>KILObits and ms


public:
    Router(unsigned short id, unsigned int max_capacity, unsigned short connection_id, std::pair<unsigned int, unsigned short> connection_cost, NodeItem *nodeitem)
    :   Node(id, max_capacity, connection_id, connection_cost, nodeitem) {}

    void create_routing_table(const std::map<unsigned short, Router*>& routers);

    std::map<unsigned short, std::pair<unsigned int, unsigned short>>& getNeighbors() { return neighbors_; }

    void print_routing_table() const;
    void print_connections() const;

    void addNeighbor(unsigned short id, const std::pair<unsigned int, unsigned short>& pair) { neighbors_.insert({id, pair}); }

    void send(); // normal version
    void send(Packet* packet); // cheat version
    void receive(Packet* packet);

    void send_to_end_node(Packet* packet);

    void print();

    std::string getName() const { return std::to_string(this->getId()); }


};

class Client : public Node {

private:
    std::string name_;

public:
    Client(unsigned short id, unsigned int max_capacity, unsigned short connection_id, std::pair<unsigned int, unsigned short> connection_cost, std::string name, NodeItem *nodeitem)
        : name_(name), Node(id, max_capacity, connection_id, connection_cost, nodeitem) {}

    void create_packet(std::string server_name, std::string content, 
                                                unsigned int flag); 

    void send(); // normal version
    void send(Packet* packet); // cheat version
    void receive(Packet* packet);
    void print();

    std::string getName() const { return name_; }

};

class Server : public Node {

private:
    std::string name_; // Youtube, Instagram etc.
    std::string content_type_; // .mp4 for Youtube, .jpg for Instagram etc.
    unsigned int content_size_; // small for pictures, large for videos, KILObytes

public:
    Server(unsigned short id, unsigned int max_capacity,  unsigned short connection_id,
    std::pair<unsigned int, unsigned short> connection_cost, std::string name, std::string content_type, unsigned int content_size, NodeItem *nodeitem)
    :   name_(name), content_type_(content_type), content_size_(content_size), Node(id, max_capacity, connection_id, connection_cost, nodeitem) {}

    void send(); // normal version
    void send(Packet* packet); // cheat version
    void receive(Packet* packet);
    void print();

    void add(Packet* packet) { packets_.push(packet); }

    std::string getContentType() const { return content_type_; }
    unsigned int getContentSize() const { return content_size_; }

    std::string getName() const { return name_; }

};

#endif
