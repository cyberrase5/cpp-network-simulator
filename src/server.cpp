#ifndef SERVER_CPP
#define SERVER_CPP

#include "nodes.hpp"

void Server::receive(Packet* packet) {

    
    std::cout << "Packet from " << packet->getSenderId() << " to "
    << packet->getReceiverId() << " arrived to server " << name_
    << "\n----" << std::endl;
    

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

void Server::send() {

    if (packets_.size() == 0) {
        std::cout << "No packets to send" << std::endl;
        return;
    }

    Packet* packet = packets_.front();
    packets_.pop();
    change_current_capacity(-packet->getSize());

    auto next = main_map.find(connection_id_);
    
    packet->wait(connection_cost_.first, connection_cost_.second);
    next->second->receive(packet);
}

void Server::send(Packet* packet) {

    auto next = main_map.find(connection_id_);
    
    packet->wait(connection_cost_.first, connection_cost_.second);
    next->second->receive(packet);
}

void Server::print() {
    std::cout << "#####\nServer " << name_
    << "\nConent type: " << content_type_ << " | content size: " << content_size_
    << "\nCurrent capacity: " << current_capacity_ << " / " << max_capacity_
    << " (KB)\nPackets in queue: " << queue_packet_count()
    << "\nConnected to router " << connection_id_ << "\n#####" << std::endl;
}

#endif