# System Project Proposal

## Code Conquest: The Adventure Game

### Description

Code Conquest is an educational game that teaches programming concepts through an engaging adventure format. Players learn to code by solving challenges and battling enemies using programming concepts. The game uses a tile-based coding system where players connect code tiles to write Python code, solve puzzles, and engage in turn-based combat with enemies that represent coding challenges.

### Target Users

- Students learning programming for the first time
- Self-learners seeking a fun and structured way to learn coding
- Educators looking for an interactive tool to teach programming concepts

### Target Deliverables

#### 1. Tile-Based Coding System

- **Core Mechanics**
  - Players connect tiles to write valid Python code
  - Each tile represents a programming element (keywords, operators, etc.)
  - Tiles are unlocked progressively as new concepts are introduced
  - Connected tiles form complete code statements

- **Learning Progression**
  - Early levels introduce basic concepts (variables, data types)
  - Later levels introduce more complex concepts (loops, functions)
  - Each concept is reinforced through multiple levels
  - Players must demonstrate understanding to progress

#### 2. Turn-Based Gameplay

- **Code Execution**
  - Players write code to attack enemies
  - Valid code triggers effects (damage, buffs, puzzle solving)
  - Invalid code results in enemy attacks
  - Real-time feedback on code execution

- **Turn System**
  - Player always moves first
  - Enemy responds after code execution
  - Health point system for both player and enemy
  - Level reset on player defeat

#### 3. Level Design

- **Concept Scenarios**
  - Structured sequences teaching specific programming concepts
  - Progressive difficulty increase
  - Practice, challenge, and story-based levels
  - Clear learning objectives for each level

- **Entities**
  - Enemies: Represent coding challenges
  - Allies: Introduce new concepts and provide guidance
  - Bosses: Test mastery of multiple concepts

#### 4. Feedback Systems

- **Code Analysis**
  - Real-time syntax checking
  - Analyzer runs after user inactivity
  - No automatic code modification
  - Encourages learning through manual correction

- **Help System**
  - Hints and tips for fixing errors
  - Real-time error messages
  - Inspired by Rust's Error ID system

#### 5. Progression Features

- **Battle Boosters**
  - Player Resurrection
  - Syntax Shield

- **Enhanced Coding**
  - Tile compression after concept mastery
  - Faster code writing in later levels
  - Unlocks based on proficiency

#### 6. Analytics and Tracking

- **Concept Mastery**
  - Tracks understanding of programming concepts
  - Measures performance across levels
  - Identifies areas for improvement

- **Level Progress**
  - Completed levels
  - Encountered enemies
  - Collected items
  - Player-written code

### Integrated System: Supabase Service

#### Implementation of Services

- **Player Profiles**
  - Achievements and milestones
  - Concept mastery tracking
  - Performance analytics
  - Customization options

- **Leaderboards**
  - Level rankings (speed, accuracy, efficiency)
  - Concept rankings (programming topic mastery)
  - Global rankings (overall coding skill)

- **Account System**
  - Secure authentication
  - Cloud-based progress saving
  - Cross-device synchronization
  - Seamless experience across devices
