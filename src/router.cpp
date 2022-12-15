#ifndef ROUTER_CPP
#define ROUTER_CPP

#define SCALING_FACTOR 1000.0  // TBD

#include "nodes.hpp"
#include <vector>


// auxiliary functions


void Router::receive(Packet* packet) {

    if (!not_full_after_add(packet->getSize())) {
        std::cout << "Packet dropped at router " << id_ << ", router is at max capacity" << std::endl;
        return;
    }

    if (packet->isDebug()) {
        std::cout << "Packet from " << packet->getSenderId() << " to " <<
        packet->getReceiverId() << "\nArrived to router " << id_
        << "\n-----" << std::endl;
    }

    if (packet->isCheat()) {
        send(packet);
        return;
    }

    current_capacity_ += packet->getSize();
    packets_.emplace(packet);
}


void Router::send() {

    if (packets_.size() == 0) {
        std::cout << "No packets to send" << std::endl;
        return;
    }

    Packet* packet = packets_.front();
    packets_.pop();
    change_current_capacity(-packet->getSize());

    if (packet->getReceiverId() == id_) {
        send_to_end_node(packet);
        return;
    }

    unsigned short next_id = routing_table_.find(packet->getReceiverId())->second;

    std::cout << "Sending to router " << next_id << std::endl;

    auto cost = neighbors_.find(next_id);
    packet->wait(cost->second.first, cost->second.second);
    main_map.find(next_id)->second->receive(packet);
}

void Router::send(Packet* packet) {
    unsigned short next_id = routing_table_.find(packet->getReceiverId())->second;

    if (packet->getReceiverId() == id_) {
        send_to_end_node(packet);
        return;
    }

    std::cout << "Sending to router " << next_id << std::endl;

    auto cost = neighbors_.find(next_id);
    packet->wait(cost->second.first, cost->second.second);
    main_map.find(next_id)->second->receive(packet);

}

void Router::send_to_end_node(Packet* packet) {

    if (packet->isDebug()) {
        std::cout << "Packet arrived at destination router, sending to end node"
        << "\n-----" << std::endl;
    }
    
    auto next = main_map.find(connection_id_);
    packet->wait(connection_cost_.first, connection_cost_.second);
    next->second->receive(packet);
}

unsigned short getMin(const std::map<unsigned short, double>& pseudo_heap) {
    double cost = pseudo_heap.begin()->second;
    unsigned short ret = pseudo_heap.begin()->first;

    for (auto it: pseudo_heap) {
        if (it.second < cost) {
            ret = it.first;
        }
    }
    
    return ret;
}

bool visited_func(std::vector<unsigned short> &v, unsigned short target) {
    for (auto i: v) {
        if (i == target) return true;
    }

    return false;
}

// pseudo_heap is slow, but since C++ heap does not support
// decresing key, here we are
void Router::create_routing_table(
    const std::map<unsigned short, Router*>& routers) {  // dijkstra
    std::map<unsigned short, unsigned short>
        previous;  // node id, receives from
    std::map<unsigned short, double> pseudo_heap; // node id, cost to it
    std::vector<unsigned short> visited;

    for (auto router: routers) {
        pseudo_heap[router.first] = 1000000.0;
        previous[router.first] = 0; // unknown
    }
    pseudo_heap[id_] = 0;

    unsigned short current_id;
    double cost;

    while (!pseudo_heap.empty()) {
        current_id = getMin(pseudo_heap); // gets id

        for (auto edge: routers.find(current_id)->second->neighbors_) {
            if (visited_func(visited, edge.first)) {
                continue;
            }  
            cost = SCALING_FACTOR / edge.second.first;
            if (pseudo_heap[edge.first] > pseudo_heap[current_id] + cost) {
                pseudo_heap[edge.first] = pseudo_heap[current_id] + cost; // update cost to smaller
                previous[edge.first] = current_id; // update prev
                
            }
        }

        visited.push_back(current_id);
        auto it = pseudo_heap.find(current_id);
        pseudo_heap.erase(it);
    }


    // trace back previous

    unsigned short temp;

    for (auto pair: previous) {
        if (previous[pair.first] == 0) continue;
        temp = pair.first;

        while (true) {
            if (previous[temp] == id_) {
                routing_table_[pair.first] = temp;
                break;
            }
            temp = previous[temp];
        }
    }
}

void Router::print_routing_table() const {
    std::cout << "Router " << id_ << " routing table:\n";

    for (auto entry: routing_table_) {
        std::cout << "| " << entry.first << " | "
        << entry.second << " |" << std::endl;
    }
}

void Router::print_connections() const {
    std::cout << " Router id " << id_ << " direct connections:\n";

    for (auto entry: neighbors_) {
        std::cout << "| router id: " << entry.first << " | " 
        << entry.second.first << "Kb/s, "
        << entry.second.second << "ms |" << std::endl;
    }

    if (connection_id_ != 0) {
        std::cout << "Connection to end node: " << connection_cost_.first
        << "Kb/s, " << connection_cost_.second << "ms" << std::endl;
    }
}

void Router::print() {
    std::cout << "Router id " << id_
    << "\nCurrent capacity: " << current_capacity_ << " / " << max_capacity_
    << " (KB)\nPackets in queue: " << queue_packet_count() << std::endl;

    if (connection_id_ != 0) {
        std::cout << "Connected to end node id " << connection_id_
        << std::endl;
    }
    std::cout << "-----" << std::endl;
}


#endif