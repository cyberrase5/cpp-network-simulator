#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <time.h>
#include <unistd.h>
#include <iostream>


class Packet {

    private:
    unsigned int size_; // !!!CHANGED!!! KILObytes
    unsigned short sender_id_;
    unsigned short receiver_id_;
    std::string content_;
    time_t time_sent_; 
    bool debug_; // print useful data when moving
    bool cheat_; // this will bypass the queue 

    public:

    Packet(unsigned int size, unsigned short sender_id, unsigned short receiver_id, std::string content) :
    size_(size), sender_id_(sender_id), receiver_id_(receiver_id), content_(content), debug_(false), cheat_(false) {
        time_sent_ = time(NULL);
    }

    ~Packet() = default;
    
    // getters, setters etc.
    unsigned short getSenderId() const { return sender_id_; }
    unsigned short getReceiverId() const { return receiver_id_; }
    bool isDebug() const { return debug_; }
    bool isCheat() const { return cheat_; }
    unsigned int getSize() const { return size_; }
    time_t getTimeSent() const { return time_sent_; }

    void setSenderId(unsigned short new_id) { sender_id_ = new_id; }
    void setReceiverId(unsigned short new_id) { receiver_id_ = new_id; }
    void setDebug() { debug_ = true; }
    void setCheat() { cheat_ = true; }

    
    std::string& getContent() { return content_; }
    void setSize(unsigned int new_size) { size_ = new_size; }

    std::string getContent_print() const { return content_; }

    void wait(unsigned int bandwidth, unsigned short latency) { // BW: Mb/s, latency: ms, size_ KB
        // wait transfer
        unsigned int transmission_delay = size_ * 8 / bandwidth; // KB -> Kb
        
        if (transmission_delay < 1) {
            double t_d = size_ * 8 / bandwidth; // same as above, seconds
            t_d *= 1000000; // seconds to microseconds
            transmission_delay = t_d; // to unsigned int
            usleep(transmission_delay); // sleep microseconds
        } else {
            sleep(transmission_delay); // sleep seconds
        }

        usleep(1000 * latency); // sleep microseconds

    }

    void print() {
        std::cout << "Packet from " << sender_id_ << " to " << receiver_id_
        << "\nContent: " << content_ << ", size: " << size_ << "KB"
        << "\nDebug packet: " << debug_
        << "\nTime elapsed since creation: " << time(NULL) - time_sent_ << std::endl;
}

};


#endif
