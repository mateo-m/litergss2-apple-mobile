#ifndef DisplayWindowUpdateMessage_H
#define DisplayWindowUpdateMessage_H
#include <string>
struct DisplayWindowUpdateMessage {
	VALUE errorObject = Qnil;
	std::string message;
};
#endif
