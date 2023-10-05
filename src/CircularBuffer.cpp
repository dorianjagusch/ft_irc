#include "../inc/CircularBuffer.hpp"

CircularBuffer::CircularBuffer() : p_head(0), p_tail(0) {
	p_buffer = new unsigned char[MAXDATASIZE * 2];
	memset(p_buffer, '\0', MAXDATASIZE * 2);
}

CircularBuffer::CircularBuffer( CircularBuffer const & src ) {
	p_buffer = NULL;
	*this = src;
}

CircularBuffer::~CircularBuffer() {
	delete[] p_buffer;
}

CircularBuffer&	CircularBuffer::operator=( CircularBuffer const & rhs ) {
	if( this != &rhs ) {
		p_head = rhs.p_head;
		p_tail = rhs.p_tail;
		if (p_buffer)
			delete [] p_buffer;
		p_buffer = new unsigned char[2 * MAXDATASIZE];
		memset(p_buffer, 0, MAXDATASIZE * 2);
		for (int i = 0; rhs.p_buffer[i]; i++) {
			p_buffer[i] = rhs.p_buffer[i];
		}
	}
	return (*this);
}

int CircularBuffer::emptyCheck() {
	//empty or full check for null heh rhymes!
	if (p_tail == p_head)
		return 0;
	return 1;
}

void CircularBuffer::addToBuffer(const char* buf, ssize_t numbytes) {
	for (ssize_t i = 0; i < numbytes; i++) {
		p_buffer[p_head] = static_cast<unsigned char> (buf[i]);
		p_head++;
		if (p_head == MAXDATASIZE * 2)
			p_head = 0;
	}
}

void CircularBuffer::addToBuffer(const char* buf) {
	
	size_t len = strlen(buf);
	for (size_t i = 0; i <  len; i++) {
		p_buffer[p_head] = static_cast<unsigned char> (buf[i]);
		p_head++;
		if (p_head == MAXDATASIZE * 2)
			p_head = 0;
	}
}

int CircularBuffer::findCRLF() const {
	for (int i = p_tail; p_buffer[i + 1]; i++) {
		if (p_buffer[i] == '\r' || p_buffer[i + 1] == '\n' || (p_buffer[i] == '\r' && p_buffer[i + 1] == '\n'))
			return (i);
	}
	return (-1);
}

std::string CircularBuffer::extractBuffer() {
	int templen = 0;
	int tail_backup = p_tail;
	while (p_tail != p_head) { //calculating len of the buffered msg
		p_tail++;
		if (p_tail == MAXDATASIZE * 2)
			p_tail = 0;
		templen++;
	}

	unsigned char* tempbuffer = new unsigned char[templen];
	for (int i = 0; tail_backup != p_head; i++) { //copy p_buffer to temp & zero p_buffer
		tempbuffer[i] = p_buffer[tail_backup];
		p_buffer[tail_backup] = '\0';
		tail_backup++;
		if (tail_backup == MAXDATASIZE * 2)
			tail_backup = 0;
	}

	std::string bufferString(reinterpret_cast<char*>(tempbuffer)); //cast and save as string
	delete[] tempbuffer; //free temp

	size_t index = 0;
    while ((index = bufferString.find("^D", index)) != std::string::npos) // find "^D"
        bufferString.replace(index, 2, ""); //replace "^D" with ""

	return (bufferString);
}

void CircularBuffer::clear(){
	memset(this, 0, MAXDATASIZE);
	p_head = 0;
	p_tail = 0;
}

void CircularBuffer::printbuf() {
	std::cout << "Tail is at: " << p_tail << ", Head is at: " << p_head << std::endl;
	std::cout << "In buffer: ";
	for (int i = p_tail; i != p_head; i++) {
		if (i == MAXDATASIZE * 2) {
			i = 0;
		}
		std::cout << p_buffer[i];
	}
	std::cout << std::endl;
	return ;
}
