/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#pragma once

#include "CMapState.h"

#include "config/GameConfigSection.h"

#include "utils/json_utils.h"
#include "utils/ModelReplacement.h"

/**
*	@brief Allows a configuration file to specify a global model replacement file.
*/
class GlobalModelReplacementSection final : public GameConfigSection
{
public:
	explicit GlobalModelReplacementSection() = default;

	std::string_view GetName() const override final { return "GlobalModelReplacement"; }

	std::tuple<std::string, std::string> GetSchema() const override final
	{
		return {
			fmt::format(R"(
"FileName": {{
	"type": "string"
}}
)"),
			{"\"FileName\""}};
	}

	bool TryParse(GameConfigContext& context) const override final
	{
		const auto fileName = context.Input.value("FileName", std::string{});

		if (!fileName.empty())
		{
			auto map = g_ModelReplacement.LoadReplacementMap(fileName);

			if (!map.empty())
			{
				auto mapState = std::any_cast<CMapState*>(context.UserData);

				//Append replacement mappings from other to my map.
				//To ensure that existing keys are overwritten this is done by appending to the new map and then moving it to ours.
				map.insert(mapState->m_GlobalModelReplacement.begin(), mapState->m_GlobalModelReplacement.end());

				mapState->m_GlobalModelReplacement = std::move(map);
			}
		}

		return true;
	}
};
