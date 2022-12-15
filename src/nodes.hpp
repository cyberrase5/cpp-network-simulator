#ifndef NODES_HPP
#define NODES_HPP

#include "packet.hpp"

#include <string>
#include <map>
#include <queue>
#include <iostream>

extern std::map<std::string, unsigned short> DNS;

class Node {

protected:
    unsigned short id_;
    std::queue<Packet*> packets_;
    unsigned int max_capacity_; // KILObytes
    unsigned int current_capacity_; // KILObytes
    unsigned short connection_id_; // for routers end node id, end nodes router id
    std::pair<unsigned int, unsigned short> connection_cost_;

public:
    Node(unsigned short id, unsigned int max_capacity, unsigned short connection_id, std::pair<unsigned int, unsigned short> connection_cost)
        :   id_(id), max_capacity_(max_capacity), connection_id_(connection_id), current_capacity_(0), connection_cost_(connection_cost) {}
    
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

};
    
extern std::map<unsigned short, Node*> main_map; // all nodes in simulation <node_id, Node>


class Router : public Node {

private:
    std::map<unsigned short, unsigned short> routing_table_; // <receiver_id, where_to_send_id>
    std::map<unsigned short, std::pair<unsigned int, unsigned short>> neighbors_; // <neighbor_id, <bandwidth, latency>>KILObits and ms
    
    
public:
    Router(unsigned short id, unsigned int max_capacity, unsigned short connection_id, std::pair<unsigned int, unsigned short> connection_cost)
    :   Node(id, max_capacity, connection_id, connection_cost) {}

    void create_routing_table(const std::map<unsigned short, Router*>& routers);

    std::map<unsigned short, std::pair<unsigned int, unsigned short>>& getNeighbors() { return neighbors_; }

    void print_routing_table() const;
    void print_connections() const;

    void addNeighbor(unsigned short id, const std::pair<unsigned int, unsigned short>& pair) { neighbors_.insert({id, pair}); }

    void send(); // normal version
    void send(Packet* packet); // cheat version
    void receive(Packet* packet);

    virtual void print();

    void send_to_end_node(Packet* packet);
    

};


class Client : public Node {

private:
    std::string name_;

public:
    Client(unsigned short id, unsigned int max_capacity, unsigned short connection_id, std::pair<unsigned int, unsigned short> connection_cost, std::string name)
        : name_(name), Node(id, max_capacity, connection_id, connection_cost) {}
    
    void create_packet(std::string server_name, std::string content, 
                                                unsigned int flag); 

    void send(); // normal version
    void send(Packet* packet); // cheat version
    void receive(Packet* packet);

    virtual void print();

    std::string getName() const { return name_; }
    
};


class Server : public Node {

private:
    std::string name_;
    std::string content_type_; // .mp4 for Youtube, .jpg for Instagram etc.
    unsigned int content_size_; // small for pictures, large for videos, KILObytes

public:
    Server(unsigned short id, unsigned int max_capacity,  unsigned short connection_id,
    std::pair<unsigned int, unsigned short> connection_cost, std::string name, std::string content_type, unsigned int content_size)
    :   name_(name), content_type_(content_type), content_size_(content_size), Node(id, max_capacity, connection_id, connection_cost) {}

    void send(); // normal version
    void send(Packet* packet); // cheat version
    void receive(Packet* packet);

    virtual void print();

    void add(Packet* packet) { packets_.push(packet); }


    std::string getContentType() const { return content_type_; }
    unsigned int getContentSize() const { return content_size_; }

    std::string getName() const { return name_; }

};



#endif