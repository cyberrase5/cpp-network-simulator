#define SCALING_FACTOR 1000.0  // to find shortest (fastest) paths, we need to calculate
                               // so that larger bandwidth = smaller number,
                               // this is done by SCALING_FACTOR / bandwidth

#include "nodes.h"
#include <vector>

/**
 * @brief called when router receives a packet, enqueues it or forwards immediately (if not full)
 * @param packet pointer to arriving packet
 */
void Router::receive(Packet* packet) {

    if (!not_full_after_add(packet->getSize())) {
        std::cout << "!!!\nPacket dropped at router " << id_ 
        << ", router is at max capacity\n!!!" << std::endl;

        nodeitem_->dropPacket();
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

/**
 * @brief pops packets_ queue, sends forward based on routing table or to end node
 * @note overloaded version exists for cheat packets
 * 
 */
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

    std::cout << "Sending from router " << id_ << " to router " << next_id << std::endl;

    auto cost = neighbors_.find(next_id);
    auto router = main_map.find(next_id);
    *nodeitem_ << router->second->getNodeItem();
    //packet->wait(cost->second.first, cost->second.second);
    //router->second->receive(packet);

    WaitThread *thread = new WaitThread(router->second, packet, cost->second.first, cost->second.second);
    thread->start();
}

/**
 * @brief "cheat" version of send(), used to bypass packet queues, sends forward based on routing table or to end node
 * @param packet pointer to the cheat packet
 * @note non-overloaded version exists for normal packets
 * 
 */
void Router::send(Packet* packet) {
    unsigned short next_id = routing_table_.find(packet->getReceiverId())->second;

    if (packet->getReceiverId() == id_) {
        send_to_end_node(packet);
        return;
    }

    std::cout << "Sending from router " << id_ << " to router " << next_id << std::endl;

    auto cost = neighbors_.find(next_id);
    //packet->wait(cost->second.first, cost->second.second);
    auto next = main_map.find(next_id);
    //next->second->receive(packet);
    WaitThread *thread = new WaitThread(next->second, packet, cost->second.first, cost->second.second);
    thread->start();

}

/**
 * @brief if in either send() is detected that packet's destination id_ is same as this router's,
 * this function is called. Sends the packet to this router's connected end node
 * 
 * @param packet pointer to packet to be sent
 */
void Router::send_to_end_node(Packet* packet) {

    if (packet->isDebug()) {
        std::cout << "-----\nPacket arrived to destination router " << id_ << ", sending to end node"
        << "\n-----" << std::endl;
    }

    auto next = main_map.find(connection_id_);
    *nodeitem_ << next->second->getNodeItem();
    //packet->wait(connection_cost_.first, connection_cost_.second);
    next->second->receive(packet);
}

/**
 * @brief used in routing table creation, gets the node id with lowest cost (mimics min_priority_queue's pop())
 * 
 * @param pseudo_heap ref to <node_id, cost> pairs, pseudo priority queue that allows decreasing key
 * @return unsigned short of the node with the lowest cost not yet visited
 */
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

/**
 * @brief auxiliary, checks has the target has been visited
 * 
 * @param v int vector of visited ids
 * @param target what number to check
 * @return true if visited
 * @return false if not visited
 */
bool visited_func(std::vector<unsigned short> &v, unsigned short target) {
    for (auto i: v) {
        if (i == target) return true;
    }

    return false;
}

/**
 * @brief find cheapest paths with Dijkstra and then from predecessor list update routing_table_ 
 * 
 * @param routers ref to existing routers, as clients and servers don't have routing tables
 */
void Router::create_routing_table(
    const std::map<unsigned short, Router*>& routers) {  // dijkstra
    std::map<unsigned short, unsigned short>
        previous;  // node id, receives from
    std::map<unsigned short, double> pseudo_heap; // node id, cost to it
    std::vector<unsigned short> visited;

    // distance to all and predecessor unknown
    for (auto router: routers) {
        pseudo_heap[router.first] = 1000000.0;
        previous[router.first] = 0; // unknown
    }
    pseudo_heap[id_] = 0;

    unsigned short current_id;
    double cost;

    // the "actual" dijkstra
    while (!pseudo_heap.empty()) {
        current_id = getMin(pseudo_heap); // gets id

        for (auto edge: routers.find(current_id)->second->neighbors_) { // iterate this node's direct neighbors
            if (visited_func(visited, edge.first)) {
                continue;
            }
            cost = SCALING_FACTOR / edge.second.first; // larger the bandwith = smaller the cost
            if (pseudo_heap[edge.first] > pseudo_heap[current_id] + cost) {
                pseudo_heap[edge.first] = pseudo_heap[current_id] + cost; // update cost to smaller
                previous[edge.first] = current_id; // update prev

            }
        }

        visited.push_back(current_id);
        auto it = pseudo_heap.find(current_id);
        pseudo_heap.erase(it);
    }


    // for all nodes, go so far back the predecessor list that the root node is found as predecessor,
    // then select that node (whose previous would be this node) to routing_table_

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

/**
 * @brief prints routing table, final destination | where to send (direct neighbors)
 * 
 */
void Router::print_routing_table() const {
    std::cout << "Router " << id_ << " routing table\n"
    << "Final destination | Where to send:\n";

    for (auto entry: routing_table_) {
        std::cout << "              | " << entry.first << " | "
        << entry.second << " |" << std::endl;
    }
}

/**
 * @brief print's router's direct connections and their speeds to other nodes
 * 
 */
void Router::print_connections() const { // obsolete
    std::cout << " Router id " << id_ << " direct connections:\n";

    for (auto entry: neighbors_) {
        std::cout << "| router id: " << entry.first << " | "
        << entry.second.first << "Kb/s, "
        << entry.second.second << "ms |" << std::endl;
    }

    if (connection_id_ != id_) {
        std::cout << "Connection to end node: " << connection_cost_.first
        << "Kb/s, " << connection_cost_.second << "ms" << std::endl;
    }
}


/**
 * @brief print useful stuff about router
 * 
 */
void Router::print() {
    std::cout << "#####\nRouter id " << id_
    << "\nCurrent capacity: " << current_capacity_ / 1000.0 << " / " 
    << max_capacity_ / 1000.0 << " (MB)\nPackets in queue: " 
    << queue_packet_count() << std::endl;

    std::cout << "----- Direct connections to other routers:\n";

    for (auto entry: neighbors_) {
        std::cout << "| router id: " << entry.first << " | "
        << entry.second.first / 1000 << " Mb/s, "
        << entry.second.second << "ms |" << std::endl;
    }

    if (connection_id_ != id_) {
        std::cout << "Connected to end node id " << connection_id_ << "\nCost: "
        << connection_cost_.first / 1000.0 << " Mb/s, " << connection_cost_.second << "ms\n#####"
        << std::endl;
    } else {
        std::cout << "\n#####" << std::endl;
    }
}
