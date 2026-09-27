#include "LiteRGSS.h"
#define LITERGSS_INCLUDE_RUBY_THREAD
#include "RubyValue.h"
#include "Texture_Bitmap.h"
#include "DisplayWindowInput.h"
#include "rbAdapter.h"
#include "DisplayWindow.h"
#include <SFML/Window/VideoMode.hpp>
#include <atomic>

extern VALUE rb_eStoppedWindowError;
extern VALUE rb_eClosedWindow;

// apple-mobile: the SFML fork draws the picture in the part of the window
// that a host app gives it with sfml_set_output_region.
extern "C" void sfml_get_output_region(float* x, float* y, float* width, float* height);
extern "C" void sfml_window_pixel_size(unsigned int* width, unsigned int* height);

namespace {
	// apple-mobile: a host app lets the player turn touch input off.
	std::atomic<bool> touchEnabled { true };

	// apple-mobile: a game that sets no touch handler reads only the mouse.
	// The first finger then moves the mouse and holds its left button.
	long mouseFinger = -1;
	// PSDK scenes skip a click in a frame where the mouse also moved (see
	// Scene_Title#update_mouse). So the press waits one pass over the event
	// queue after the move, and the release waits one pass after the press.
	// The game reads Mouse between two passes.
	bool mousePressWaits = false;
	bool mouseReleaseWaits = false;

	bool touchReachesTheGame() {
		return touchEnabled.load(std::memory_order_relaxed);
	}

	// PSDK's Mouse module divides the coordinate an event carries. It
	// divides by the window scale, or by the desktop size when the game
	// runs full screen. Graphics gives DisplayWindow.new the same scale
	// and the same full screen flag, so both numbers are here, and the
	// point inside the picture goes out multiplied by the same number.
	// Then Mouse lands on the game pixel under the finger.
	sf::Vector2i touchInsideThePicture(const cgss::DisplayWindowSettings& settings, int x, int y) {
		unsigned int windowWidth = 0;
		unsigned int windowHeight = 0;
		sfml_window_pixel_size(&windowWidth, &windowHeight);
		float regionX = 0;
		float regionY = 0;
		float regionWidth = 0;
		float regionHeight = 0;
		sfml_get_output_region(&regionX, &regionY, &regionWidth, &regionHeight);
		const double rectWidth = regionWidth * windowWidth;
		const double rectHeight = regionHeight * windowHeight;
		if (rectWidth <= 0 || rectHeight <= 0) {
			return { x, y };
		}
		const double partX = (x - regionX * windowWidth) / rectWidth;
		const double partY = (y - regionY * windowHeight) / rectHeight;
		if (settings.fullscreen) {
			const auto desktop = sf::VideoMode::getDesktopMode();
			return {
				static_cast<int>(partX * desktop.width),
				static_cast<int>(partY * desktop.height)
			};
		}

		return {
			static_cast<int>(partX * settings.video.width * settings.video.scale),
			static_cast<int>(partY * settings.video.height * settings.video.scale)
		};
	}
}

extern "C" void litergss_set_touch_enabled(int enabled) {
	touchEnabled.store(enabled != 0, std::memory_order_relaxed);
}

void DisplayWindowInput::manageErrorMessage(VALUE self, const DisplayWindowUpdateMessage& message) {
	/* Force window closing if the error is ClosedWindowError */
	if (message.errorObject == rb_eClosedWindow) {
		stop();
	}

	m_insideGraphicsUpdate = false;
	rb_raise(message.errorObject, "%s", message.message.c_str());
}

void DisplayWindowInput::updateProcessEvent(VALUE self, DisplayWindowUpdateMessage& message) {
	sf::Event event;
	auto& window = rb::Get<DisplayWindowElement>(self);
	ID rbCall = rb_intern("call");

	auto moveMouse = [&](sf::Vector2i point) {
		if (window.rOnMouseMoved != Qnil) {
			VALUE args[2] = { INT2NUM(point.x), INT2NUM(point.y) };
			rb_funcall2(window.rOnMouseMoved, rbCall, 2, args);
		}
	};
	auto sendLeftButton = [&](VALUE handler) {
		if (handler != Qnil) {
			VALUE arg = ULONG2NUM(sf::Mouse::Left);
			rb_funcall2(handler, rbCall, 1, &arg);
		}
	};
	bool mousePressedThisPass = false;
	if (mousePressWaits) {
		mousePressWaits = false;
		sendLeftButton(window.rOnMouseButtonPressed);
		mousePressedThisPass = true;
	} else if (mouseReleaseWaits) {
		mouseReleaseWaits = false;
		sendLeftButton(window.rOnMouseButtonRelease);
	}

	while (popEvent(event))
	{
		switch(event.type)
		{
			case sf::Event::EventType::Closed:
				if (NIL_P(window.rOnClosed) || rb_funcall(window.rOnClosed, rbCall, 0) != Qfalse) {
					message.errorObject = rb_eClosedWindow;
					message.message = "Game Window has been closed by user";
					return;
				}
			case sf::Event::EventType::Resized:
				if (window.rOnResized != Qnil) {
					VALUE args[2] = {
						ULONG2NUM(event.size.width),
						ULONG2NUM(event.size.height)
					};
					rb_funcall2(window.rOnResized, rbCall, 2, args);
				}
				break;
			case sf::Event::EventType::LostFocus:
				if (window.rOnLostFocus != Qnil) {
					rb_funcall(window.rOnLostFocus, rbCall, 0);
				}
				window->setActive(false);
				break;
			case sf::Event::EventType::GainedFocus:
				window->setActive(true);
				if (window.rOnGainedFocus != Qnil) {
					rb_funcall(window.rOnGainedFocus, rbCall, 0);
				}
				break;
			case sf::Event::TextEntered:
				if (window.rOnTextEntered != Qnil) {
					VALUE str = rb_utf8_str_new_cstr((char*)sf::String(event.text.unicode).toUtf8().c_str());
					rb_funcall2(window.rOnTextEntered, rbCall, 1, &str);
				}
				break;
			case sf::Event::EventType::KeyPressed:
				if (window.rOnKeyPressed != Qnil) {
					VALUE args[6] = {
						ULONG2NUM(event.key.code),
						ULONG2NUM(event.key.scancode),
						event.key.alt ? Qtrue : Qfalse,
						event.key.control ? Qtrue : Qfalse,
						event.key.shift ? Qtrue : Qfalse,
						event.key.system ? Qtrue : Qfalse
					};
					// A game built before the scancode joined this event
					// registers `proc { |code, alt| ... }`. The scancode now
					// sits where alt sat, and a scancode is never nil, so
					// every key reads as "alt is down". PSDK answers
					// Alt+Enter with Graphics.swap_fullscreen, so the confirm
					// key toggles the video mode and sets no key. The proc's
					// arity is the one signal of which shape it carries: the
					// old proc asks for 2 values, the current one for 3.
					if (NUM2INT(rb_funcall(window.rOnKeyPressed, rb_intern("arity"), 0)) == 2) {
						VALUE legacy[5] = { args[0], args[2], args[3], args[4], args[5] };
						rb_funcall2(window.rOnKeyPressed, rbCall, 5, legacy);
					} else {
						rb_funcall2(window.rOnKeyPressed, rbCall, 6, args);
					}
				}
				break;
			case sf::Event::EventType::KeyReleased:
				if (window.rOnKeyReleased != Qnil) {
					VALUE args[2] = {
						ULONG2NUM(event.key.code),
						ULONG2NUM(event.key.scancode)
					};
					rb_funcall2(window.rOnKeyReleased, rbCall, 2, args);
				}
				break;
			case sf::Event::EventType::MouseWheelScrolled:
				if (window.rOnMouseWheelScrolled != Qnil) {
					VALUE args[2] = {
						ULONG2NUM(event.mouseWheelScroll.wheel),
						DBL2NUM(static_cast<double>(event.mouseWheelScroll.delta))
					};
					rb_funcall2(window.rOnMouseWheelScrolled, rbCall, 2, args);
				}
				break;
			case sf::Event::EventType::MouseButtonPressed:
				if (window.rOnMouseButtonPressed != Qnil) {
					VALUE arg = ULONG2NUM(event.mouseButton.button);
					rb_funcall2(window.rOnMouseButtonPressed, rbCall, 1, &arg);
				}
				break;
			case sf::Event::EventType::MouseButtonReleased:
				if (window.rOnMouseButtonRelease != Qnil) {
					VALUE arg = ULONG2NUM(event.mouseButton.button);
					rb_funcall2(window.rOnMouseButtonRelease, rbCall, 1, &arg);
				}
				break;
			case sf::Event::EventType::MouseMoved:
				if (window.rOnMouseMoved != Qnil) {
					VALUE args[2] = {
						INT2NUM(event.mouseMove.x),
						INT2NUM(event.mouseMove.y)
					};
					rb_funcall2(window.rOnMouseMoved, rbCall, 2, args);
				}
				break;
			case sf::Event::EventType::MouseEntered:
				if (window.rOnMouseEntered != Qnil) {
					rb_funcall(window.rOnMouseEntered, rbCall, 0);
				}
				break;
			case sf::Event::EventType::MouseLeft:
				if (window.rOnMouseLeft != Qnil) {
					rb_funcall(window.rOnMouseLeft, rbCall, 0);
				}
				break;
			case sf::Event::EventType::JoystickButtonPressed:
				if (window.rOnJoystickButtonPressed != Qnil) {
					VALUE args[2] = {
						UINT2NUM(event.joystickButton.joystickId),
						UINT2NUM(event.joystickButton.button)
					};
					rb_funcall2(window.rOnJoystickButtonPressed, rbCall, 2, args);
				}
				break;
			case sf::Event::EventType::JoystickButtonReleased:
				if (window.rOnJoystickButtonReleased != Qnil) {
					VALUE args[2] = {
						UINT2NUM(event.joystickButton.joystickId),
						UINT2NUM(event.joystickButton.button)
					};
					rb_funcall2(window.rOnJoystickButtonReleased, rbCall, 2, args);
				}
				break;
			case sf::Event::EventType::JoystickMoved:
				if (window.rOnJoystickMoved != Qnil) {
					VALUE args[3] = {
						UINT2NUM(event.joystickMove.joystickId),
						LONG2NUM(event.joystickMove.axis),
						DBL2NUM(static_cast<double>(event.joystickMove.position))
					};
					rb_funcall2(window.rOnJoystickMoved, rbCall, 3, args);
				}
				break;
			case sf::Event::EventType::JoystickConnected:
				if (window.rOnJoystickConnected != Qnil) {
					VALUE arg = UINT2NUM(event.joystickConnect.joystickId);
					rb_funcall2(window.rOnJoystickConnected, rbCall, 1, &arg);
				}
				break;
			case sf::Event::EventType::JoystickDisconnected:
				if (window.rOnJoystickDisconnected != Qnil) {
					VALUE arg = UINT2NUM(event.joystickConnect.joystickId);
					rb_funcall2(window.rOnJoystickDisconnected, rbCall, 1, &arg);
				}
				break;
			case sf::Event::EventType::TouchBegan:
				if (window.rOnTouchBegan != Qnil && touchReachesTheGame()) {
					const auto point = touchInsideThePicture(window->getSettings(), event.touch.x, event.touch.y);
					VALUE args[3] = {
						UINT2NUM(event.touch.finger),
						INT2NUM(point.x),
						INT2NUM(point.y)
					};
					rb_funcall2(window.rOnTouchBegan, rbCall, 3, args);
				} else if (window.rOnTouchBegan == Qnil && touchReachesTheGame() && mouseFinger < 0) {
					mouseFinger = event.touch.finger;
					moveMouse(touchInsideThePicture(window->getSettings(), event.touch.x, event.touch.y));
					mousePressWaits = true;
				}
				break;
			case sf::Event::EventType::TouchMoved:
				if (window.rOnTouchMoved != Qnil && touchReachesTheGame()) {
					const auto point = touchInsideThePicture(window->getSettings(), event.touch.x, event.touch.y);
					VALUE args[3] = {
						UINT2NUM(event.touch.finger),
						INT2NUM(point.x),
						INT2NUM(point.y)
					};
					rb_funcall2(window.rOnTouchMoved, rbCall, 3, args);
				} else if (static_cast<long>(event.touch.finger) == mouseFinger) {
					moveMouse(touchInsideThePicture(window->getSettings(), event.touch.x, event.touch.y));
				}
				break;
			case sf::Event::EventType::TouchEnded:
				if (window.rOnTouchEnded != Qnil && touchReachesTheGame()) {
					const auto point = touchInsideThePicture(window->getSettings(), event.touch.x, event.touch.y);
					VALUE args[3] = {
						UINT2NUM(event.touch.finger),
						INT2NUM(point.x),
						INT2NUM(point.y)
					};
					rb_funcall2(window.rOnTouchEnded, rbCall, 3, args);
				} else if (static_cast<long>(event.touch.finger) == mouseFinger) {
					mouseFinger = -1;
					if (mousePressWaits || mousePressedThisPass) {
						mouseReleaseWaits = true;
					} else {
						sendLeftButton(window.rOnMouseButtonRelease);
					}
				}
				break;
			case sf::Event::SensorChanged:
				if (window.rOnSensorChanged != Qnil) {
					VALUE args[4] = {
						UINT2NUM(event.sensor.type),
						DBL2NUM(static_cast<double>(event.sensor.x)),
						DBL2NUM(static_cast<double>(event.sensor.y)),
						DBL2NUM(static_cast<double>(event.sensor.z))
					};
					rb_funcall2(window.rOnSensorChanged, rbCall, 4, args);
				}
				break;
			default:
				break;
		}
	}
}

void* DisplayWindowInput_Update_Internal(void* dataPtr) {
	//NO RUBY API ACCESS MUST BE DONE HERE
	auto& self = *reinterpret_cast<DisplayWindowInput*>(dataPtr);
	if (self.isOpen()) {
		self.draw();
		return nullptr;
	}

	auto message = std::make_unique<DisplayWindowUpdateMessage>();
	message->errorObject = rb_eStoppedWindowError;
	message->message = "Game Window was closed during DisplayWindow.update by an unknown cause...";
	return message.release();
}

std::unique_ptr<DisplayWindowUpdateMessage> DisplayWindowInput::realDraw() {
	auto* result = rb_thread_call_without_gvl(DisplayWindowInput_Update_Internal, static_cast<void*>(this), NULL, NULL);
	return std::unique_ptr<DisplayWindowUpdateMessage>(reinterpret_cast<DisplayWindowUpdateMessage*>(result));
}

void DisplayWindowInput::update(VALUE self, bool input) {
	// Prevent a Thread from calling update during an already running update process
	if (m_insideGraphicsUpdate) {
		return;
	}
	m_insideGraphicsUpdate = true;

	/* Graphics.update real process */
	auto message = realDraw();

	/* Message Processing */
	DisplayWindowUpdateMessage localMessage {};
	if (input) {
		updateProcessEvent(self, message == nullptr ? localMessage : *message);
	}
	localMessage = message == nullptr ? localMessage : *message;

	if (!localMessage.message.empty()) {
		manageErrorMessage(self, localMessage);
	}

	m_insideGraphicsUpdate = false;
}

void DisplayWindowInput::updateOnlyInput(VALUE self) {
	if (m_insideGraphicsUpdate) {
		return;
	}
	m_insideGraphicsUpdate = true;

	DisplayWindowUpdateMessage message;
	updateProcessEvent(self, message);
	if (!message.message.empty()) {
		manageErrorMessage(self, message);
	}

	m_insideGraphicsUpdate = false;
}
