#include <system_error>
#include <unistd.h>
#include <fmt/format.h>

#include "UDPTransceiver.hpp"

static struct sockaddr_in buildSockAddr(const char* ip, uint16_t port)
{
	struct sockaddr_in result;
	result.sin_family = AF_INET;
	result.sin_port = htons(port);
	if (inet_pton(AF_INET, ip, &result.sin_addr) != 1) {
		throw std::system_error(errno, std::system_category(), "inet_pton");
	}
	return result;
}

static bool sockAddrEqual(struct sockaddr_in* lhs, struct sockaddr_in* rhs) {
	return lhs->sin_addr.s_addr == rhs->sin_addr.s_addr &&
	       lhs->sin_port == rhs->sin_port;
}

UDPTransceiver::UDPTransceiver(uint16_t port)
{
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		throw std::system_error(errno, std::system_category(), "socket");
	}

	input = buildSockAddr("0.0.0.0", port);
	if (bind(fd, reinterpret_cast<const struct sockaddr*>(&input), sizeof(input)) < 0) {
		throw std::system_error(errno, std::system_category(), "bind");
	}
}

std::optional<MemSlice> UDPTransceiver::getNextMsg()
{
	struct sockaddr_in senderAddr;
	auto sizeOf = static_cast<socklen_t>(sizeof(senderAddr));
	ssize_t rd = recvfrom(fd, buffer, BUFFER_LENGTH, MSG_DONTWAIT, 
	                      reinterpret_cast<struct sockaddr*>(&senderAddr), &sizeOf);
	if (rd < 0) {
		if (errno != EWOULDBLOCK && errno != EAGAIN) {
			fmt::print(stderr, "recvfrom: {}\n", strerror(errno));
		}
		return {};
	}
	if (!output.has_value() || !sockAddrEqual(&output.value(), &senderAddr)) {
		output = senderAddr;
	}
	return MemSlice(buffer, rd);
}

bool UDPTransceiver::sendMsg(MemSlice msg)
{
	if (!output.has_value()) {
		return false;
	}
	ssize_t sent = sendto(fd, msg.getPtr(), msg.getSize(), 0, 
	                      reinterpret_cast<const struct sockaddr*>(&output.value()), sizeof(output.value()));
	if (sent < 0) {
		throw std::system_error(errno, std::system_category(), "sendto");
	}
	return true;
}

void UDPTransceiver::setTarget(const char* ip, uint16_t port)
{
	output = buildSockAddr(ip, port);
}

UDPTransceiver::~UDPTransceiver()
{
	int status = close(fd);
	if (status < 0) {
		fmt::print(stderr, "close: {}\n", strerror(errno));
	}
}