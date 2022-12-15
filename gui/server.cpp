#include "nodes.h"

/*! \brief called when server receives packet, changes packet data.
 * 
 * Swap receiver and sender ids, append packet's content_ with content_type_,
 * change packet's size_ to content_size_
 * Enqueue packet or send forward if packet is cheat packet
 * @param packet pointer to the arriving packet
 */
void Server::receive(Packet* packet) {


    std::cout << "-----\nPacket from " << packet->getSenderId() << " to "
    << packet->getReceiverId() << " arrived to server " << name_
    << "\n-----" << std::endl;


    // swap sender and receiver ids
    unsigned short temp = packet->getSenderId();
    packet->setSenderId(packet->getReceiverId());
    packet->setReceiverId(temp);

    // append content with server's content type
    packet->getContent().append(getContentType());

    // change packet size to more appropriate
    packet->setSize(getContentSize());

    if (packet->isCheat()) {
        send(packet);
        return;
    }

    current_capacity_ += packet->getSize();
    add(packet); // add to queue
}

/**
 * @brief pops packets_ queue, sends to this server's connected router
 * @note overloaded version exists for cheat packets
 * 
 */
void Server::send() {

    if (packets_.size() == 0) {
        std::cout << "No packets to send" << std::endl;
        return;
    }

    Packet* packet = packets_.front();
    packets_.pop();
    change_current_capacity(-packet->getSize());

    auto next = main_map.find(connection_id_);
    *nodeitem_ << next->second->getNodeItem();

    //packet->wait(connection_cost_.first, connection_cost_.second); "real version", uncommented is GUI version
    //next->second->receive(packet);

    WaitThread *thread = new WaitThread(next->second, packet, connection_cost_.first, connection_cost_.second);
    thread->start();
}

/**
 * @brief "cheat" version of send(), used to bypass packet queues, send to router
 * @param packet pointer to the cheat packet
 * @note non-overloaded version exists for normal packets
 * 
 */
void Server::send(Packet* packet) {

    auto next = main_map.find(connection_id_);

    //packet->wait(connection_cost_.first, connection_cost_.second);
    next->second->receive(packet);
}


/**
 * @brief print useful stuff about server
 * 
 */
void Server::print() {
    std::cout << "#####\nServer " << name_
    << "\nConent type: " << content_type_ << " | content size: " << content_size_ / 1000.0
    << " (MB)\nCurrent capacity: " << current_capacity_ / 1000.0 << " / " << max_capacity_ / 1000.0
    << " (MB)\nPackets in queue: " << queue_packet_count()
    << "\nConnected to router " << connection_id_ << ", connection speed:\n"
    << connection_cost_.first / 1000.0 << " Mb/s, " << connection_cost_.second << "ms\n#####"
    << std::endl;
}

