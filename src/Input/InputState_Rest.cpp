#include "InputState_Rest.hpp"
#include "InputState_Translate.hpp"

#include "Instance.hpp"
#include "Helper/DisplayInfos.hpp"

#include "Components/TransformMatrix.hpp"
#include <glm/gtx/matrix_transform_2d.hpp>

#include "GUI/FileBrowser.hpp"
#include "Debugging/Log.hpp"


InputState_Rest::InputState_Rest(InputSystem* inputSystem)
	: IInputState(inputSystem)
{}

void InputState_Rest::onLeftClicDown() {
	if (DisplayInfos::KeyIsDown(SDL_SCANCODE_SPACE))
		m_inputSystem->m_currentState = std::make_unique<InputState_Translate>(m_inputSystem, I.drawingBoardId());
	else {
		entt::entity hoveredLayer = I.layersManager().layerHoveredByMouse();
		if (I.registry().valid(hoveredLayer))
			m_inputSystem->m_currentState = std::make_unique<InputState_Translate>(m_inputSystem, hoveredLayer);
	}
}

void InputState_Rest::onWheelScroll(float dl) {
	glm::mat3& mat = I.registry().get<Cmp::TransformMatrix>(I.drawingBoardId()).val;
	mat = glm::scale(mat, glm::vec2(pow(0.95f, -dl)));
}

void InputState_Rest::onKeyDown(SDL_Scancode key) {
	if (DisplayInfos::KeyIsDown(SDL_SCANCODE_LCTRL)) {
		if (key == SDL_SCANCODE_E) {
			std::string filepath = FileBrowser::GetImageFileSave();
			spdlog::info("Saving file '{}'", filepath);
		}
		else if (key == SDL_SCANCODE_I) {
			std::string filepath = FileBrowser::GetImageFileOpen();
			spdlog::info("Importing file '{}'", filepath);
		}
	}
}