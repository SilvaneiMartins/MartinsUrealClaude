# UnrealClaude

![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.8-313131?style=flat&logo=unrealengine&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-20-00599C?style=flat&logo=c%2B%2B&logoColor=white)
![Plataformas](https://img.shields.io/badge/Plataformas-Win64%20%7C%20Linux%20%7C%20macOS-0078D6?style=flat&logo=windows&logoColor=white)
![Claude Code](https://img.shields.io/badge/Claude%20Code-Integra%C3%A7%C3%A3o-D97757?style=flat&logo=anthropic&logoColor=white)
![MCP](https://img.shields.io/badge/MCP-20%2B%20ferramentas-8A2BE2?style=flat)
![Licença](https://img.shields.io/badge/Licen%C3%A7a-MIT-green?style=flat)

**Integração da CLI do Claude Code com o Unreal Engine 5.8** — tenha assistência de programação com contexto integrado da documentação do UE 5.8 diretamente no editor.

> **Plataformas compatíveis:** Windows (Win64), Linux e macOS (Apple Silicon). Compatível com o Claude Opus 4.7 e a versão correspondente do Claude Code.

## Visão geral

O UnrealClaude integra a [CLI do Claude Code](https://docs.anthropic.com/en/docs/claude-code) diretamente ao Editor do Unreal Engine 5.8 e executa um servidor MCP para permitir o acesso de outros agentes de programação.

**Principais recursos:**

- **Integração nativa com o editor** — painel de chat acoplado ao editor, respostas transmitidas em tempo real, chamadas de ferramentas agrupadas e blocos de código formatados.
- **Servidor MCP** — mais de 20 ferramentas do Model Context Protocol para manipular atores, editar Blueprints, gerenciar níveis, materiais, entradas e muito mais.
- **Contexto dinâmico do UE 5.8** — a ponte MCP carrega documentação precisa da API do UE 5.8 sob demanda.
- **Edição de Blueprints** — crie e modifique Blueprints, Animation Blueprints e máquinas de estado. Esse recurso ainda tem limitações; use com cautela.
- **Gerenciamento de níveis** — abra e crie níveis e modelos de mapas programaticamente.
- **Gerenciamento de assets** — pesquise assets e consulte dependências e referenciadores.
- **Fila de tarefas assíncronas** — operações demoradas não expiram por tempo limite.
- **Execução de scripts** — com sua autorização, o Claude pode escrever, compilar (por meio do Live Coding) e executar scripts.
- **Persistência de sessões** — o histórico de conversas é preservado entre sessões do editor.
- **Contexto do projeto** — coleta automaticamente informações do projeto (módulos, plugins e assets) e pode inspecionar as viewports do editor.
- **Autenticação do Claude Code** — não é necessário gerenciar uma chave de API separadamente.

## Pré-requisitos

### 1. Instale a CLI do Claude Code

```bash
npm install -g @anthropic-ai/claude-code
```

### 2. Autentique-se no Claude Code

```bash
claude auth login
```

Uma janela do navegador será aberta para autenticação com sua conta Anthropic (assinatura Claude Pro/Max) ou para configurar o acesso à API.

### 3. Verifique a instalação

```bash
claude --version
claude -p "Olá, você consegue me ver?"
```

## Instalação

Consulte a categoria **Editor** no navegador de plugins do editor. Talvez seja necessário rolar a lista para encontrá-la caso a busca não a exiba.

### Etapa 1: clone e compile

Este plugin precisa ser compilado a partir do código-fonte para sua plataforma e versão do Unreal Engine. Não há binários pré-compilados.

1. Clone este repositório, incluindo o submódulo da ponte MCP:
   ```bash
   git clone --recurse-submodules https://github.com/SilvaneiMartins/MartinsUrealClaude.git
   ```
   Se você já clonou sem `--recurse-submodules`, execute:
   ```bash
   cd MartinsUrealClaude
   git submodule update --init
   ```

2. Compile o plugin:

   **Windows:**
   ```bash
   Engine\Build\BatchFiles\RunUAT.bat BuildPlugin -Plugin="PATH\TO\UnrealClaude\UnrealClaude.uplugin" -Package="OUTPUT\PATH" -TargetPlatforms=Win64
   ```

   **Linux:**
   ```bash
   Engine/Build/BatchFiles/RunUAT.sh BuildPlugin -Plugin="/path/to/UnrealClaude/UnrealClaude.uplugin" -Package="/output/path" -TargetPlatforms=Linux
   ```

   **macOS:**
   ```bash
   Engine/Build/BatchFiles/RunUAT.sh BuildPlugin -Plugin="/path/to/UnrealClaude/UnrealClaude.uplugin" -Package="/output/path" -TargetPlatforms=Mac
   ```

### Etapa 2: instale o plugin

Copie o plugin compilado para a pasta de plugins do seu projeto ou da sua instalação do Unreal Engine.

> **Importante:** não clone este repositório diretamente em `YourProject/Plugins/`. O plugin está na subpasta `UnrealClaude/` (a que contém `UnrealClaude.uplugin`), enquanto a raiz do repositório também contém documentação que o Unreal não deve examinar. Copie somente essa subpasta para `YourProject/Plugins/UnrealClaude/UnrealClaude.uplugin` e regenere os arquivos do projeto.

**Opção A: plugin do projeto (recomendado)**

Copie o resultado da compilação para a pasta `Plugins` do projeto:

```text
YourProject/
├── Content/
├── Source/
└── Plugins/
    └── UnrealClaude/
        ├── Binaries/
        ├── Source/
        ├── Resources/
        ├── Config/
        └── UnrealClaude.uplugin
```

**Opção B: plugin do Unreal Engine (todos os projetos)**

Copie o plugin para a pasta de plugins da instalação do Unreal Engine:

**Windows:**
```text
C:\Program Files\Epic Games\UE_5.8\Engine\Plugins\Marketplace\UnrealClaude\
```

**Linux:**
```text
/path/to/UnrealEngine/Engine/Plugins/Marketplace/UnrealClaude/
```

### Etapa 3: instale as dependências da ponte MCP

Esta etapa é necessária para as ferramentas de Blueprint e a integração com o editor:

```bash
cd <PluginPath>/UnrealClaude/Resources/mcp-bridge
npm install
```

### Etapa 4: inicie o editor

Inicie o editor. O plugin será carregado automaticamente.

### Etapa 5 (opcional): use com a CLI ou o aplicativo do Claude

1. Inicie o Claude Code ou a CLI do Claude e aponte-o para a instalação do UnrealClaude.
2. Depois de confirmar que está tudo pronto, reinicie somente o Claude Code ou a CLI para que as alterações sejam aplicadas.

## Início rápido no macOS (Apple Silicon)

Para obter instruções completas, consulte [INSTALL_MAC.md](INSTALL_MAC.md).

1. **Instale o Node.js e a CLI do Claude Code:**
   ```bash
   brew install node
   npm install -g @anthropic-ai/claude-code
   claude
   ```
2. **Instale o plugin** na pasta `Plugins/` do projeto.
3. **Instale as dependências da ponte MCP:**
   ```bash
   cd YourProject/Plugins/UnrealClaude/Resources/mcp-bridge
   npm install
   ```
4. **Inicie o editor** e abra **Tools > Claude Assistant**.

## Início rápido no Linux (Rocky/Fedora)

Para obter instruções completas, consulte [INSTALL_LINUX.md](INSTALL_LINUX.md).

1. **Instale as bibliotecas:**
   ```bash
   sudo dnf install -y nss nspr mesa-libgbm libXcomposite libXdamage libXrandr alsa-lib pciutils-libs libXcursor atk at-spi2-atk pango cairo gdk-pixbuf2 gtk3
   ```
2. **Instale o suporte à área de transferência:**
   ```bash
   sudo dnf install -y wl-clipboard   # Wayland
   sudo dnf install -y xclip          # alternativa para X11
   ```
3. **Configure o Wayland:**
   ```bash
   export SDL_VIDEODRIVER=wayland
   export UE_Linux_EnableWaylandNative=1
   ```
4. **Compile e inicie o editor:**
   ```bash
   ./UnrealEditor -vulkan
   ```

## Uso

### Abrir o painel do Claude

No menu do editor, selecione **Tools → Claude Assistant**.

### Exemplos de solicitações

```text
Como criar um Actor Component personalizado em C++?

Qual é a melhor maneira de implementar um sistema de vida usando o GAS?

Explique o World Partition e como configurar o streaming para um mundo aberto.

Escreva uma função BlueprintCallable que gere partículas em uma posição.

Como usar corretamente TObjectPtr<> em vez de ponteiros brutos no UE 5.8?
```

### Atalhos de entrada

| Atalho | Ação |
|--------|------|
| `Enter` | Envia a mensagem |
| `Shift+Enter` | Insere uma nova linha |
| `Escape` | Cancela a solicitação atual |

## Recursos

### Persistência de sessões

As conversas são salvas automaticamente na pasta `Saved/UnrealClaude/` do projeto e restauradas quando o editor é aberto novamente. O contexto da conversa é mantido entre sessões.

### Contexto do projeto

O UnrealClaude coleta automaticamente informações sobre:

- Módulos de código-fonte e suas dependências
- Plugins habilitados
- Configurações do projeto
- Assets recentes
- Instruções personalizadas em `CLAUDE.md`

### Execução de scripts

O Claude pode escrever e executar scripts Python, comandos de console e scripts do Editor Utility, além de compilar C++ por meio do Live Coding. Cada execução depende de uma **caixa de diálogo de permissão**, negada por padrão. Consulte **Aprovação automática da execução de scripts** em [Configuração](#configuração).

> **Observação — ferramentas de script e console ficam ocultas por padrão.** A ponte MCP não anuncia ao modelo `execute_script`, `run_console_command`, `cleanup_scripts` nem `get_script_history`. Essas ferramentas avançadas ficam desativadas nas versões publicadas para que o modelo use as ferramentas estruturadas do editor e para manter a lista de ferramentas enxuta. Ainda é possível chamá-las pelo nome exato, e toda execução continua sujeita à caixa de diálogo de permissão do C++. Para disponibilizá-las ao modelo, remova esses nomes de `HIDDEN_TOOL_NAMES` em `Resources/mcp-bridge/tool-router.js`. **`run_console_command` pode acessar o interpretador Python (`py …`); removê-la da lista de ocultas permite a execução de código arbitrário. Habilite-a somente em ambientes confiáveis.**

### Servidor MCP

O plugin inclui um servidor do Model Context Protocol (MCP) com mais de 20 ferramentas que expõem funcionalidades do editor ao Claude e a ferramentas externas. Por padrão, o servidor MCP usa a porta 3000 e é iniciado automaticamente quando o editor é carregado.

**Categorias de ferramentas:**

- **Atores** — cria, move, exclui, inspeciona e altera propriedades de atores.
- **Gerenciamento de níveis** — abre níveis, cria níveis a partir de modelos e lista os modelos disponíveis.
- **Blueprints** — cria e modifica Blueprints, variáveis, funções, nós e pinos.
- **Animation Blueprints** — edita máquinas de estado, estados, transições, condições e operações em lote.
- **Assets** — pesquisa assets e consulta dependências e referenciadores com paginação.
- **Personagens** — configura personagens, ajustes de movimento e consultas de dados.
- **Materiais** — opera em materiais e instâncias de materiais.
- **Enhanced Input** — gerencia ações de entrada e contextos de mapeamento.
- **Utilitários** — oferece comandos de console, Output Log, captura da viewport e execução de scripts.
- **Fila de tarefas assíncronas** — executa em segundo plano operações demoradas.

Para consultar a documentação completa das ferramentas MCP, incluindo parâmetros, exemplos e detalhes da API, acesse o repositório da [ponte MCP do UnrealClaude](https://github.com/SilvaneiMartins/ue5-mcp-bridge-martins).

#### Sistema de contexto dinâmico do UE 5.8

A ponte MCP inclui um carregador de contexto dinâmico com documentação precisa da API do UE 5.8 sob demanda. Use `unreal_get_ue_context` para consultar categorias (como animação, Blueprint, Slate, ator, assets e replicação) ou pesquisar por palavras-chave. O estado do contexto aparece na saída de `unreal_status`.

## Configuração

### Configurações do projeto

Em **Project Settings → Plugins → Unreal Claude**:

- **Aprovação automática da execução de scripts** (desativada por padrão) — quando ativada, todo script Python, C++, de console ou Editor Utility iniciado pela ponte MCP ou pelo chat integrado é executado sem exibir a caixa de diálogo de permissão. Cada execução aprovada automaticamente gera uma entrada `LogUnrealClaude` com seu tipo e descrição para manter o registro de auditoria. Esse recurso foi pensado para fluxos de trabalho confiáveis com MCP ou agentes, nos quais a confirmação de cada script gera atrito. **Ative-o somente em máquinas e projetos cujos clientes conectados sejam confiáveis.**

A configuração é salva em `Config/DefaultEditor.ini`, na seção `[/Script/UnrealClaude.UnrealClaudeSettings]`.

### Prompts de sistema personalizados

Você pode ampliar o contexto integrado do UE 5.8 criando um arquivo `CLAUDE.md` na raiz do projeto:

```markdown
# Contexto do meu projeto

## Arquitetura
- Este é um jogo de sobrevivência multiplayer
- Usa o modelo Dedicated Server
- Usa GAS para todas as habilidades

## Padrões de código
- Sempre use UPROPERTY para acesso pelo Blueprint
- Prefixe interfaces com I (IInteractable)
- Use GameplayTags para identificar habilidades
```

### Ferramentas permitidas

Por padrão, o plugin permite que o Claude use: `Read`, `Write`, `Edit`, `Grep`, `Glob` e `Bash`. Você pode alterar essa lista em `ClaudeSubsystem.cpp`:

```cpp
Config.AllowedTools = { TEXT("Read"), TEXT("Grep"), TEXT("Glob") }; // Somente leitura
```

## Como funciona

1. O usuário envia uma solicitação pelo widget do editor.
2. O plugin reúne o contexto do UE 5.8 e as informações do projeto.
3. O plugin executa `claude -p --skip-permissions --append-system-prompt "..." "sua solicitação"`.
4. O Claude Code é executado com a pasta do projeto como diretório de trabalho.
5. A resposta é capturada e exibida no painel de chat.
6. A conversa é salva para sessões futuras.

### Comando equivalente no terminal

```bash
cd "C:\YourProject"
claude -p --skip-permissions \
  --allowedTools "Read,Write,Edit,Grep,Glob,Bash" \
  --append-system-prompt "Você é especialista em desenvolvimento com Unreal Engine 5.8..." \
  "Como criar um GameMode personalizado?"
```

## Solução de problemas

### “Claude CLI não encontrada”

1. Verifique se o Claude está instalado: `claude --version`
2. Confira se ele está no `PATH`: `where claude`
3. Reinicie o Unreal Editor após a instalação.

### “Autenticação necessária”

Execute `claude auth login` em um terminal para se autenticar.

### Respostas lentas

O Claude Code é executado na pasta do projeto e pode ler arquivos para obter contexto. Projetos grandes podem demorar mais na primeira resposta.

A quantidade de plugins globais do Claude Code também pode afetar o desempenho (por exemplo, Superpowers, ralp-loop e context7). O contexto injetado por esses plugins pode aumentar o tempo de resposta em mais de três minutos.

### O plugin não compila

Verifique se você está usando o Unreal Engine 5.8. As plataformas compatíveis são Windows (Win64), Linux e macOS.

### O servidor MCP não inicia

Verifique se a porta 3000 está disponível. O servidor MCP registra mensagens em `LogUnrealClaude`.

### Ferramentas MCP indisponíveis ou ferramentas de Blueprint sem funcionar

Se o Claude informar que as ferramentas MCP estão nas instruções, mas não aparecerem na lista de funções:

1. **Instale as dependências da ponte MCP.** A causa mais comum é a ausência dos pacotes do npm:
   ```bash
   cd YourProject/Plugins/UnrealClaude/Resources/mcp-bridge
   npm install
   ```
2. **Verifique se o servidor HTTP está em execução.** Com o editor aberto, teste:
   ```bash
   curl http://localhost:3000/mcp/status
   ```
   A resposta deve conter um JSON com informações do projeto.
3. **Confira o Output Log.** Procure mensagens `LogUnrealClaude`:
   - `MCP Server started on http://localhost:3000` — o servidor está em execução.
   - `Registered X MCP tools` — as ferramentas foram carregadas.
4. **Reinicie o editor** após instalar as dependências do npm.

### Solicitação travada

O novo watchdog deve alertar sobre solicitações travadas após 60 segundos. Verifique se todos os arquivos estão no disco e desative serviços de sincronização, como OneDrive e Dropbox, para os arquivos do projeto.

### Depuração da ponte MCP

A ponte MCP também está disponível como [repositório independente](https://github.com/SilvaneiMartins/ue5-mcp-bridge-martins) e possui uma suíte de testes Vitest. Para investigar problemas na ponte (lista de ferramentas, tradução de parâmetros ou injeção de contexto), execute os testes separadamente:

```bash
cd path/to/ue5-mcp-bridge
npm install
npm test
```

Os testes verificam a ponte sem exigir que o Unreal Editor esteja em execução.

## Contribuição

Sinta-se à vontade para criar um fork e adaptar o projeto às suas necessidades. Algumas áreas que podem ser aprimoradas:

- [x] Suporte a Linux (agradecimentos a [@bearyjd](https://github.com/bearyjd))
- [x] Suporte a macOS (agradecimentos a [@lateralsummer](https://github.com/lateralsummer))
- [ ] Ferramentas MCP adicionais (as ferramentas atuais precisam ser refatoradas; novas ferramentas não são prioridade no momento)

## Licença

Licença MIT — consulte o arquivo [LICENSE](UnrealClaude/LICENSE).

## Créditos

- Desenvolvido para o Unreal Engine 5.8.
- Integra-se ao [Claude Code](https://claude.ai/code), da Anthropic.
