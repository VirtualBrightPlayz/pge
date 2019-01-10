#include <Window/Window.h>
#include <IO/IO.h>

using namespace PGE;

IO::IO(Window* window) {
    keyboardSubscriber = SysEvents::Subscriber(window->getSdlWindow(),SysEvents::Subscriber::EventType::KEYBOARD);
    mouseSubscriber = SysEvents::Subscriber(window->getSdlWindow(),SysEvents::Subscriber::EventType::MOUSE);
    gamepadSubscriber = SysEvents::Subscriber(window->getSdlWindow(),SysEvents::Subscriber::EventType::GAMEPAD);

    SysEvents::subscribe(keyboardSubscriber);
    SysEvents::subscribe(mouseSubscriber);
    SysEvents::subscribe(gamepadSubscriber);

    inputs.clear();
}

IO::~IO() {
    SysEvents::unsubscribe(keyboardSubscriber);
    SysEvents::unsubscribe(mouseSubscriber);
    SysEvents::unsubscribe(gamepadSubscriber);
}

void IO::update() {
    SDL_Event event;
    while (keyboardSubscriber.popEvent(event)) {
        SDL_KeyboardEvent keyEvent = event.key;
        for (std::set<UserInput*>::iterator it=inputs.begin();it!=inputs.end();it++) {
            UserInput* input = (*it);
            if (input->getDevice()==UserInput::DEVICE::KEYBOARD) {
                KeyboardInput* keyboardInput = (KeyboardInput*)input;
                if (keyEvent.keysym.scancode==keyboardInput->getButton()) {
                    if (event.type == SDL_KEYDOWN) {
                        input->setDown(true);
                    } else if (event.type == SDL_KEYUP) {
                        input->setDown(false);
                    }
                }
            }
        }
    }

    while (mouseSubscriber.popEvent(event)) {
        if (event.type==SDL_MOUSEMOTION) {
            mousePos.x = event.motion.x;
            mousePos.y = event.motion.y;
        } else if (event.type==SDL_MOUSEBUTTONDOWN || event.type==SDL_MOUSEBUTTONUP) {
            SDL_MouseButtonEvent mouseButtonEvent = event.button;
            for (std::set<UserInput*>::iterator it=inputs.begin();it!=inputs.end();it++) {
                UserInput* input = (*it);
                if (input->getDevice()==UserInput::DEVICE::MOUSE) {
                    MouseInput* mouseInput = (MouseInput*)input;
                    MouseInput::BUTTON button;
                    switch (mouseButtonEvent.button) {
                        case SDL_BUTTON_LEFT: {
                            button = MouseInput::BUTTON::LEFT;
                        } break;
                        case SDL_BUTTON_RIGHT: {
                            button = MouseInput::BUTTON::RIGHT;
                        } break;
                        case SDL_BUTTON_MIDDLE: {
                            button = MouseInput::BUTTON::MIDDLE;
                        } break;
                        case SDL_BUTTON_X1: {
                            button = MouseInput::BUTTON::SIDE1;
                        } break;
                        case SDL_BUTTON_X2: {
                            button = MouseInput::BUTTON::SIDE2;
                        } break;
                    }
                    if (button==mouseInput->getButton()) {
                        if (event.type==SDL_MOUSEBUTTONDOWN) {
                            input->setDown(true);
                        } else if (event.type==SDL_MOUSEBUTTONUP) {
                            input->setDown(false);
                        }
                    }
                }
            }
        }
    }
}

Vector2i IO::getMousePosition() const {
    return mousePos;
}

void IO::trackInput(UserInput* input) {
    inputs.emplace(input);
}

void IO::untrackInput(UserInput* input) {
    std::set<UserInput*>::iterator it = inputs.find(input);
    if (it != inputs.end()) {
        inputs.erase(it);
    }
}
