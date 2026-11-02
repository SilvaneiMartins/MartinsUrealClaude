# Instalação no Windows — Martins Unreal Claude

Este guia explica como instalar e compilar o plugin no Windows com Unreal Engine 5.8.

## Pré-requisitos

- Unreal Engine 5.8 instalado.
- Visual Studio 2022 com a carga de trabalho **Desenvolvimento para jogos com C++**, MSVC e Windows SDK.
- Git para Windows.
- Node.js 18 ou superior, incluindo npm.
- Claude Code CLI instalado e autenticado.

Para instalar e autenticar a CLI do Claude Code, abra o PowerShell e execute:

```powershell
npm install -g @anthropic-ai/claude-code
claude auth login
claude --version
```

O comando de autenticação abre o navegador. Entre na sua conta Anthropic e confirme a instalação com `claude --version`.

## 1. Baixe o repositório

Abra o PowerShell e clone o repositório com o submódulo da ponte MCP:

```powershell
New-Item -ItemType Directory -Force C:\Projetos | Out-Null
Set-Location C:\Projetos
git clone --recurse-submodules https://github.com/SilvaneiMartins/MartinsUrealClaude.git
Set-Location .\MartinsUrealClaude
```

Se o repositório já foi clonado sem `--recurse-submodules`, inicialize o submódulo:

```powershell
git submodule update --init --recursive
```

Confirme que `UnrealClaude\Resources\mcp-bridge` contém os arquivos da ponte MCP.

## 2. Instale as dependências da ponte MCP

```powershell
Set-Location C:\Projetos\MartinsUrealClaude\UnrealClaude\Resources\mcp-bridge
npm install
```

As dependências são necessárias para as ferramentas MCP, incluindo as ferramentas de Blueprint.

## 3. Instale o plugin no projeto Unreal

Recomendamos instalar o plugin no projeto, em vez de na instalação do Unreal Engine.

1. Feche o Unreal Editor.
2. Crie a pasta `Plugins` na raiz do seu projeto, se ela ainda não existir.
3. Copie **somente** a pasta `UnrealClaude` do repositório para `Plugins` do projeto. Ajuste os caminhos abaixo para o seu projeto:

```powershell
$repo = "C:\Projetos\MartinsUrealClaude"
$projectPlugins = "C:\Projetos\MeuProjeto\Plugins"

New-Item -ItemType Directory -Force $projectPlugins | Out-Null
Copy-Item "$repo\UnrealClaude" "$projectPlugins\UnrealClaude" -Recurse
```

A estrutura deve ficar assim, sem uma pasta `UnrealClaude` duplicada:

```text
MeuProjeto/
├── MeuProjeto.uproject
└── Plugins/
    └── UnrealClaude/
        ├── UnrealClaude.uplugin
        ├── Resources/
        ├── Source/
        └── Config/
```

O arquivo `.uplugin` deve estar diretamente em `Plugins\UnrealClaude\UnrealClaude.uplugin`.

## 4. Gere os arquivos e compile o projeto

O plugin contém um módulo C++, portanto o projeto precisa ser compilado como projeto C++.

1. Se o projeto for somente Blueprint, abra-o e use **Tools → New C++ Class** para adicionar uma classe C++.
2. Feche o editor.
3. No Explorador de Arquivos, clique com o botão direito no arquivo `.uproject` e selecione **Generate Visual Studio project files**.
4. Abra a solução `.sln` gerada no Visual Studio.
5. Selecione a configuração **Development Editor** e a plataforma **Win64**.
6. Compile o projeto do jogo com **Build**.

Se o Unreal Editor avisar que há módulos ausentes ou compilados para outra versão, escolha **Sim** para recompilar. Se o projeto estiver associado a outra versão da Unreal, clique com o botão direito no `.uproject`, escolha **Switch Unreal Engine version** e selecione a instalação 5.8.

### Alternativa: compilar e empacotar somente o plugin

Também é possível empacotar o plugin com o Automation Tool. Ajuste os caminhos da Unreal Engine, do repositório e da pasta de saída:

```powershell
$ueRoot = "C:\Program Files\Epic Games\UE_5.8"
$plugin = "C:\Projetos\MartinsUrealClaude\UnrealClaude\UnrealClaude.uplugin"
$output = "C:\Projetos\Build\UnrealClaude"

& "$ueRoot\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin `
  "-Plugin=$plugin" `
  "-Package=$output" `
  -TargetPlatforms=Win64
```

Copie o conteúdo empacotado para `MeuProjeto\Plugins\UnrealClaude`. Se preferir instalar para todos os projetos, use a pasta `Engine\Plugins\Marketplace\UnrealClaude` da instalação da Unreal Engine.

## 5. Habilite e abra o plugin

1. Abra o projeto no Unreal Editor.
2. Em **Edit → Plugins**, procure por **Martins Unreal Claude**, na categoria **Editor**.
3. O plugin vem habilitado por padrão. Se estiver desabilitado, marque **Enabled** e reinicie o editor.
4. Abra **Tools → Claude Assistant**.

O servidor MCP inicia na porta `3000`. No **Output Log**, procure por `LogUnrealClaude` e pela mensagem de inicialização do servidor.

## Solução de problemas

### O plugin não aparece na lista

- Confirme que o arquivo está em `Plugins\UnrealClaude\UnrealClaude.uplugin`.
- Verifique se o plugin não foi copiado com uma pasta duplicada, como `Plugins\UnrealClaude\UnrealClaude\UnrealClaude.uplugin`.
- Gere novamente os arquivos do projeto e compile em **Development Editor / Win64**.
- Confira o **Output Log** e `Saved\Logs\MeuProjeto.log` em busca de erros relacionados a `UnrealClaude` ou `.uplugin`.

### Falha ao compilar

- Confirme que Visual Studio 2022 tem a carga de trabalho **Desenvolvimento para jogos com C++**, MSVC e Windows SDK instalados.
- Verifique se o projeto e a instalação do plugin usam Unreal Engine 5.8.
- Consulte a saída do build ou `Saved\Logs` para identificar o erro de compilação.
- Se houver resíduos de uma compilação anterior, feche o editor, remova as pastas `Intermediate` e `Binaries` do projeto e do plugin, gere os arquivos novamente e recompile.

### O Claude Code não é encontrado ou pede autenticação

Abra o PowerShell e execute `claude --version`. Se o comando não for encontrado, confirme a instalação global pelo npm e reinicie o Unreal Editor. Se for solicitada autenticação, execute `claude auth login`.

### As ferramentas MCP ou de Blueprint não funcionam

- Confira se `npm install` foi executado em `UnrealClaude\Resources\mcp-bridge`.
- Com o editor aberto, teste o servidor no PowerShell:
  ```powershell
  Invoke-RestMethod http://localhost:3000/mcp/status
  ```
- Confira o **Output Log** e procure mensagens `LogUnrealClaude`.

### Confirmar que o plugin carregou

No **Output Log** do editor, filtre por `LogUnrealClaude`. A inicialização bem-sucedida deve registrar que o servidor MCP está disponível em `http://localhost:3000`.
