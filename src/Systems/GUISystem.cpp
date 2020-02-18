#include "GUISystem.hpp"

#include "Instance.hpp"

#include <imgui/imgui.h>

#include "Components/ParametersList.hpp"
#include "Components/GUI/SliderFloat.hpp"
#include "Components/GUI/ColorPicker3.hpp"

bool GUISystem::s_bShowGUI = true;

GUISystem::GUISystem(Instance& instance)
	: ISystem(instance)
{}

void GUISystem::render() {
	if (s_bShowGUI) {
		entt::registry& R = I.registry();
		entt::entity selLayer = I.layersManager().selectedLayer();
		if (I.registry().valid(selLayer)) {
			ImGui::Begin("Parameters");
			for (entt::entity guiElement : R.get<Cmp::Parameters>(selLayer).list) {
				// Slider float
				Cmp::SliderFloat* sf = R.try_get<Cmp::SliderFloat>(guiElement);
				if (sf)
					if (ImGui::SliderFloat(sf->name.c_str(), &sf->val, sf->minVal, sf->maxVal, sf->format.c_str(), sf->power))
						R.assign<entt::tag<"MustRecomputeTexture"_hs>>(selLayer);
					else {
						// ColorPicker3
						Cmp::ColorPicker3* cp3 = R.try_get<Cmp::ColorPicker3>(guiElement);
						if (cp3)
							ImGui::ColorPicker3(cp3->name.c_str(), (float*)&cp3->val, cp3->flags);
					}
			}
			ImGui::End();
		}
	}
}