// Copyright Silvanei Martins. Todos os direitos reservados.

#pragma once

#include "Misc/AutomationTest.h"

namespace UnrealClaudeTest
{
	/** Shared test flags used across UnrealClaude automation tests. */
	constexpr EAutomationTestFlags DefaultTestFlags =
		EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter;
}
