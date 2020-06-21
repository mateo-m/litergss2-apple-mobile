#include <cassert>
#include <LiteCGSS/Graphics/RenderStates.h>
#define LITERGSS_INCLUDE_RUBY_THREAD
#include "RubyValue.h"
#include "log.h"
#include "NormalizeNumbers.h"

#include "Input.h"

#include "Graphics.h"
#include "Texture_Bitmap.h"
#include "GraphicsSingleton.h"

GraphicsSingleton::GraphicsSingleton() : 
	m_gameWindow(m_eventDispatcher) {
}

void GraphicsSingleton::init() {
	if (!cgss::RenderStates::areShadersEnabled()) {
		rb_warn("Shaders are not available :(");
	} else if (!cgss::RenderStates::areGeometryShadersEnabled()) {
		rb_warn("Geometry shaders are not available :(");
	}

	/* Window Loading */
	auto config = m_configLoader.load();
	frame_count = 0;

	m_gameWindow.reload(std::move(config));

	//LOG("[GraphicsSingleton] Init");

	/* Input adjustement */
	L_Input_Reset_Clocks();
	L_Input_Setusec_threshold(1000000 / config.frameRate);
}

void GraphicsSingleton::manageErrorMessage(VALUE self, const GraphicsUpdateMessage& message) {
	/* If the error is ClosedWindowError, we manage the window closing 
	 * When @on_close is defined to a proc, @on_close can decide if the window closing is allowed or not
	 * or do things before closing the window
	 * That's a kind of rescue process 
	 */
	if(message.errorObject == rb_eClosedWindow)
	{
		VALUE closeHandle = rb_iv_get(self, "@on_close");
		if(closeHandle != Qnil)
		{
			VALUE handleClass = rb_class_of(closeHandle);
			if(handleClass == rb_cProc)
				if(rb_proc_call(closeHandle, rb_ary_new()) == Qfalse) {
					InsideGraphicsUpdate = false;
					return; /* If the proc returns false we doesn't show the exception */
				}
		}
		stop();
	}
	/* We raise the message */
	InsideGraphicsUpdate = false;
	rb_raise(message.errorObject, "%s", message.message.c_str());
}

void GraphicsSingleton::updateProcessEvent(GraphicsUpdateMessage& message) {
	sf::Event event;
	L_EnteredText.clear();
	while(m_gameWindow.pollEvent(event))
	{
		switch(event.type)
		{
			case sf::Event::EventType::Closed:
				message.errorObject = rb_eClosedWindow;
				message.message = "Game Window has been closed by user.";
				return;
			case sf::Event::EventType::KeyPressed:
				L_Input_Update_Key(event.key.code, true);
				break;
			case sf::Event::EventType::KeyReleased:
				L_Input_Update_Key(event.key.code, false);
				break;
			case sf::Event::EventType::JoystickButtonPressed:
				L_Input_Update_Joy(event.joystickButton.joystickId, event.joystickButton.button, true);
				break;
			case sf::Event::EventType::JoystickButtonReleased:
				L_Input_Update_Joy(event.joystickButton.joystickId, event.joystickButton.button, false);
				break;
			case sf::Event::EventType::JoystickMoved:
				L_Input_Update_JoyPos(event.joystickMove.joystickId,
					event.joystickMove.axis,
					event.joystickMove.position);
				break;
			case sf::Event::EventType::JoystickConnected:
			case sf::Event::EventType::JoystickDisconnected:
				L_Input_Reset_JoyPos(event.joystickConnect.joystickId);
				break;
			case sf::Event::EventType::MouseMoved:
				L_Input_Mouse_Pos_Update(event.mouseMove.x, event.mouseMove.y);
				break;
			case sf::Event::EventType::MouseButtonPressed:
				L_Input_Mouse_Button_Update(event.mouseButton.button, true);
				L_Input_Mouse_Pos_Update(event.mouseButton.x, event.mouseButton.y);
				break;
			case sf::Event::EventType::MouseButtonReleased:
				L_Input_Mouse_Button_Update(event.mouseButton.button, false);
				L_Input_Mouse_Pos_Update(event.mouseButton.x, event.mouseButton.y);
				break;
			case sf::Event::EventType::MouseWheelScrolled:
				if(event.mouseWheelScroll.wheel == sf::Mouse::Wheel::VerticalWheel)
					L_Input_Mouse_Wheel_Update(static_cast<long>(event.mouseWheelScroll.delta));
				L_Input_Mouse_Pos_Update(event.mouseWheelScroll.x, event.mouseWheelScroll.y);
				break;
			case sf::Event::EventType::MouseLeft:
				L_Input_Mouse_Pos_Update(-256, -256);
				break;
			case sf::Event::EventType::TextEntered:
				L_EnteredText.append((char*)sf::String(event.text.unicode).toUtf8().c_str());
				break;
			default:
				break;
		}
	}
}

bool GraphicsSingleton::isGameWindowOpen() const {
	return m_gameWindow.isOpen();
}

void GraphicsSingleton::stop() {
	m_gameWindow.stop();
}

void GraphicsSingleton::windowDraw() {
	m_gameWindow.draw();
}

void* GraphicsDraw_Update_Internal(void* dataPtr) {
	//NO RUBY API ACCESS MUST BE DONE HERE
	auto& self = *reinterpret_cast<GraphicsSingleton*>(dataPtr);
	if(self.isGameWindowOpen()) {
		self.windowDraw();
		return nullptr;
	}

	auto message = std::make_unique<GraphicsUpdateMessage>();
	message->errorObject = rb_eStoppedGraphics;
	message->message = "Game Window was closed during Graphics.update by a unknow cause...";
	return message.release();
}

std::unique_ptr<GraphicsUpdateMessage> GraphicsSingleton::draw() {
	auto* result = rb_thread_call_without_gvl(GraphicsDraw_Update_Internal, static_cast<void*>(this), NULL, NULL);
	return std::unique_ptr<GraphicsUpdateMessage>(reinterpret_cast<GraphicsUpdateMessage*>(result));
}

void GraphicsSingleton::update(VALUE self, bool input) {
	// Prevent a Thread from calling Graphics.update during Graphics.update process
	if(InsideGraphicsUpdate) { 
		return;
	}
	InsideGraphicsUpdate = true;

	/* Graphics.update real process */
	auto message = draw();
	
	/* Message Processing */
	GraphicsUpdateMessage localMessage{};
	if(input) {
		updateProcessEvent(message == nullptr ? localMessage : *message);
	}
	localMessage = message == nullptr ? localMessage : *message;
	
	if(!localMessage.message.empty()) {
		manageErrorMessage(self, localMessage);
	}

	/* End of Graphics.update process */
	InsideGraphicsUpdate = false;

	if(input) {
		/* Update the frame count */
		frame_count++;
	}
}

void GraphicsSingleton::updateOnlyInput(VALUE self) {
	if (InsideGraphicsUpdate) {
		return;
	}
	InsideGraphicsUpdate = true;

	GraphicsUpdateMessage message;
	updateProcessEvent(message);
	if (!message.message.empty()) {
		manageErrorMessage(self, message);
	}

	InsideGraphicsUpdate = false;
}

void GraphicsSingleton::resizeScreen(int width, int height) {
	m_gameWindow.resizeScreen(width, height);
}

void GraphicsSingleton::setShader(sf::RenderStates* shader) {
	m_gameWindow.setShader(shader);
}

void GraphicsSingleton::setIcon(const sf::Image& icon) {
	m_gameWindow.setIcon(icon);
}

void GraphicsSingleton::sortZ() {
	m_gameWindow.sortZ();
}

VALUE GraphicsSingleton::takeSnapshot() {
	return TextureElement::snapToTexture(m_gameWindow);
}

void GraphicsSingleton::transition(VALUE self, int argc, VALUE* argv) {
	//8 = from RGSS doc
	long time = 8;
	if (argc >= 1) {
		time = rb_num2long(argv[0]);
	}
	if (argc < 2 || rb_obj_is_kind_of(argv[1], rb_cBitmap) != Qtrue) {
		m_gameWindow.transition(time, nullptr);
	} else {
		auto& texture = rb::Get<TextureElement>(argv[1]);
		m_gameWindow.transition(time, texture.instance());
	}
}

void GraphicsSingleton::freeze(VALUE self) {
	m_gameWindow.freeze();
}
