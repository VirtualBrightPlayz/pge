#include <UserInput/UserInput.h>

using namespace PGE;

UserInput::UserInput() {
    inputDown = false;
}

bool UserInput::isDown() const {
    return inputDown;
}

bool UserInput::isHit() const {
    return inputHit;
}

void UserInput::setDown(bool down) {
    inputDown = down;
}

void UserInput::setHit(bool hit) {
    inputHit = hit;
}

KeyboardInput::KeyboardInput(KeyboardInput::SCANCODE inKeyCode) {
    keyCode = inKeyCode;
}

UserInput::DEVICE KeyboardInput::getDevice() const {
    return UserInput::DEVICE::KEYBOARD;
}

KeyboardInput::SCANCODE KeyboardInput::getButton() const {
    return keyCode;
}

MouseInput::MouseInput(MouseInput::BUTTON inMouseButton) {
    mouseButton = inMouseButton;
}

UserInput::DEVICE MouseInput::getDevice() const {
    return UserInput::DEVICE::MOUSE;
}

MouseInput::BUTTON MouseInput::getButton() const {
    return mouseButton;
}

void MouseInput::setDoubleClick(bool bruh) {
    doubleClick = bruh;
}

bool MouseInput::doubleClicked() const {
    return doubleClick;
}

void MouseInput::setTripleClick(bool bruh) {
    tripleClick = bruh;
}

bool MouseInput::tripleClicked() const {
    return tripleClick;
}

ControllerInput::ControllerInput(ControllerInput::BUTTON inControllerButton) {
    controllerButton = inControllerButton;
}

UserInput::DEVICE ControllerInput::getDevice() const {
    return UserInput::DEVICE::CONTROLLER;
}

ControllerInput::BUTTON ControllerInput::getButton() const {
    return controllerButton;
}
