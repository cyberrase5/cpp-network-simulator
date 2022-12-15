#ifndef CLIENT_CPP
#define CLIENT_CPP

#include "nodes.hpp"

#define REQUEST_SIZE 1 //KILObytes, TBD

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
            std::cout << "New packet is a cheat packet, sent immediately"  << "\n-----" << std::endl;
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

    std::cout << "Packet could not be queued, client is at max capacity" << std::endl;

}

void Client::receive(Packet* packet) { // READY I THINK
    if (!not_full_after_add(packet->getSize())) {
        std::cout << "Packet dropped at " << name_ << ", client is at max capacity" << std::endl;
        return;
    }

    std::cout << "Packet from " << packet->getSenderId() << " arrvied at " << name_
    << "\nContent: " << packet->getContent() << ", size: " << packet->getSize() << "KB"
    << "\nThe whole trip took " << time(NULL) - packet->getTimeSent() << " seconds"
    << std::endl;


    delete packet; // maybe delete this
}

void Client::send(Packet* packet) { // cheat version, DONE I THINK

    auto next = main_map.find(connection_id_);
    packet->wait(connection_cost_.first, connection_cost_.second);
    next->second->receive(packet);
}

void Client::send() { // normal version, DONE I THINK

    if (packets_.size() == 0) {
        std::cout << "No packets to send" << std::endl;
        return;
    }

    Packet* packet = packets_.front();
    packets_.pop();

    if (packet->isDebug()) {
        std::cout << "Packet from " << name_ << "departing to "
        << packet->getReceiverId() << std::endl;
    }

    change_current_capacity(-packet->getSize());

    auto next = main_map.find(connection_id_);

    packet->wait(connection_cost_.first, connection_cost_.second);
    next->second->receive(packet);
    
    
}

void Client::print() {
    std::cout << "#####\nClient " << name_ << " id: " << id_
    << "\nCurrent capacity: " << current_capacity_ << " / " << max_capacity_
    << "(KB)\nPackets in queue: " << queue_packet_count()
    << "\nConnected to router " << connection_id_
    << "\n#####" << std::endl;
}

#endif