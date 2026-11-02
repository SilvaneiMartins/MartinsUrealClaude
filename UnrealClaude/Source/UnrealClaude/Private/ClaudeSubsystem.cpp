// Copyright Silvanei Martins. Todos os direitos reservados.

#include "ClaudeSubsystem.h"
#include "ClaudeCodeRunner.h"
#include "ClaudeSessionManager.h"
#include "ProjectContext.h"
#include "ScriptExecutionManager.h"
#include "UnrealClaudeModule.h"
#include "UnrealClaudeConstants.h"

// Static cache avoids re-allocating this multi-KB prompt on every SendPrompt call.
// {{UE_VERSION}} is replaced with the engine version this plugin was compiled against.
static const FString CachedEngineSystemPrompt = FString(TEXT(R"(You are an expert Unreal Engine {{UE_VERSION}} developer assistant integrated directly into the UE Editor.

CONTEXT:
- You are helping with an Unreal Engine {{UE_VERSION}} project
- The user is working in the Unreal Editor and expects UE{{UE_VERSION}}-specific guidance
- Focus on current UE{{UE_VERSION}} APIs, patterns, and best practices

KEY UE{{UE_VERSION}} FEATURES TO BE AWARE OF:
- Enhanced Nanite and Lumen for next-gen rendering
- World Partition for open world streaming
- Mass Entity (experimental) for large-scale simulations
- Enhanced Input System (preferred over legacy input)
- Common UI for cross-platform interfaces
- Gameplay Ability System (GAS) for complex ability systems
- MetaSounds for procedural audio
- Chaos physics engine (default)
- Control Rig for animation
- Niagara for VFX

CODING STANDARDS:
- Use UPROPERTY, UFUNCTION, UCLASS macros properly
- Follow Unreal naming conventions (F for structs, U for UObject, A for Actor, E for enums)
- Prefer BlueprintCallable/BlueprintPure for BP-exposed functions
- Use TObjectPtr<> for object pointers in headers (UE5+)
- Use Forward declarations in headers, includes in cpp
- Properly use GENERATED_BODY() macro

WHEN PROVIDING CODE:
- Always specify the correct includes
- Use proper UE{{UE_VERSION}} API calls (not deprecated ones)
- Include both .h and .cpp when showing class implementations
- Explain any engine-specific gotchas or limitations

TOOL USAGE GUIDELINES:
- You have dedicated MCP tools for common Unreal Editor operations. ALWAYS prefer these over execute_script:
  * spawn_actor, move_actor, delete_actors, get_level_actors, set_property - Actor manipulation
  * open_level (open/new/list_templates) - Level management: open maps, create new levels, list templates
  * blueprint_query, blueprint_modify - Blueprint inspection and editing
  * anim_blueprint_modify - Animation blueprint state machines
  * asset_search, asset_dependencies, asset_referencers - Asset discovery and dependency tracking
  * capture_viewport - Screenshot the editor viewport
  * run_console_command - Run editor console commands
  * enhanced_input - Input action and mapping context management
  * character, character_data - Character and movement configuration
  * material - Material and material instance operations
  * task_submit, task_status, task_result, task_list, task_cancel - Async task management
- Only use execute_script when NO dedicated tool can accomplish the task
- Use open_level to switch levels instead of console commands (the 'open' command is blocked for security)
- Use get_ue_context to look up UE{{UE_VERSION}} API patterns before writing scripts

RESPONSE FORMAT:
- Be concise but thorough
- Provide code examples when helpful
- Mention relevant documentation or resources
- Warn about common pitfalls)")).Replace(TEXT("{{UE_VERSION}}"), *UnrealClaudeConstants::EngineVersionString());

FClaudeCodeSubsystem& FClaudeCodeSubsystem::Get()
{
	static FClaudeCodeSubsystem Instance;
	return Instance;
}

FClaudeCodeSubsystem::FClaudeCodeSubsystem()
{
	Runner = MakeUnique<FClaudeCodeRunner>();
	SessionManager = MakeUnique<FClaudeSessionManager>();
}

FClaudeCodeSubsystem::~FClaudeCodeSubsystem()
{
	// Defined out-of-line so TUniquePtr sees the full type of Runner/SessionManager for destruction
}

IClaudeRunner* FClaudeCodeSubsystem::GetRunner() const
{
	return Runner.Get();
}

void FClaudeCodeSubsystem::SendPrompt(
	const FString& Prompt,
	FOnClaudeResponse OnComplete,
	const FClaudePromptOptions& Options)
{
	FClaudeRequestConfig Config;

	Config.Prompt = BuildPromptWithHistory(Prompt);
	Config.WorkingDirectory = FPaths::ProjectDir();
	Config.bSkipPermissions = true;
	Config.AllowedTools = { TEXT("Read"), TEXT("Write"), TEXT("Edit"), TEXT("Grep"), TEXT("Glob"), TEXT("Bash") };

	Config.AttachedImagePaths = Options.AttachedImagePaths;

	// Prompt caching order: static blocks first (highest TTL hit rate), dynamic last.
	// 1. CachedEngineSystemPrompt — fully static, compiles to a constant string
	// 2. GetProjectContextPrompt — lightweight 1-line summary (stable between calls)
	// 3. CustomSystemPrompt (CLAUDE.md) — user-controlled, treat as stable
	// Conversation history and the user prompt are always appended last (dynamic, not cached).
	if (Options.bIncludeEngineContext)
	{
		Config.SystemPrompt = GetEngineSystemPrompt();
	}

	if (Options.bIncludeProjectContext)
	{
		Config.SystemPrompt += GetProjectContextPrompt();
	}

	if (!CustomSystemPrompt.IsEmpty())
	{
		Config.SystemPrompt += TEXT("\n\n") + CustomSystemPrompt;
	}

	Config.OnStreamEvent = Options.OnStreamEvent;

	// Wrap the user's completion callback so we can persist the exchange before forwarding it
	FOnClaudeResponse WrappedComplete;
	WrappedComplete.BindLambda([this, Prompt, OnComplete](const FString& Response, bool bSuccess)
	{
		if (bSuccess && SessionManager.IsValid())
		{
			SessionManager->AddExchange(Prompt, Response);
			SessionManager->SaveSession();
		}
		OnComplete.ExecuteIfBound(Response, bSuccess);
	});

	Runner->ExecuteAsync(Config, WrappedComplete, Options.OnProgress);
}

void FClaudeCodeSubsystem::SendPrompt(
	const FString& Prompt,
	FOnClaudeResponse OnComplete,
	bool bIncludeEngineContext,
	FOnClaudeProgress OnProgress,
	bool bIncludeProjectContext)
{
	FClaudePromptOptions Options;
	Options.bIncludeEngineContext = bIncludeEngineContext;
	Options.bIncludeProjectContext = bIncludeProjectContext;
	Options.OnProgress = OnProgress;
	SendPrompt(Prompt, OnComplete, Options);
}

FString FClaudeCodeSubsystem::GetEngineSystemPrompt() const
{
	return CachedEngineSystemPrompt;
}

FString FClaudeCodeSubsystem::GetProjectContextPrompt() const
{
	// Lightweight one-liner summary only — keeps this block stable for prompt caching.
	// Full class list, source tree, and level actors are available via unreal_get_project_context.
	const FString Summary = FProjectContextManager::Get().GetContextSummary();

	FString Context;
	if (!Summary.IsEmpty())
	{
		Context = TEXT("\n\n=== PROJECT CONTEXT ===\n");
		Context += Summary + TEXT("\n");
		Context += TEXT("Call unreal_get_project_context for the full class list, source tree, and level actors.\n");
		Context += TEXT("=== END PROJECT CONTEXT ===\n");
	}

	FString ScriptHistory = FScriptExecutionManager::Get().FormatHistoryForContext(10);
	if (!ScriptHistory.IsEmpty())
	{
		Context += TEXT("\n\n") + ScriptHistory;
	}

	return Context;
}

void FClaudeCodeSubsystem::SetCustomSystemPrompt(const FString& InCustomPrompt)
{
	CustomSystemPrompt = InCustomPrompt;
}

const TArray<TPair<FString, FString>>& FClaudeCodeSubsystem::GetHistory() const
{
	static TArray<TPair<FString, FString>> EmptyHistory;
	if (SessionManager.IsValid())
	{
		return SessionManager->GetHistory();
	}
	return EmptyHistory;
}

void FClaudeCodeSubsystem::ClearHistory()
{
	if (SessionManager.IsValid())
	{
		SessionManager->ClearHistory();
	}
}

void FClaudeCodeSubsystem::ResetSession()
{
	if (SessionManager.IsValid())
	{
		SessionManager->ClearHistory();
		SessionManager->DeleteSessionFile();
	}
}

void FClaudeCodeSubsystem::CancelCurrentRequest()
{
	if (Runner.IsValid())
	{
		Runner->Cancel();
	}
}

bool FClaudeCodeSubsystem::SaveSession()
{
	if (SessionManager.IsValid())
	{
		return SessionManager->SaveSession();
	}
	return false;
}

bool FClaudeCodeSubsystem::LoadSession()
{
	if (SessionManager.IsValid())
	{
		return SessionManager->LoadSession();
	}
	return false;
}

bool FClaudeCodeSubsystem::HasSavedSession() const
{
	if (SessionManager.IsValid())
	{
		return SessionManager->HasSavedSession();
	}
	return false;
}

FString FClaudeCodeSubsystem::GetSessionFilePath() const
{
	if (SessionManager.IsValid())
	{
		return SessionManager->GetSessionFilePath();
	}
	return FString();
}

FString FClaudeCodeSubsystem::BuildPromptWithHistory(const FString& NewPrompt) const
{
	if (!SessionManager.IsValid())
	{
		return NewPrompt;
	}

	const TArray<TPair<FString, FString>>& History = SessionManager->GetHistory();
	if (History.Num() == 0)
	{
		return NewPrompt;
	}

	FString PromptWithHistory;

	// Cap history window at MaxHistoryInPrompt to keep token budget bounded
	int32 StartIndex = FMath::Max(0, History.Num() - UnrealClaudeConstants::Session::MaxHistoryInPrompt);

	for (int32 i = StartIndex; i < History.Num(); ++i)
	{
		const TPair<FString, FString>& Exchange = History[i];
		PromptWithHistory += FString::Printf(TEXT("Human: %s\n\nAssistant: %s\n\n"), *Exchange.Key, *Exchange.Value);
	}

	PromptWithHistory += FString::Printf(TEXT("Human: %s"), *NewPrompt);

	return PromptWithHistory;
}
