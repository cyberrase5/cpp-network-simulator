#include "nodes.h"

#define REQUEST_SIZE 1 //KILObytes

/**
 * @brief creates Packet-object and stores it in queue or sends forward immediately
 * 
 * @param server_name string, get node_id_ from DNS with this
 * @param content string, for immersion reasons
 * @param flag unsigned int, used to bitwise select if packet is debug or cheat
 */
void Client::create_packet(std::string server_name, std::string content, unsigned int flag
                            ) {

    if (DNS.find(server_name) == DNS.end()) {
        std::cout << "404 not found" << std::endl;
        return;
    }
    
    unsigned short receiver_id = DNS.find(server_name)->second;
    Packet* new_packet = new Packet(REQUEST_SIZE, connection_id_, receiver_id, content);

    if (flag & 1) new_packet->setDebug(); // bitwise operation
    if (flag & 2) new_packet->setCheat();

    if (not_full_after_add(new_packet->getSize())) {
        if (new_packet->isCheat()) {
            std::cout << "-----\nNew packet is a cheat packet, sent immediately"  << "\n-----" << std::endl;
            new_packet->wait(connection_cost_.first, connection_cost_.second); // propagation & latency wait
            send(new_packet);
            return;
        }

        // normal case
        packets_.emplace(new_packet);
        current_capacity_ += new_packet->getSize();
        std::cout << "Packet added to " << name_ << "'s queue" << std::endl;
        return;
    }

    std::cout << "!!!!!\nPacket could not be queued, client is at max capacity" 
    << ", enter another command\n!!!!!" << std::endl;

}

/**
 * @brief called when client receives packet, prints useful information
 * @param packet pointer to the arriving packet
 * 
 */
void Client::receive(Packet* packet) {
    if (!not_full_after_add(packet->getSize())) {
        std::cout << "!!!\nPacket dropped at " << name_ << ", client is at max capacity\n!!!" << std::endl;
        return;
    }

    std::cout << "Packet from " << packet->getSenderId() << " arrvied at " << name_
    << "\nContent: " << packet->getContent() << ", size: " << packet->getSize() << "KB"
    << "\nThe whole trip took " << time(NULL) - packet->getTimeSent() << " seconds"
    << std::endl;


    delete packet; // maybe delete this
}

/**
 * @brief "cheat" version of send(), used to bypass packet queues, send to router
 * @param packet pointer to the cheat packet
 * @note non-overloaded version exists for normal packets
 * 
 */
void Client::send(Packet* packet) {

    if (packet->isDebug()) {
        std::cout << "-----\nPacket from " << name_ << " departing to "
        << packet->getReceiverId() << "\n-----" << std::endl;
    }

    auto next = main_map.find(connection_id_);
    packet->wait(connection_cost_.first, connection_cost_.second);
    next->second->receive(packet);
}

/**
 * @brief pops packets_ queue, sends to this client's connected router
 * @note overloaded version exists for cheat packets
 * 
 */
void Client::send() { // normal version

    if (packets_.size() == 0) {
        std::cout << "No packets to send" << std::endl;
        return;
    }

    Packet* packet = packets_.front();
    packets_.pop();

    if (packet->isDebug()) {
        std::cout << "----\nPacket from " << name_ << " departing to "
        << packet->getReceiverId() << std::endl;
    }

    change_current_capacity(-packet->getSize());

    auto next = main_map.find(connection_id_);
    *nodeitem_ << next->second->getNodeItem();

    //packet->wait(connection_cost_.first, connection_cost_.second);
    //next->second->receive(packet);

    WaitThread *thread = new WaitThread(next->second, packet, connection_cost_.first, connection_cost_.second);
    thread->start();
}

/**
 * @brief print useful stuff about client
 * 
 */
void Client::print() {
    std::cout << "#####\nClient " << name_ << " id: " << id_
    << "\nCurrent capacity: " << current_capacity_ / 1000.0 << " / " << max_capacity_ / 1000.0
    << "(MB)\nPackets in queue: " << queue_packet_count()
    << "\nConnected to router " << connection_id_ << ", connection speed:\n"
    << connection_cost_.first / 1000.0 << " Mb/s, " << connection_cost_.second << "ms\n#####"
    << std::endl;
}

