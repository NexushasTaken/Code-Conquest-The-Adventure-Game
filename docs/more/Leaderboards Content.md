# Leaderboards

## Overview
Leaderboards track player performance across different aspects of the game, from individual levels to overall coding mastery. This system encourages learning and improvement through friendly competition.

## Per-Level Scores

### Speed

**Description**: Time taken to finish the level

**Calculation**:
$\text{Speed} = \text{End Time} - \text{Start Time}$

**Rules**:
- Only valid if the final submitted code has no errors
- Measured in seconds
- Lower values are better
- Tracks fastest level completion times

### Accuracy

**Description**: How many mistakes you made

**Calculation**:
$\text{Accuracy} = (\text{Correct Lines}) - (\text{Error Count})$

**Rules**:
- Counts all syntax and logic errors
- Higher values are better
- Measures code quality and correctness

### Efficiency

**Description**: How clean your solution is

**Calculation**:
$\text{Efficiency} = \frac{\text{Enemy HP}}{\text{Tiles Used}}$

**Rules**:
- Only counts tiles in final submitted code
- Higher values are better
- Rewards concise and effective solutions

## Normalized Scores (0-100 Scale)

### Normalized Speed

**Description**: How close you are to the world record

**Calculation**:
$\text{Normalized Speed} = 100 \times \frac{\text{Your Time}}{\text{Fastest Time}}$

**Rules**:
- 100 = world record
- 50 = twice as slow as record
- 0 = took much longer than record
- Used for level rankings

### Normalized Accuracy

**Description**: How close you are to perfect accuracy

**Calculation**:
$\text{Normalized Accuracy} = 100 \times \frac{\text{Your Accuracy}}{\text{Best Accuracy}}$

**Rules**:
- 100 = perfect accuracy
- 0 = made errors in every line
- Used for level rankings

### Normalized Efficiency

**Description**: How close you are to the most efficient solution

**Calculation**:
$\text{Normalized Efficiency} = 100 \times \frac{\text{Your Efficiency}}{\text{Best Efficiency}}$

**Rules**:
- 100 = used fewest tiles
- 0 = used many more tiles than needed
- Used for level rankings

## Mastery Scores

### Concept Proficiency

**Description**: Your average skill in a programming topic

**Calculation**:
$\text{Concept Mastery} = \frac{\text{Sum of Normalized Scores}}{3 \times \text{Number of Levels}}$

**Rules**:
- Includes Normalized Speed, Accuracy and Efficiency
- Max 100 points per level
- Average of all levels in the concept
- Used for concept rankings
- Tracks progress in specific programming topics (Variables, If-statements, Loops, etc.)

### Overall Mastery

**Description**: Your overall coding skill

**Calculation**:
$\text{Grand Mastery} = \frac{\text{Sum of Concept Proficiency}}{\text{Number of Concepts}}$

**Rules**:
- Average of all concept proficiencies
- Max 100 points total
- Only counts concepts you've attempted
- Used for global rankings
- Represents overall programming ability

## Leaderboard Types

1. **Level Leaderboards**
   - Ranks players on specific levels
   - Uses: Normalized Speed, Accuracy, and Efficiency scores
   - Shows best performance for each level
   - Encourages replay and improvement

2. **Concept Leaderboards**
   - Ranks players by programming topic
   - Uses: Concept Proficiency score
   - Shows mastery of specific programming concepts
   - Tracks progress in learning different topics

3. **Global Leaderboard**
   - Ranks overall player skill
   - Uses: Overall Mastery score
   - Shows best overall programmers
   - Represents complete game progress
