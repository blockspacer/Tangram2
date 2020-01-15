#include "InputState_Rest.hpp"

#include "InputSystem.hpp"

#include "InputState_DBTranslation.hpp"

#include "Debugging/Log.hpp"

#include "Components/TransformMatrix.hpp"
#include <glm/gtx/matrix_transform_2d.hpp>

#include "App.hpp"

InputState_Rest::InputState_Rest(InputSystem* inputSystem)
	: IInputState(inputSystem)
{}

void InputState_Rest::onLeftClicDown() {
	if (InputSystem::KeyIsDown(SDL_SCANCODE_SPACE))
		m_inputSystem->setState<InputState_DBTranslation>();
}

void InputState_Rest::onWheelScroll(float dl) {
	glm::mat3& mat = m_inputSystem->m_registry.get<Cmp::TransformMatrix>(App::Get().m_drawingBoardId).val;
	mat = glm::scale(mat, glm::vec2(pow(0.95f, -dl)));
}