#pragma once

#include <cstddef>
#include <optional>
#include <arpa/inet.h>

struct MemSlice
{
	MemSlice(void* ptr, size_t size) : ptr(ptr), size(size) {}
	MemSlice(std::string str) : ptr(str.data()), size(str.size()) {}
	void* getPtr() { return ptr; }
	size_t getSize() { return size; }
private:
	void* ptr;
	size_t size;
};

struct UDPTransceiver
{
	UDPTransceiver(uint16_t port);
	~UDPTransceiver();

	void setTarget(const char* ip, uint16_t port);
	std::optional<MemSlice> getNextMsg();
	bool sendMsg(MemSlice message);

private:
	int fd;
	struct sockaddr_in input;
	std::optional<struct sockaddr_in> output;
	static const size_t BUFFER_LENGTH = 32;
	std::byte buffer[BUFFER_LENGTH];
};