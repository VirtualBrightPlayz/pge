#include <PGE/Input/Input.h>

using namespace PGE;

bool Input::isDown() const {
    return inputDown;
}

bool Input::isHit() const {
    return inputHit;
}

bool Input::isReleased() const {
    return inputReleased;
}

void Input::setDown(bool down) {
    inputDown = down;
}

void Input::setHit(bool hit) {
    inputHit = hit;
}

void Input::setReleased(bool released) {
    inputReleased = released;
}

Input::Device KeyboardInput::getDevice() const {
    return Input::Device::KEYBOARD;
}

int KeyboardInput::getKey() const {
    return (int)keyCode;
}

KeyboardInput::Keycode KeyboardInput::getButton() const {
    return keyCode;
}

Input::Device MouseInput::getDevice() const {
    return Input::Device::MOUSE;
}

int MouseInput::getKey() const {
    return (int)mouseButton;
}

MouseInput::Button MouseInput::getButton() const {
    return mouseButton;
}

void MouseInput::setClickCount(int count) {
    clicks = count;
}

int MouseInput::getClickCount() const {
    return clicks;
}

ControllerInput::ControllerInput(Controller* ctrlr, ControllerInput::Button inControllerButton, float threshold)
    : controllerButton(inControllerButton) {
    controller = ctrlr;
    stickPosition = Vectors::ZERO2F;
    pressDepth = 0.f;
    downThreshold = threshold;
}

Input::Device ControllerInput::getDevice() const {
    return Input::Device::CONTROLLER;
}

int ControllerInput::getKey() const {
    return (int)controllerButton;
}

ControllerInput::Button ControllerInput::getButton() const {
    return controllerButton;
}

const Vector2f& ControllerInput::getStickPosition() const {
    return stickPosition;
}

float ControllerInput::getPressDepth() const {
    return pressDepth;
}

float ControllerInput::getDownThreshold() const {
    return downThreshold;
}

const Controller* ControllerInput::getController() const {
    return controller;
}

void ControllerInput::removeController() {
    controller = nullptr;
    stickPosition = Vectors::ZERO2F;
    inputHit = false; inputDown = false;
    pressDepth = 0.f;
}

void ControllerInput::setStickPosition(const Vector2f& pos) {
    stickPosition = pos;
}

void ControllerInput::setPressDepth(float depth) {
    pressDepth = depth;
    inputDown = pressDepth > downThreshold;
}

void ControllerInput::setDownThreshold(float threshold) {
    downThreshold = threshold;
    inputDown = pressDepth > downThreshold;
}
