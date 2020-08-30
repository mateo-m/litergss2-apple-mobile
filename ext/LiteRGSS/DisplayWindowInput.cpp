#include "LiteRGSS.h"
#define LITERGSS_INCLUDE_RUBY_THREAD
#include "RubyValue.h"
#include "Texture_Bitmap.h"
#include "DisplayWindowInput.h"

extern VALUE rb_eStoppedGraphics;
extern VALUE rb_eClosedWindow;

void DisplayWindowInput::manageErrorMessage(VALUE self, const GraphicsUpdateMessage& message) {
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
					m_insideGraphicsUpdate = false;
					return; /* If the proc returns false we doesn't show the exception */
				}
		}
		stop();
	}
	
	m_insideGraphicsUpdate = false;
	//rb_raise(message.errorObject, "%s", message.message.c_str());
}

void DisplayWindowInput::updateProcessEvent(GraphicsUpdateMessage& message) {
	sf::Event event;
	//L_EnteredText.clear();
	while(pollEvent(event))
	{
		switch(event.type)
		{
			case sf::Event::EventType::Closed:
				message.errorObject = rb_eClosedWindow;
				message.message = "Game Window has been closed by user.";
				return;
			case sf::Event::EventType::KeyPressed:
				/*L_Input_Update_Key(event.key.code, true);*/
				break;
			case sf::Event::EventType::KeyReleased:
				/*L_Input_Update_Key(event.key.code, false);*/
				break;
			case sf::Event::EventType::JoystickButtonPressed:
				/*L_Input_Update_Joy(event.joystickButton.joystickId, event.joystickButton.button, true);*/
				break;
			case sf::Event::EventType::JoystickButtonReleased:
				/*L_Input_Update_Joy(event.joystickButton.joystickId, event.joystickButton.button, false);*/
				break;
			case sf::Event::EventType::JoystickMoved:
				/*L_Input_Update_JoyPos(event.joystickMove.joystickId,
					event.joystickMove.axis,
					event.joystickMove.position);*/
				break;
			case sf::Event::EventType::JoystickConnected:
			case sf::Event::EventType::JoystickDisconnected:
				/*L_Input_Reset_JoyPos(event.joystickConnect.joystickId);*/
				break;
			case sf::Event::EventType::MouseMoved:
				/*L_Input_Mouse_Pos_Update(event.mouseMove.x, event.mouseMove.y);*/
				break;
			case sf::Event::EventType::MouseButtonPressed:
				/*L_Input_Mouse_Button_Update(event.mouseButton.button, true);
				L_Input_Mouse_Pos_Update(event.mouseButton.x, event.mouseButton.y);*/
				break;
			case sf::Event::EventType::MouseButtonReleased:
				/*L_Input_Mouse_Button_Update(event.mouseButton.button, false);
				L_Input_Mouse_Pos_Update(event.mouseButton.x, event.mouseButton.y);*/
				break;
			case sf::Event::EventType::MouseWheelScrolled:
				/*if(event.mouseWheelScroll.wheel == sf::Mouse::Wheel::VerticalWheel)
					L_Input_Mouse_Wheel_Update(static_cast<long>(event.mouseWheelScroll.delta));
				L_Input_Mouse_Pos_Update(event.mouseWheelScroll.x, event.mouseWheelScroll.y);*/
				break;
			case sf::Event::EventType::MouseLeft:
				/*L_Input_Mouse_Pos_Update(-256, -256);*/
				break;
			case sf::Event::EventType::TextEntered:
				/*L_EnteredText.append((char*)sf::String(event.text.unicode).toUtf8().c_str());*/
				break;
			default:
				break;
		}
	}
}

void* DisplayWindowInput_Update_Internal(void* dataPtr) {
	//NO RUBY API ACCESS MUST BE DONE HERE
	auto& self = *reinterpret_cast<DisplayWindowInput*>(dataPtr);
	if(self.isOpen()) {
		self.draw();
		return nullptr;
	}

	auto message = std::make_unique<GraphicsUpdateMessage>();
	message->errorObject = rb_eStoppedGraphics;
	message->message = "Game Window was closed during Graphics.update by an unknown cause...";
	return message.release();
}

std::unique_ptr<GraphicsUpdateMessage> DisplayWindowInput::realDraw() {
	auto* result = rb_thread_call_without_gvl(DisplayWindowInput_Update_Internal, static_cast<void*>(this), NULL, NULL);
	return std::unique_ptr<GraphicsUpdateMessage>(reinterpret_cast<GraphicsUpdateMessage*>(result));
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
	GraphicsUpdateMessage localMessage {};
	if (input) {
		updateProcessEvent(message == nullptr ? localMessage : *message);
	}
	localMessage = message == nullptr ? localMessage : *message;
	
	if (!localMessage.message.empty()) {
		manageErrorMessage(self, localMessage);
	}

	m_insideGraphicsUpdate = false;
	if (input) {
		m_frameCount++;
	}
}

void DisplayWindowInput::updateOnlyInput(VALUE self) {
	if (m_insideGraphicsUpdate) {
		return;
	}
	m_insideGraphicsUpdate = true;

	GraphicsUpdateMessage message;
	updateProcessEvent(message);
	if (!message.message.empty()) {
		manageErrorMessage(self, message);
	}

	m_insideGraphicsUpdate = false;
}

void DisplayWindowInput::transition(VALUE self, int argc, VALUE* argv) {
	//8 = from RGSS doc
	long time = 8;
	if (argc >= 1) {
		time = rb_num2long(argv[0]);
	}
	if (argc < 2 || rb_obj_is_kind_of(argv[1], rb_cBitmap) != Qtrue) {
		cgss::DisplayWindow::transition(time, nullptr);
	} else {
		auto& texture = rb::Get<TextureElement>(argv[1]);
		cgss::DisplayWindow::transition(time, texture.instance());
	}
}
