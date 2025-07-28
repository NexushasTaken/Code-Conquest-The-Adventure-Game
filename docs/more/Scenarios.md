# Scenarios

## Variables & Data Types

### Making a Scenario

For players to retain what they've learned, multiple scenarios for each concept are essential. Each data type should have one or more dedicated scenarios.

Template:

```markdown
### Scenario #X: [Concept Name] - [Specific Challenge]

**Enemy Profile**  

- **Name**: [Creative Name]  
- **HP**: [Value]  
- **Base Damage**: [Value]  

**Player Stats**  

- **HP**: [Value]  

#### **Objective**  

[Clear 1-2 sentence goal]

#### **Mechanics**  

**Valid Moves**:  

- [Condition 1: What makes code valid?]  
- [Condition 2: Additional constraints]  

**Damage Formula**:  
`[Formula]`

#### **Penalties**  

**Common Pitfalls**:  

- [Specific mistakes to avoid]  

#### **Examples**  

[Turn Examples]
```

**Base Damage** is the default offensive attack damage of the enemy.

---

### Scenario #1: Integer Basics - Attack with Integers

**Enemy Profile**  

- **Name**: `NullInt`  
- **HP**: 50  
- **Base Damage**: 10  

**Player Stats**  

- **HP**: 100  

#### **Objective**  

Use integer values to attack the enemy, making sure the attack values follow the rules.

#### **Mechanics**  

**Valid Moves**:  

- Assign an integer value to `attack`.
- Each turn, the attack value must differ from previous turns.
- Valid when `attack <= 25`.

**Damage Formula**:  
`Damage = attack`

#### **Penalties**  

**Common Pitfalls**:  

- **Overpowered** (`attack > 25`): The enemy counterattacks for `base_damage + attack`.
- **Repeated value**: If the same value is used twice in a row, the enemy counterattacks for `base_damage + attack`.

#### **Examples**  

- Valid: `attack = 5` -> 5 damage
- Overpowered: `attack = 30` -> enemy counters for 40 damage
- Repeated: `attack = 5` (after previous 5) -> enemy counters for 15 damage

---

### Scenario #2: Fibonacci Sequence - Attack with Fibonacci Numbers

**Enemy Profile**  

- **Name**: `FiboNull`  
- **HP**: 60  
- **Base Damage**: 10  

**Player Stats**  

- **HP**: 100  

#### **Objective**  

Use Fibonacci numbers to attack the enemy.

#### **Mechanics**  

**Valid Moves**:  

- The attack must follow the Fibonacci sequence (1, 1, 2, 3, 5, 8, 13, 21, 34...).
- Damage is calculated as `(current_fib * position) + 1`.

**Damage Formula**:  
`Damage = (current_fib * position) + 1`

#### **Penalties**  

**Common Pitfalls**:  

- **Non-Fibonacci number**: If the *number* used is not part of the Fibonacci sequence, the enemy counterattacks for that same *number* damage.

#### **Examples**  

- Turn 1: `attack = 1` -> `(1 * 1) + 1 = 2` damage
- Turn 2: `attack = 1` -> `(1 * 2) + 1 = 3` damage
- Turn 3: `attack = 2` -> `(2 * 3) + 1 = 7` damage
- Invalid: `attack = 4` -> enemy counters for 4 damage

---

### Scenario #3: Floating Point Precision - Manage Precision in Attacks

**Enemy Profile**  

- **Name**: `FloatFiend`  
- **HP**: 30  
- **Base Damage**: 10  

**Player Stats**  

- **HP**: 30  

#### **Objective**  

Assign float values to `attack` while maintaining precision and staying within the valid range.

#### **Mechanics**  

**Valid Moves**:  

- Assign a float value to `attack` with ≤ 2 decimal places.
- The `attack` value must be within the range of `0.5 <= attack <= 5.5`.

**Damage Formula**:  
`Damage = attack * 10`

#### **Penalties**  

**Common Pitfalls**:  

- **Out of range**: If `attack` is outside the valid range, the enemy counterattacks for `attack` damage.
- **Too precise**: If `attack` has more than two decimal places, the enemy counterattacks for `base_damage`.

#### **Examples**  

- Valid: `attack = 1.25` -> 12.5 damage
- Invalid range: `attack = 6.0` -> enemy counters for 6 damage
- Too precise: `attack = 1.234` -> enemy counters for 10 damage

---

### Scenario #4: Boolean Trap Logic - Solve Boolean Traps

**Enemy Profile**  

- **Name**: `TrueLie`  
- **HP**: 40  
- **Base Damage**: 10  

**Player Stats**  

- **HP**: 50  

#### **Objective**  

Determine the truth value of statements and attack accordingly.

#### **Mechanics**  

**Valid Moves**:  

- The enemy will make a statement each turn.
- Attack by determining if the statement is true or false.

**Damage Formula**:  
`Damage = (int(attack) + 1) * 10`

- `True` -> 20 damage
- `False` -> 10 damage

#### **Penalties**  

**Common Pitfalls**:  

- **Incorrect boolean**: If the boolean is incorrect, the enemy counterattacks for `base_damage + 5`.

#### **Examples**  

1. "The sun is a planet." -> `attack = False` -> 10 damage
2. "Water boils at 100°C." -> `attack = True` -> 20 damage
3. Wrong answer -> enemy counters for 15 damage

---

### Scenario #5: String Echo Fight - Match the String

**Enemy Profile**  

- **Name**: `EchoLure`  
- **HP**: 40  
- **Base Damage**: 10  

**Player Stats**  

- **HP**: 50  

#### **Objective**  

Attack by matching the string exactly as shown by the enemy.

#### **Mechanics**  

**Valid Moves**:  

- Attack by matching the enemy's string exactly, including case and spelling.

**Damage Formula**:  
`Damage = len(word) * 2`

#### **Penalties**  

**Common Pitfalls**:  

- **Mismatch**: If the string doesn't match, the enemy counterattacks for `base_damage + len(word)`.
- **Empty string**: If the string is empty, the enemy counters for `base_damage`.

#### **Examples**  

- Enemy: "hello" -> `word = "hello"` -> 10 damage
- Wrong case: `word = "Hello"` -> enemy counters for 15 damage
- Empty: `word = ""` -> enemy counters for 10 damage

---

### Scenario #6: List Formation - Create a List to Attack

**Enemy Profile**  

- **Name**: `SwarmBug`  
- **HP**: 60  
- **Base Damage**: 8  

**Player Stats**  

- **HP**: 60  

#### **Objective**  

Create a list with the correct number of elements to deal damage.

#### **Mechanics**  

**Valid Moves**:  

- Create a list named `targets`.
- The list should contain the correct number of elements.

**Damage Formula**:  
`Damage = len(targets) * 4`

#### **Penalties**  

**Common Pitfalls**:  

- **Wrong length**: If the list length is incorrect, the enemy counterattacks for `base_damage + (2 * |expected - actual|)`.
- **Not a list**: If the variable is not a list, the enemy counterattacks for `base_damage + 5`.

#### **Examples**  

1. "3 bugs!" -> `targets = [1, 2, 3]` -> 12 damage
2. "1 bug!" -> `targets = ['x']` -> 4 damage
3. Wrong length -> enemy counters based on difference

---

### Scenario #7: Tuple Lock Code - Solve the Tuple Puzzle

**Enemy Profile**  

- **Name**: `LockWorm`  
- **HP**: 50  
- **Base Damage**: 10  

**Player Stats**  

- **HP**: 50  

#### **Objective**  

Create a sorted tuple to unlock the enemy's defenses.

#### **Mechanics**  

**Valid Moves**:  

- Create a tuple named `code`.
- The numbers in the tuple must be in ascending order.

**Damage Formula**:  
`Damage = sum(code)`

#### **Penalties**  

**Common Pitfalls**:  

- **Unsorted**: If the numbers are not in ascending order, the enemy counterattacks for `base_damage + position_of_first_error`.
- **Not a tuple**: If the variable is not a tuple, the enemy counterattacks for `base_damage + 3`.

#### **Examples**  

1. "7, 2, 5" -> `code = (2, 5, 7)` -> 14 damage
2. "3, 3, 1" -> `code = (1, 3, 3)` -> 7 damage
3. Unsorted -> enemy counters based on error position

---

### Scenario #8: LetterCount Curse - Count Letters to Attack

**Enemy Profile**  

- **Name**: `GlyphGhoul`  
- **HP**: 50  
- **Base Damage**: 10  

**Player Stats**  

- **HP**: 60  

#### **Objective**  

Count the frequency of letters in the enemy's glyph strings and attack accordingly.

#### **Mechanics**  

**Valid Moves**:  

- Create a dictionary named `countmap` where:
  - Keys represent unique letters.
  - Values represent the occurrence counts of each letter.

**Damage Formula**:  
`Damage = sum(countmap.values()) * 2`

#### **Penalties**  

**Common Pitfalls**:  

- **Wrong counts**: If the counts are incorrect, the enemy counterattacks for `sum(values)`.
- **Missing keys**: For every missing key, the enemy deals +2 damage.
- **Wrong type**: If the dictionary contains invalid types, the enemy deals +5 damage.

#### **Examples**  

1. "aabbcc" -> `countmap = {'a': 2, 'b': 2, 'c': 2}` -> 12 damage
2. Missing 'b' -> enemy counters for sum + penalties
