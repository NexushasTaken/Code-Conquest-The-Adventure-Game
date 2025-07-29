# Code Conquest - The Adventure Game

Read [Document](./docs/README.md) for game documents

## How to setup the game?

Follow the steps and instruction below to properly setup the game development.

**You can't run the game on your Phone, PC is required.**

You need to install `git` and `uv` to setup the Development Environment for VSCode.

Assuming you already have VSCode installed, so we will be installing `git` and `uv`.

Go to [Git Website](https://git-scm.com/downloads), look for **Downloads**, and click **Windows**, find **Git for Windows/x64 Setup**, click it and the *installer* will be download automatically, wait for it until it finished downloading.

After the installer was downloaded, run it:

- Just click **Next** until you see "Choose the Default Editor used by Git", there should be choices to what Editor you will use:

 - Choose "Use Visual Studio Code as Git's default editor"

 - After that, just click **Next** until it installs, and click **Finish**.

Launch VSCode, if you can't find it, press "Windows" Key on your keyboard, and search for "VSCode" or "Visual Studio Code".

After VSCode is opened, press "Ctrl + Shift + `" on your keyboard, and a terminal(or Command Prompt) should open.

> the backtick or "`" is located above the "Tab" on your keyboard.

Enter this command:

```bash
powershell -ExecutionPolicy Bypass -c "irm https://astral.sh/uv/install.ps1 | iex"
```

> Entering this command should install **uv** software, which is a python package and package manager.

> If you don't believe me, see it for your self, go to this website [uv Github Website](https://github.com/astral-sh/uv?tab=readme-ov-file)

Next, close VSCode and launch it again.

Press "Ctrl + K + O", and find the folder named "Desktop", click it, and click "Select Folder"("Select folder" should be located on bottom right of the window that opens after you pressed "Ctrl + K + O").

Press "Ctrl + Shift + `" on your keyboard, to open the Command Prompt.

Clone this repository by entring on the Command Prompt:

```bash
git clone --depth 1 https://github.com/NexushasTaken/Code-Conquest-The-Adventure-Game "Code Conquest - The Adventure Game"
```

> Entering this command should download all necessary files for the Development Environment of this Project, which is a Game.

> On the Desktop folder, you should see a folder named "Code Conquest - The Adventure Game", you should open VSCode inside this folder, read the next sentence for the next step.

Press "Ctrl + K + O", and find the folder named "Desktop", inside of it, you should see "Code Conquest - The Adventure Game", click it or double click it, and click "Select Folder".

Press "Ctrl + Shift + `" on your keyboard, to open the Command Prompt.

Enter this command:

```bash
uv sync
```

> This command should download python packages required to run the game.

> This command should also download python if you dont have it installed yet.

After the command is done, the setup is complete.

## But how to actually run the Game?

This step is always necessary whenever you close the VSCode or the terminal, so that you can actually run the game.

Enter this on Command Prompt:

```bash
.\.venv\Scripts\activate
```

> What if there is an error after running the command? then you should run this too:

```bash
Set-ExecutionPolicy -Scope CurrentUser RemoteSigned
```

and then run the command again:

```bash
.\.venv\Scripts\activate
```

> after you entered it, you should see "(code-conquest)", indicating that you can run the game.

To run the game, simply just enter:

```bash
uv run .\src\main.py
```

> Entering this command should open a window where you can see the game itself.

> As of writing this, you should only see a Login Screen.

There is possibility that after running `uv run .\src\main.py`, you will see this error message:

```sh
Traceback (most recent call last):
  File "/home/nexus/dev/remote/Code-Conquest-The-Adventure-Game/./src/main.py", line 209, in <module>
    main()
    ~~~~^^
  File "/home/nexus/dev/remote/Code-Conquest-The-Adventure-Game/./src/main.py", line 203, in main
    game = GameView()
  File "/home/nexus/dev/remote/Code-Conquest-The-Adventure-Game/./src/main.py", line 37, in __init__
    self.client = setup_client()
                  ~~~~~~~~~~~~^^
  File "/home/nexus/dev/remote/Code-Conquest-The-Adventure-Game/./src/main.py", line 24, in setup_client
    raise RuntimeError("SUPABASE_URL and SUPABASE_KEY must be set")
RuntimeError: SUPABASE_URL and SUPABASE_KEY must be set
```

That's because, you need the `.env` **file** that is located on inside "Code Conquest - The Adventure Game".

the file must contain a string with the name of `SUPABASE_URL` and `SUPABASE_KEY`.

Ask your leader for the actual contents of the `.env` file.

The contents of the file must be similar to this:

```sh
SUPABASE_URL="<supabase-project-url>"
SUPABASE_KEY="<supabase-project-key>"
```

> why i didn't include `.env` file on this github repository? it's for security reason, since, this repository is public, meaning any people can access repository, and these SUPABASE_URL and SUPABASE_KEY is similar to password, but for Supabase projects.

